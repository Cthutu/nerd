param(
    [Parameter(Mandatory = $true)][string]$Example,
    [switch]$BuildOnly
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path $PSScriptRoot -Parent

# A VS Code task need not inherit a Visual Studio developer shell. Import the
# native headers/libraries without replacing the user's LLVM tools on PATH.
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if (-not (Test-Path -LiteralPath $vswhere)) {
    throw 'Install Visual Studio C++ build tools and a Windows SDK.'
}
$vsRoot = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw 'Visual Studio C++ build tools were not found.' }
$devCmd = Join-Path $vsRoot 'Common7/Tools/VsDevCmd.bat'
$previousLib = $env:LIB
$devEnvironment = & $env:ComSpec /d /c "call `"$devCmd`" -no_logo -arch=x64 -host_arch=x64 >nul && set"
if ($LASTEXITCODE -ne 0) { throw 'Failed to initialize the Windows SDK/CRT environment.' }
foreach ($line in $devEnvironment) {
    if ($line -match '^(INCLUDE|LIB|LIBPATH|VCToolsInstallDir|WindowsSdkDir|WindowsSDKVersion)=(.*)$') {
        [Environment]::SetEnvironmentVariable($Matches[1], $Matches[2], 'Process')
    }
}
if ($previousLib) { $env:LIB += ';' + $previousLib }
if ($env:VULKAN_SDK) {
    $vulkanLib = Join-Path $env:VULKAN_SDK 'Lib'
    if (-not (Test-Path -LiteralPath (Join-Path $vulkanLib 'vulkan-1.lib'))) {
        throw 'VULKAN_SDK must point to a Windows Vulkan SDK containing Lib/vulkan-1.lib.'
    }
    $env:LIB += ';' + $vulkanLib
    # Scoop/extracted SDKs may not register the validation layer system-wide.
    $vulkanBin = Join-Path $env:VULKAN_SDK 'Bin'
    if (Test-Path -LiteralPath (Join-Path $vulkanBin 'VkLayer_khronos_validation.json')) {
        $env:VK_ADD_LAYER_PATH = (@($env:VK_ADD_LAYER_PATH, $vulkanBin) | Where-Object { $_ }) -join ';'
    }
} elseif ($Example -eq 'vktriangle') {
    throw 'Set VULKAN_SDK to the installed Windows Vulkan SDK, then restart VS Code.'
}
$env:CC = 'clang'
$recipe = if ($BuildOnly) { 'build-example' } else { 'run-example' }
Push-Location $repoRoot
try {
    & just $recipe $Example
    if ($LASTEXITCODE -ne 0) { throw "just $recipe $Example failed (exit $LASTEXITCODE)." }
} finally {
    Pop-Location
}
