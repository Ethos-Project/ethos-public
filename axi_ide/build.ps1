param(
    [string]$OutputPath = (Join-Path $PSScriptRoot 'axi_ide.exe')
)

$ErrorActionPreference = 'Stop'
$gpp = Join-Path $PSScriptRoot '..\axi_compiler\bootstrap\mingw\bin\mingw64\bin\g++.exe'
$scintilla = Join-Path $PSScriptRoot 'Scintilla.dll'
if (-not (Test-Path -LiteralPath $gpp -PathType Leaf)) { throw "Compiler missing: $gpp" }
if (-not (Test-Path -LiteralPath $scintilla -PathType Leaf)) { throw "Scintilla runtime missing: $scintilla" }

$OutputPath = [IO.Path]::GetFullPath($OutputPath)
& $gpp -std=c++17 (Join-Path $PSScriptRoot 'src\main.cpp') -o $OutputPath -lcomctl32 -luxtheme -lgdi32 -ldwmapi -lole32 -luuid -mwindows
if ($LASTEXITCODE -ne 0) { throw "IDE build failed with exit code $LASTEXITCODE" }

$runtimeOutput = Join-Path (Split-Path $OutputPath -Parent) 'Scintilla.dll'
if ([IO.Path]::GetFullPath($scintilla) -ne $runtimeOutput) {
    Copy-Item -LiteralPath $scintilla -Destination $runtimeOutput -Force
}
Write-Host "Build Succeeded: $OutputPath" -ForegroundColor Green




