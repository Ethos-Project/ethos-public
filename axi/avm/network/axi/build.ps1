param(
    [string]$OutputPath
)

$ErrorActionPreference = 'Stop'
$workspace = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$gcc = Join-Path $workspace 'bootstrap\mingw\bin\mingw64\bin\gcc.exe'
if (-not $OutputPath) {
    $OutputPath = Join-Path $workspace 'network\tmp\axi-wrap-sdd\axi-rc.exe'
}
$outputDirectory = Split-Path -Parent $OutputPath
[System.IO.Directory]::CreateDirectory($outputDirectory) | Out-Null

$sources = @(
    (Join-Path $PSScriptRoot 'axi.c'),
    (Join-Path $PSScriptRoot 'axi_store.c'),
    (Join-Path $PSScriptRoot 'axi_manifest.c'),
    (Join-Path $PSScriptRoot 'axi_object.c'),
    (Join-Path $PSScriptRoot 'axi_wrap.c'),
    (Join-Path $PSScriptRoot 'axi_resume.c'),
    (Join-Path $PSScriptRoot 'axi_runner.c')
)
$arguments = @('-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic', '-municode') +
    $sources + @('-o', $OutputPath, '-lbcrypt')

Write-Host "COMPILE: $gcc $($arguments -join ' ')"
& $gcc @arguments
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "BUILT: $OutputPath"
exit 0
