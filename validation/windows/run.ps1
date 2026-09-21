# Forward arguments to the standard-library-only Python runner.
$ErrorActionPreference = 'Stop'
$runner = Join-Path $PSScriptRoot 'run.py'
if (Get-Command py -ErrorAction SilentlyContinue) {
    & py -3 $runner @args
} elseif (Get-Command python -ErrorAction SilentlyContinue) {
    & python $runner @args
} else {
    throw 'Python 3.10+ is required. Install it or run this script from your Python development environment.'
}
exit $LASTEXITCODE
