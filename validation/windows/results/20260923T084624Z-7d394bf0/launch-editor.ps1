# Isolated editor validation; leaves installed compiler/extensions unchanged.
$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '../../../..')).Path
. (Join-Path $PSScriptRoot 'environment.ps1')
$editorScratch = Join-Path $PSScriptRoot 'scratch'
$editorFolder = Join-Path $editorScratch 'editor'
New-Item -ItemType Directory -Force $editorFolder | Out-Null
@'
mix :: fn (left: i32, right: i32) -> i32 {
    total := left + right
    return total
}

main :: fn () {
    first: i32
    first = 10
    second: i32
    second = first + 1
    third: i32
    third = mix(second, 3)
    on third == 14 => prn("ok")
}
'@ | Set-Content (Join-Path $editorFolder 'main.n')
$workspace = @{
    folders = @(@{ path = $editorFolder })
    settings = @{ 'nerd.languageServer.path' = (Join-Path $repoRoot '_bin/nerd-debug.exe') }
    launch = @{
        version = '0.2.0'
        configurations = @(@{
            name = 'Validate Nerd Windows debugger'
            type = 'nerd'
            request = 'launch'
            program = '${command:nerd.buildActiveFileForDebug}'
            cwd = '${workspaceFolder}'
            args = @()
        })
    }
}
$workspaceFile = Join-Path $editorScratch 'editor.code-workspace'
$workspace | ConvertTo-Json -Depth 8 | Set-Content $workspaceFile
$extensionPath = Join-Path $repoRoot 'syntax/nerd-vscode'
$installedExtensions = Join-Path $env:USERPROFILE 'scoop/persist/vscode/data/extensions'
& code --new-window --user-data-dir (Join-Path $editorScratch 'vscode-data') --extensions-dir $installedExtensions --extensionDevelopmentPath $extensionPath $workspaceFile --goto ((Join-Path $editorFolder 'main.n') + ':8')
