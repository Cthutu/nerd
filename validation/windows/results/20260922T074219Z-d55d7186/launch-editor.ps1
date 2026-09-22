# Isolated editor validation; does not install or replace extensions.
$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '../../../..')).Path
. (Join-Path $repoRoot '_tmp/windows-env.ps1')
$editorScratch = Join-Path $PSScriptRoot 'scratch'
$extensionPath = Join-Path $repoRoot 'syntax/nerd-vscode'
$installedExtensions = Join-Path $env:USERPROFILE 'scoop/persist/vscode/data/extensions'
& code --new-window --user-data-dir (Join-Path $editorScratch 'vscode-data') --extensions-dir $installedExtensions --extensionDevelopmentPath $extensionPath (Join-Path $editorScratch 'editor.code-workspace') --goto ((Join-Path $editorScratch 'editor/main.n') + ':8')
