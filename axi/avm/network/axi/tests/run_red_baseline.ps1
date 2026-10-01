$ErrorActionPreference = 'Stop'

$workspace = (Resolve-Path (Join-Path $PSScriptRoot '..\..\..')).Path
$gcc = Join-Path $workspace 'bootstrap\mingw\bin\mingw64\bin\gcc.exe'
$source = Join-Path $PSScriptRoot 'test_current_wrap_contract.c'
$outputDirectory = Join-Path $workspace 'network\tmp\axi-wrap-sdd'
$testExecutable = Join-Path $outputDirectory 'test_current_wrap_contract.exe'
$axiExecutable = Join-Path $workspace 'network\axi_dvcs\axi.exe'

$testExitCode = 1
try {
    New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

    $compileArguments = @('-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic', $source, '-o', $testExecutable)
    Write-Host "COMPILE: $gcc $($compileArguments -join ' ')"
    & $gcc @compileArguments
    $compileExitCode = $LASTEXITCODE
    Write-Host "COMPILE EXIT: $compileExitCode"
    if ($compileExitCode -eq 0) {
        Write-Host "RUN: $testExecutable $axiExecutable"
        & $testExecutable $axiExecutable
        $testExitCode = $LASTEXITCODE
        Write-Host "RUN EXIT: $testExitCode"
    }
    else {
        $testExitCode = $compileExitCode
    }
}
finally {
    if (Test-Path -LiteralPath $testExecutable) {
        Remove-Item -LiteralPath $testExecutable -Force
    }
}

exit $testExitCode
