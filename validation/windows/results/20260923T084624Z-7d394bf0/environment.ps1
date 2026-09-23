# Dot-source from the repository. Process-only settings; no global installation.
$validationRepo = (Resolve-Path (Join-Path $PSScriptRoot '../../../..')).Path
$validationLlvm = Join-Path $validationRepo '_tmp/llvm-tools/clang+llvm-22.1.8-x86_64-pc-windows-msvc/bin'
if (-not (Test-Path (Join-Path $validationLlvm 'opt.exe'))) {
    throw 'Extract the official LLVM 22.1.8 archive described in HANDOFF.md first.'
}
$env:PATH = $validationLlvm + ';' + $env:PATH
$env:VCToolsInstallDir = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.44.35207\'
$env:WindowsSdkDir = 'C:\Program Files (x86)\Windows Kits\10\'
$env:WindowsSDKVersion = '10.0.26100.0\'
$env:INCLUDE = @(
    (Join-Path $env:VCToolsInstallDir 'include')
    'C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\ucrt'
    'C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\shared'
    'C:\Program Files (x86)\Windows Kits\10\Include\10.0.26100.0\um'
) -join ';'
$env:LIB = @(
    'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.44.35207\lib\x64'
    'C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64'
    'C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64'
) -join ';'
$env:NERD_LIB_PATH = Join-Path $validationRepo 'mods'
$env:PYTHONUTF8 = '1'
