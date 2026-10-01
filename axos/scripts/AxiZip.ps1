<#
.SYNOPSIS
    AxiZip - Multi-Agent Context & Backup Packaging Tool for Axi Workspaces.
.DESCRIPTION
    Recursively scans a workspace directory, enforces .axiignore exclusion patterns,
    and packages clean, lightweight .zip archives optimized for LLM multi-agent context sharing
    and offline backups.
.PARAMETER SourceDir
    The root workspace folder to pack. Defaults to current directory.
.PARAMETER OutputFile
    The output .zip file path. Defaults to <FolderName>_context.zip.
.PARAMETER IgnoreFile
    The ignore file name to use. Defaults to '.axiignore'.
.EXAMPLE
    powershell -File C:\Ethos\axos\scripts\AxiZip.ps1 -SourceDir "C:\Ethos\axos" -OutputFile "C:\Ethos\axos_context.zip"
#>

[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string]$SourceDir = (Get-Location).Path,

    [Parameter(Position = 1)]
    [string]$OutputFile,

    [string]$IgnoreFile = ".axiignore",
    [switch]$Quiet
)

$ErrorActionPreference = "Stop"

Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem

$resolvedSource = (Resolve-Path $SourceDir).Path.TrimEnd('\')
if (-not (Test-Path $resolvedSource -PathType Container)) {
    Write-Error "Source directory not found: $resolvedSource"
    exit 1
}

$sourceName = Split-Path $resolvedSource -Leaf
if (-not $OutputFile) {
    $OutputFile = Join-Path (Split-Path $resolvedSource -Parent) "$($sourceName)_context.zip"
}
$resolvedOutput = [System.IO.Path]::GetFullPath($OutputFile)

# Create parent directory of output if needed
$outDir = Split-Path $resolvedOutput -Parent
if ($outDir -and -not (Test-Path $outDir)) {
    New-Item -ItemType Directory -Force -Path $outDir | Out-Null
}

# Remove existing target zip if present
if (Test-Path $resolvedOutput) {
    Remove-Item -Force $resolvedOutput
}

# Load .axiignore patterns
$patterns = [System.Collections.Generic.List[string]]::new()

# Implicit defaults
$defaultIgnores = @('.axi', '.git', '.gemini', '.vscode', '.idea')
foreach ($d in $defaultIgnores) { $patterns.Add($d) }

$ignorePath = Join-Path $resolvedSource $IgnoreFile
if (Test-Path $ignorePath) {
    Get-Content $ignorePath | ForEach-Object {
        $line = $_.Trim()
        if ($line -and -not $line.StartsWith('#')) {
            $patterns.Add($line.Replace('\', '/').TrimStart('/'))
        }
    }
}

function Test-IsIgnored ($relPath, $isDir) {
    $normalized = $relPath.Replace('\', '/')
    if ($isDir -and -not $normalized.EndsWith('/')) {
        $normalized += '/'
    }

    foreach ($pat in $patterns) {
        $cleanPat = $pat.TrimEnd('/')
        $isDirPattern = $pat.EndsWith('/')

        # 1. Exact match or directory prefix match
        if ($isDirPattern) {
            if ($normalized.StartsWith("$cleanPat/") -or $normalized -eq "$cleanPat/") {
                return $true
            }
            # Match any nested directory name (e.g. "node_modules/")
            if ($normalized -like "*/$cleanPat/*" -or $normalized -like "$cleanPat/*") {
                return $true
            }
        } else {
            # 2. Wildcard pattern (e.g. "*.tmp", "*.log")
            if ($pat.Contains('*')) {
                $leaf = Split-Path $normalized.TrimEnd('/') -Leaf
                if ($leaf -like $pat -or $normalized -like $pat) {
                    return $true
                }
            } else {
                # 3. Exact file or relative path match
                if ($normalized -eq $pat -or $normalized.EndsWith("/$pat")) {
                    return $true
                }
            }
        }
    }
    return $false
}

if (-not $Quiet) {
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host "  AxiZip: Packaging Multi-Agent Context Archive" -ForegroundColor Cyan
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host "Source Directory : $resolvedSource"
    Write-Host "Target Archive   : $resolvedOutput"
    Write-Host "Active Patterns  : $($patterns.Count) rules (from $IgnoreFile + defaults)"
    Write-Host "Scanning workspace..." -ForegroundColor Yellow
}

$filesToPack = [System.Collections.Generic.List[PSCustomObject]]::new()
$totalIgnoredFiles = 0
$totalIgnoredDirs = 0
$uncompressedBytes = 0

function Enumerate-Dir ($currentDir, $relBase) {
    $items = Get-ChildItem -LiteralPath $currentDir -Force
    foreach ($item in $items) {
        $relPath = if ($relBase) { "$relBase/$($item.Name)" } else { $item.Name }
        $isDir = $item.PSIsContainer

        if (Test-IsIgnored $relPath $isDir) {
            if ($isDir) {
                $script:totalIgnoredDirs++
            } else {
                $script:totalIgnoredFiles++
            }
            continue
        }

        if ($isDir) {
            Enumerate-Dir $item.FullName $relPath
        } else {
            # Avoid self-zipping if output is inside source directory
            if ($item.FullName -eq $resolvedOutput) { continue }

            $script:uncompressedBytes += $item.Length
            $filesToPack.Add([PSCustomObject]@{
                FullPath = $item.FullName
                RelPath  = $relPath
                Length   = $item.Length
            })
        }
    }
}

Enumerate-Dir $resolvedSource ""

if (-not $Quiet) {
    Write-Host "Found $($filesToPack.Count) files to package ($([math]::Round($uncompressedBytes / 1MB, 2)) MB)." -ForegroundColor Green
    Write-Host "Pruned $totalIgnoredDirs directories and $totalIgnoredFiles files via .axiignore." -ForegroundColor DarkGray
    Write-Host "Compressing into archive..." -ForegroundColor Yellow
}

# Create Zip Archive
$zipArchive = [System.IO.Compression.ZipFile]::Open($resolvedOutput, [System.IO.Compression.ZipArchiveMode]::Create)
try {
    foreach ($file in $filesToPack) {
        $entryName = $file.RelPath.Replace('\', '/')
        [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile(
            $zipArchive,
            $file.FullPath,
            $entryName,
            [System.IO.Compression.CompressionLevel]::Optimal
        ) | Out-Null
    }
} finally {
    $zipArchive.Dispose()
}

$compressedBytes = (Get-Item $resolvedOutput).Length
$ratio = if ($uncompressedBytes -gt 0) { [math]::Round((1 - ($compressedBytes / $uncompressedBytes)) * 100, 1) } else { 0 }

if (-not $Quiet) {
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host "  Packaging Complete!" -ForegroundColor Green
    Write-Host "============================================================" -ForegroundColor Cyan
    Write-Host "Archive Location : $resolvedOutput"
    Write-Host "Files Packaged   : $($filesToPack.Count)"
    Write-Host "Uncompressed Size: $([math]::Round($uncompressedBytes / 1MB, 2)) MB"
    Write-Host "Compressed Size  : $([math]::Round($compressedBytes / 1MB, 2)) MB ($ratio% reduction)"
    Write-Host "Ideal for multi-agent context ingestion and storage backup." -ForegroundColor DarkCyan
}

return $resolvedOutput
