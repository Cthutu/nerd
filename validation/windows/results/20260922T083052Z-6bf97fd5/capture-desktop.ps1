param(
    [string[]]$Examples = @('pixels', 'dungeon', 'triangle'),
    [string[]]$Compilers = @('debug', 'release')
)
# Capture only the owned test windows, with native input and normal shutdown.
# Run after benchmarks; no builds are performed here.
$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '../../../..')).Path
$matrixRoot = $PSScriptRoot
$matrix = Get-Content (Join-Path $matrixRoot 'desktop.json') -Raw | ConvertFrom-Json -AsHashtable
$captureRoot = Join-Path $PSScriptRoot 'desktop-captures'
New-Item -ItemType Directory -Force $captureRoot | Out-Null
Add-Type -AssemblyName System.Drawing
Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class NerdDesktopProbe {
 [StructLayout(LayoutKind.Sequential)] public struct Rect { public int Left,Top,Right,Bottom; }
 [StructLayout(LayoutKind.Sequential)] public struct Point { public int X,Y; }
 [StructLayout(LayoutKind.Explicit, Size=20)] public struct Input {
  [FieldOffset(0)] public ushort Type;
  [FieldOffset(4)] public int Down;
  [FieldOffset(8)] public ushort Repeat;
  [FieldOffset(10)] public ushort Virtual;
  [FieldOffset(12)] public ushort Scan;
  [FieldOffset(14)] public ushort Character;
  [FieldOffset(16)] public uint Control;
 }
 [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr w,out Rect r);
 [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr w,out Rect r);
 [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr w,ref Point p);
 [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr w,IntPtr dc,uint flags);
 [DllImport("user32.dll")] public static extern bool MoveWindow(IntPtr w,int x,int y,int width,int height,bool repaint);
 [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr w,uint m,IntPtr v,IntPtr l);
 [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr w);
 [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
 [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr w,int command);
 [DllImport("kernel32.dll")] public static extern bool AttachConsole(uint pid);
 [DllImport("kernel32.dll")] public static extern bool FreeConsole();
 [DllImport("kernel32.dll", CharSet=CharSet.Unicode)] static extern IntPtr CreateFile(string path,uint access,uint share,IntPtr sa,uint mode,uint flags,IntPtr template);
 [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);
 [DllImport("kernel32.dll", CharSet=CharSet.Unicode)] static extern bool WriteConsoleInputW(IntPtr h,Input[] input,uint count,out uint written);
 public static bool ConsoleKey(uint pid,ushort key,ushort character) {
  // This probe runs in its own PowerShell process. Detach that process from
  // its inherited console before temporarily attaching to the owned game.
  FreeConsole();
  if (!AttachConsole(pid)) return false;
  try {
   IntPtr h=CreateFile("CONIN$",0xC0000000,3,IntPtr.Zero,3,0,IntPtr.Zero);
   if(h==new IntPtr(-1)) return false;
   try {
    Input down=new Input { Type=1,Down=1,Repeat=1,Virtual=key,Character=character,
                           Scan=(ushort)(key==0x20 ? 0x39 : 0x10) };
    Input up=down; up.Down=0; uint written;
    bool pressed=WriteConsoleInputW(h,new[]{down},1,out written) && written==1;
    System.Threading.Thread.Sleep(100);
    return WriteConsoleInputW(h,new[]{up},1,out written) && written==1 && pressed;
   } finally { CloseHandle(h); }
  } finally { FreeConsole(); }
 }
}
'@

function Save-WindowCapture([IntPtr]$Window, [string]$Name) {
    $rect = New-Object NerdDesktopProbe+Rect
    if (-not [NerdDesktopProbe]::GetClientRect($Window, [ref]$rect)) { throw 'No client rectangle' }
    $bitmap = New-Object System.Drawing.Bitmap($rect.Right, $rect.Bottom)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    try {
        $dc = $graphics.GetHdc()
        try { $printed = [NerdDesktopProbe]::PrintWindow($Window, $dc, 3) }
        finally { $graphics.ReleaseHdc($dc) }
        $colours = [System.Collections.Generic.HashSet[int]]::new()
        for ($y=0; $y -lt $rect.Bottom; $y+=8) {
            for ($x=0; $x -lt $rect.Right; $x+=8) { $null = $colours.Add($bitmap.GetPixel($x,$y).ToArgb()) }
        }
        $method = 'PrintWindow client capture'
        if (-not $printed -or $colours.Count -lt 16) {
            # GPU-backed windows may not implement PrintWindow. Require the
            # exact owned window in front before copying its client rectangle.
            [NerdDesktopProbe]::ShowWindow($Window,9) | Out-Null
            [NerdDesktopProbe]::SetForegroundWindow($Window) | Out-Null
            Start-Sleep -Milliseconds 200
            if ([NerdDesktopProbe]::GetForegroundWindow() -ne $Window) { throw 'Owned window could not be foregrounded for GPU capture' }
            $point = New-Object NerdDesktopProbe+Point
            [NerdDesktopProbe]::ClientToScreen($Window,[ref]$point) | Out-Null
            $graphics.CopyFromScreen($point.X,$point.Y,0,0,$bitmap.Size)
            $method = 'Foreground client capture'
            $colours.Clear()
            for ($y=0; $y -lt $rect.Bottom; $y+=8) {
                for ($x=0; $x -lt $rect.Right; $x+=8) { $null = $colours.Add($bitmap.GetPixel($x,$y).ToArgb()) }
            }
        }
        $path = Join-Path $captureRoot ($Name + '.png')
        $bitmap.Save($path)
        return @{ image = 'desktop-captures/' + $Name + '.png'; method=$method;
                  width=$rect.Right; height=$rect.Bottom; sampled_colours=$colours.Count;
                  sha256=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash }
    } finally { $graphics.Dispose(); $bitmap.Dispose() }
}

$reportPath = Join-Path $PSScriptRoot 'desktop-automated.json'
$reports = if (Test-Path $reportPath) { Get-Content $reportPath -Raw | ConvertFrom-Json -AsHashtable } else { @{} }
foreach ($key in ($matrix.cases.Keys | Sort-Object)) {
    $case = $matrix.cases[$key]
    if ($case.example -notin $Examples -or $case.compiler -notin $Compilers) { continue }
    $result = @{ status='RUNNING'; compiler=$case.compiler; example=$case.example;
                 target=$case.target; jobs=$case.jobs; backend=$case.backend;
                 compiler_sha256=$case.compiler_sha256; observed_utc=(Get-Date).ToUniversalTime().ToString('o') }
    $process = $null
    try {
        $program = Join-Path $matrixRoot ('scratch/manual/' + $key + '.exe')
        $result.program_sha256 = (Get-FileHash -LiteralPath $program -Algorithm SHA256).Hash
        $cwd = Join-Path $repoRoot ('examples/' + $case.example)
        if ($case.example -eq 'dungeon') {
            $process = Start-Process -FilePath (Join-Path $env:WINDIR 'System32/conhost.exe') -ArgumentList ('"' + $program + '"') -WorkingDirectory $cwd -WindowStyle Normal -PassThru
            $deadline = (Get-Date).AddSeconds(10)
            do {
                Start-Sleep -Milliseconds 100
                $game = Get-Process -Name $key -ErrorAction SilentlyContinue | Where-Object Path -EQ $program
            } while (-not $game -and (Get-Date) -lt $deadline)
            if (-not $game) { throw 'Dungeon process did not start' }
            $process = $game
        } else {
            $process = Start-Process -FilePath $program -WorkingDirectory $cwd -WindowStyle Normal -PassThru
        }
        $null = $process.Handle
        $deadline = (Get-Date).AddSeconds(10)
        do {
            Start-Sleep -Milliseconds 100
            $process.Refresh()
        } while (-not $process.HasExited -and $process.MainWindowHandle -eq 0 -and (Get-Date) -lt $deadline)
        if ($process.HasExited -or $process.MainWindowHandle -eq 0) { throw 'No visible test window' }
        $window = $process.MainWindowHandle
        Start-Sleep -Milliseconds 400
        $result.before_input = Save-WindowCapture $window ($key + '-before')
        if ($result.before_input.sampled_colours -lt 16) { throw 'Capture did not establish rendered content' }
        if ($case.example -eq 'pixels') {
            Start-Sleep -Milliseconds 300
            $result.animated = Save-WindowCapture $window ($key + '-animated')
            if ($result.animated.sha256 -eq $result.before_input.sha256) { throw 'Pixels frame did not change' }
        }
        if ($case.example -eq 'dungeon') {
            $game = $process
            if (-not [NerdDesktopProbe]::ConsoleKey($game.Id,0x20,0x20)) { throw 'Could not send Dungeon regeneration input' }
            Start-Sleep -Milliseconds 350
            $result.after_input = Save-WindowCapture $window ($key + '-regenerated')
            if ($result.after_input.sha256 -eq $result.before_input.sha256) { throw 'Dungeon did not change after regeneration input' }
            if (-not [NerdDesktopProbe]::ConsoleKey($game.Id,0x51,0x71)) { throw 'Could not send Dungeon quit input' }
        } else {
            $rect = New-Object NerdDesktopProbe+Rect
            [NerdDesktopProbe]::GetWindowRect($window,[ref]$rect) | Out-Null
            [NerdDesktopProbe]::MoveWindow($window,$rect.Left,$rect.Top,976,759,$true) | Out-Null
            Start-Sleep -Milliseconds 300
            $result.resized = Save-WindowCapture $window ($key + '-resized')
            if ($result.resized.width -eq $result.before_input.width -and $result.resized.height -eq $result.before_input.height) { throw 'Client dimensions did not change on resize' }
            if ($result.resized.sampled_colours -lt 16) { throw 'Resized capture did not establish rendered content' }
            [NerdDesktopProbe]::PostMessage($window,0x100,[IntPtr]0x51,[IntPtr]0x100001) | Out-Null
            [NerdDesktopProbe]::PostMessage($window,0x101,[IntPtr]0x51,[IntPtr]0xC0100001L) | Out-Null
        }
        if (-not $process.WaitForExit(5000)) { throw 'Test did not exit after Q' }
        $result.exit_code = $process.ExitCode
        if ($result.exit_code -ne 0) { throw ('Nonzero exit: ' + $result.exit_code) }
        $result.status = 'CAPTURED; visual review required'
    } catch {
        $result.status = 'BLOCKED OR FAILED'
        $result.error = $_.Exception.Message
    } finally {
        if ($process -and -not $process.HasExited) {
            $process.CloseMainWindow() | Out-Null
            if (-not $process.WaitForExit(3000)) { $process.Kill($true); $process.WaitForExit() }
        }
    }
    $reports[$key] = $result
    $reports | ConvertTo-Json -Depth 8 | Set-Content $reportPath
    Write-Output ($key + ': ' + $result.status + ' ' + $result.error)
}
