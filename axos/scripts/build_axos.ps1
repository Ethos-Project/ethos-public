# =====================================================================
# Build script for axos.exe (Axos CLI Launcher)
# Resolves compiler via Allforge Toolchain Registry
# =====================================================================

$ErrorActionPreference = "Stop"

$resolver = "C:\Ethos\Allforge\toolchains\allforge-toolchain.ps1"
if (!(Test-Path $resolver)) {
    Write-Error "Allforge toolchain resolver not found: $resolver"
    exit 1
}

$gcc = powershell -ExecutionPolicy Bypass -File $resolver resolve mingw64 -ExeName gcc
if ([string]::IsNullOrWhiteSpace($gcc) -or !(Test-Path $gcc)) {
    Write-Error "Failed to resolve gcc from Allforge Toolchain Registry"
    exit 1
}

$windres = powershell -ExecutionPolicy Bypass -File $resolver resolve mingw64 -ExeName windres
if ([string]::IsNullOrWhiteSpace($windres) -or !(Test-Path $windres)) {
    # Fallback to finding windres alongside gcc
    $gccDir = Split-Path $gcc -Parent
    $windres = Join-Path $gccDir "windres.exe"
}

Write-Host "[BUILD] Resolved GCC: $gcc"
Write-Host "[BUILD] Resolved Windres: $windres"

$srcDir = "C:\Ethos\Axos\axos_lang\src"
$binDir = "C:\Ethos\Axos\bin"
$rcFile = Join-Path $srcDir "axos_version.rc"
$resFile = Join-Path $srcDir "axos_version.res"
$cFile = Join-Path $srcDir "axos_launcher.c"
$outExe = Join-Path $binDir "axos.exe"

# 1. Compile PE Resource Stamp (0.1.0)
Write-Host "[BUILD] Compiling PE version stamp: $resFile ..."
& $windres -i $rcFile -O coff -o $resFile
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to compile version resource with windres"
    exit $LASTEXITCODE
}

# 2. Compile axos.exe
Write-Host "[BUILD] Compiling $outExe ..."
& $gcc -O3 -municode -I"C:\Ethos\Axos\include" $cFile $resFile -o $outExe
if ($LASTEXITCODE -ne 0) {
    Write-Error "Compilation of axos.exe failed."
    exit $LASTEXITCODE
}

# 3. Deploy avm_core.dll into Axos\bin
$avmCoreSrc = "C:\Ethos\Allos\bin\avm_core.dll"
$avmCoreDst = Join-Path $binDir "avm_core.dll"
Copy-Item $avmCoreSrc $avmCoreDst -Force

Write-Host "[BUILD] SUCCESS: Generated $outExe and deployed avm_core.dll"
