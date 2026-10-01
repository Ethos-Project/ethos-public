<#
.SYNOPSIS
    PackageWebAgentContext.ps1 - Generates a curated, clean context zip (< 25 MB) for AI Web Agents.
.DESCRIPTION
    Packages the actual source code, specifications, documentation, and configurations of axos and allos.
    Excludes logos, build folders, binaries, node_modules, and historical DVCS blob stores.
#>

param(
    [string]$OutputFile = "C:\Ethos\ethos_webagent_context.zip"
)

$ErrorActionPreference = "Stop"

Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem

if (Test-Path $OutputFile) {
    Remove-Item -Force $OutputFile
}

$root = "C:\Ethos"

# Curated core source directories in allos and axos
$targetDirs = @(
    "C:\Ethos\allos\avm\kernel",
    "C:\Ethos\allos\avm\daemon",
    "C:\Ethos\allos\allforge\bin",
    "C:\Ethos\allos\allforge\cog_router",
    "C:\Ethos\allos\allforge\lib\std",
    "C:\Ethos\allos\allforge\lib\cpp",
    "C:\Ethos\allos\allos-ui\AllosWinUI",
    "C:\Ethos\allos\allos-ui\SpatialStudio.RenderKit",
    "C:\Ethos\allos\allos-ui\Spatial_Studio",
    "C:\Ethos\allos\allos-ui\NatLP",
    "C:\Ethos\allos\allos-php\ipc",
    "C:\Ethos\allos\websites",
    "C:\Ethos\axos\axi",
    "C:\Ethos\axos\axos_ide",
    "C:\Ethos\axos\axos_lang",
    "C:\Ethos\axos\components",
    "C:\Ethos\axos\examples",
    "C:\Ethos\axos\interfaces",
    "C:\Ethos\axos\packaging",
    "C:\Ethos\axos\scripts",
    "C:\Ethos\axos\avm\console",
    "C:\Ethos\axos\avm\cli",
    "C:\Ethos\axos\avm\modules",
    "C:\Ethos\axos\avm\lib\core",
    "C:\Ethos\axos\avm\docs"
)

# Root documentation and specs
$rootFiles = @(
    "C:\Ethos\README.md",
    "C:\Ethos\.axiignore",
    "C:\Ethos\ecosystem_map.txt",
    "C:\Ethos\allos\README.md",
    "C:\Ethos\allos\avm\README.md",
    "C:\Ethos\allos\avm\AVM_PROPRIETARY_SPEC.md",
    "C:\Ethos\allos\avm\forge.allos",
    "C:\Ethos\allos\allforge\README.md",
    "C:\Ethos\allos\allos-ui\README.md",
    "C:\Ethos\allos\allos-ui\package.json",
    "C:\Ethos\allos\allos-ui\server.js",
    "C:\Ethos\allos\allos-php\README.md",
    "C:\Ethos\allos\websites\README.md",
    "C:\Ethos\axos\README.md",
    "C:\Ethos\axos\bin\README.md"
)

# Skip patterns to guarantee zero binaries and build outputs
$skipFolderPatterns = @("\node_modules\", "\build\", "\dist\", "\bin\", "\obj\", "\publish\", "\CMakeFiles\", "\.git\", "\.axi\", "\.allos\", "\.gemini\", "\.vscode\")
$validExtensions = @(".allos", ".axi", ".axos", ".cpp", ".c", ".h", ".hpp", ".cs", ".csproj", ".php", ".js", ".ts", ".html", ".css", ".json", ".yaml", ".yml", ".md", ".iss", ".ps1", ".bat", ".txt", ".def", ".eros")

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  Generating Curated Web-Agent Context Archive (< 25 MB)" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor Cyan

$filesToPack = [System.Collections.Generic.List[PSCustomObject]]::new()
$uncompressedBytes = 0

# 1. Collect target directories
foreach ($dir in $targetDirs) {
    if (Test-Path $dir) {
        $items = Get-ChildItem -Path $dir -Recurse -File -Force -ErrorAction SilentlyContinue
        foreach ($file in $items) {
            $skip = $false
            foreach ($pat in $skipFolderPatterns) {
                if ($file.FullName.Contains($pat)) { $skip = $true; break }
            }
            if ($skip) { continue }

            $ext = $file.Extension.ToLowerInvariant()
            if ($validExtensions -contains $ext) {
                $rel = $file.FullName.Substring($root.Length).TrimStart('\', '/')
                $script:uncompressedBytes += $file.Length
                $filesToPack.Add([PSCustomObject]@{
                    FullPath = $file.FullName
                    RelPath  = $rel.Replace('\', '/')
                })
            }
        }
    }
}

# 2. Collect root docs & specs
foreach ($rf in $rootFiles) {
    if (Test-Path $rf) {
        $item = Get-Item $rf
        $rel = $item.FullName.Substring($root.Length).TrimStart('\', '/')
        $script:uncompressedBytes += $item.Length
        $filesToPack.Add([PSCustomObject]@{
            FullPath = $item.FullName
            RelPath  = $rel.Replace('\', '/')
        })
    }
}

Write-Host "Selected $($filesToPack.Count) curated source & documentation files." -ForegroundColor Green
Write-Host "Uncompressed Size: $([math]::Round($uncompressedBytes / 1MB, 2)) MB" -ForegroundColor DarkCyan
Write-Host "Compressing into $OutputFile ..." -ForegroundColor Yellow

$zipArchive = [System.IO.Compression.ZipFile]::Open($OutputFile, [System.IO.Compression.ZipArchiveMode]::Create)
try {
    foreach ($f in $filesToPack) {
        [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
            $zipArchive,
            $f.FullPath,
            $f.RelPath,
            [System.IO.Compression.CompressionLevel]::Optimal
        ) | Out-Null
    }
} finally {
    $zipArchive.Dispose()
}

$zipSize = (Get-Item $OutputFile).Length
$zipSizeMB = [math]::Round($zipSize / 1MB, 2)
$ratio = [math]::Round((1 - ($zipSize / $uncompressedBytes)) * 100, 1)

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "  Success! Package Created Successfully" -ForegroundColor Green
Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "Output File      : $OutputFile"
Write-Host "Total Files      : $($filesToPack.Count)"
Write-Host "Uncompressed Size: $([math]::Round($uncompressedBytes / 1MB, 2)) MB"
Write-Host "Final Zip Size   : $zipSizeMB MB ($ratio% compression)"

if ($zipSizeMB -le 25.0) {
    Write-Host "STATUS: READY FOR WEB AGENT (Within 25 MB limit!)" -ForegroundColor Green
} else {
    Write-Host "STATUS: WARNING - Exceeds 25 MB" -ForegroundColor Red
}

return $OutputFile
