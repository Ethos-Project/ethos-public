$ErrorActionPreference = 'Stop'
$sourceRoot = 'C:\Ethos\ethos-logos\RELEASES\FOSS'
$publicRoot = 'C:\Ethos\ethos-public'
function Read-Tree([string]$root) {
    $tree = [Collections.Generic.Dictionary[string,string]]::new([StringComparer]::OrdinalIgnoreCase)
    foreach ($path in [IO.Directory]::EnumerateFiles($root, '*', [IO.SearchOption]::AllDirectories)) {
        $relative = $path.Substring($root.Length + 1)
        if ($relative.StartsWith('.git\', [StringComparison]::OrdinalIgnoreCase)) { continue }
        $before = [IO.FileInfo]::new($path)
        $length = $before.Length
        $modified = $before.LastWriteTimeUtc.Ticks
        $stream = [IO.File]::OpenRead($path)
        try { $hash = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($stream)) }
        finally { $stream.Dispose() }
        $after = [IO.FileInfo]::new($path)
        if ($length -ne $after.Length -or $modified -ne $after.LastWriteTimeUtc.Ticks) {
            throw "File changed during verification: $path"
        }
        $tree.Add($relative, $hash)
    }
    return ,$tree
}
$source = Read-Tree $sourceRoot
$public = Read-Tree $publicRoot
$sourceOnly = @($source.Keys | Where-Object { -not $public.ContainsKey($_) })
$publicOnly = @($public.Keys | Where-Object { -not $source.ContainsKey($_) })
$different = @($source.Keys | Where-Object { $public.ContainsKey($_) -and $source[$_] -ne $public[$_] })
$result = [ordered]@{
    VerifiedAtUtc = [DateTime]::UtcNow.ToString('o')
    SourceFiles = $source.Count
    PublicFiles = $public.Count
    SourceOnly = $sourceOnly
    PublicOnly = $publicOnly
    ContentDifferences = $different
    Exclusion = 'ethos-public\.git\ only'
}
$result | ConvertTo-Json -Depth 4 | Tee-Object -FilePath (Join-Path $PSScriptRoot 'mirror-result.json')
$source | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $PSScriptRoot 'foss-sha256.json')
if ($sourceOnly.Count -or $publicOnly.Count -or $different.Count) { exit 1 }
