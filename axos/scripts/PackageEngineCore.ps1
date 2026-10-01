Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem

$root = 'C:\Ethos'
$outCore = 'C:\Ethos\ethos_engine_core.zip'
if (Test-Path $outCore) { Remove-Item -Force $outCore }

$corePaths = @(
    # The true AVM bytecode runtime & launcher & JIT & Mobile
    'C:\Ethos\allos\avm\kernel',
    'C:\Ethos\allos\avm\daemon',
    'C:\Ethos\allos\avm\build_avm.ps1',
    'C:\Ethos\allos\avm\README.md',
    'C:\Ethos\allos\avm\AVM_PROPRIETARY_SPEC.md',
    'C:\Ethos\allos\avm\forge.allos',
    
    # Allforge Kernel & Stdlib
    'C:\Ethos\allos\allforge\bin',
    'C:\Ethos\allos\allforge\cog_router\kernel',
    'C:\Ethos\allos\allforge\cog_router\forge.allos',
    'C:\Ethos\allos\allforge\cog_router\lea_core.allos',
    'C:\Ethos\allos\allforge\lib\std',
    'C:\Ethos\allos\allforge\lib\cpp',
    'C:\Ethos\allos\allforge\README.md',

    # Real Transpiler & Axos compiler pipeline
    'C:\Ethos\allos\allos-php\tools\proprietary\Transpiler_Engine',
    'C:\Ethos\allos\allos-php\tools\proprietary\allos_c_engine',

    # Axos Language & DVCS core
    'C:\Ethos\axos\axi\src',
    'C:\Ethos\axos\axos_lang',
    'C:\Ethos\axos\interfaces',
    'C:\Ethos\axos\avm\modules\axos_compiler\compiler.axos',
    'C:\Ethos\axos\avm\modules\axos_compiler\dvcs_crypto.axos',
    'C:\Ethos\axos\README.md',

    # Master Ecosystem Maps
    'C:\Ethos\README.md',
    'C:\Ethos\ecosystem_map.txt',
    'C:\Ethos\.axiignore'
)

$coreZip = [System.IO.Compression.ZipFile]::Open($outCore, [System.IO.Compression.ZipArchiveMode]::Create)
$totalFiles = 0
$totalBytes = 0

try {
    foreach ($p in $corePaths) {
        if (Test-Path $p -PathType Leaf) {
            $file = Get-Item $p
            $rel = $p.Substring($root.Length).TrimStart('\', '/').Replace('\', '/')
            [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($coreZip, $p, $rel, [System.IO.Compression.CompressionLevel]::Optimal) | Out-Null
            $totalFiles++
            $totalBytes += $file.Length
        } elseif (Test-Path $p -PathType Container) {
            Get-ChildItem -Path $p -Recurse -File | Where-Object { 
                $_.Extension -notin @('.exe', '.dll', '.pdb', '.o', '.obj', '.a', '.lib', '.zip') -and
                !$_.FullName.Contains('\.git\') -and
                !$_.FullName.Contains('\.axi\') -and
                !$_.FullName.Contains('\.allos\')
            } | ForEach-Object {
                $rel = $_.FullName.Substring($root.Length).TrimStart('\', '/').Replace('\', '/')
                [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($coreZip, $_.FullName, $rel, [System.IO.Compression.CompressionLevel]::Optimal) | Out-Null
                $totalFiles++
                $totalBytes += $_.Length
            }
        }
    }
} finally {
    $coreZip.Dispose()
}

$zipItem = Get-Item $outCore
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  ethos_engine_core.zip Updated Successfully" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "Output File      : $($zipItem.FullName)"
Write-Host "Total Files      : $totalFiles"
Write-Host "Uncompressed Size: $([math]::Round($totalBytes / 1KB, 1)) KB"
Write-Host "Final Zip Size   : $([math]::Round($zipItem.Length / 1KB, 1)) KB"
