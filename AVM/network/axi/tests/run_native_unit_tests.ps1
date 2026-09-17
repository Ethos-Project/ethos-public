param(
    [Parameter(Mandatory = $true)]
    [string]$axiExecutable
)

$ErrorActionPreference = 'Stop'
$dvcs = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$workspace = (Resolve-Path (Join-Path $dvcs '..\..')).Path
$gcc = Join-Path $workspace 'bootstrap\mingw\bin\mingw64\bin\gcc.exe'
$output = Join-Path $workspace 'network\tmp\axi-wrap-sdd\test_object_identity.exe'
$suiteRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("axi-unit-{0}" -f [guid]::NewGuid().ToString('N'))

try {
    [System.IO.Directory]::CreateDirectory((Split-Path -Parent $output)) | Out-Null
    $arguments = @(
        '-std=c11', '-Wall', '-Wextra', '-Werror', '-pedantic', '-municode',
        (Join-Path $PSScriptRoot 'test_object_identity.c'),
        (Join-Path $dvcs 'axi_store.c'),
        (Join-Path $dvcs 'axi_manifest.c'),
        (Join-Path $dvcs 'axi_object.c'),
        '-o', $output, '-lbcrypt'
    )
    & $gcc @arguments
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

    [System.IO.Directory]::CreateDirectory($suiteRoot) | Out-Null
    & $axiExecutable init --root $suiteRoot | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "fixture init failed with exit $LASTEXITCODE" }
    [System.IO.File]::WriteAllBytes((Join-Path $suiteRoot 'payload.bin'), [byte[]](1, 2, 3, 4))
    $manifest = @"
Manifest {
  format: axi-wrap-manifest-v1
  session_id: identity-test
  issue_ids: []
  context_ids: []
  paths: ["payload.bin"]
}
"@
    [System.IO.File]::WriteAllText((Join-Path $suiteRoot '.axi\wrap-manifest.toon'),
        $manifest, [System.Text.UTF8Encoding]::new($false))
    & $axiExecutable wrap --root $suiteRoot | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "fixture wrap failed with exit $LASTEXITCODE" }

    & $output $suiteRoot identity-test
    exit $LASTEXITCODE
}
finally {
    if (Test-Path -LiteralPath $output) { Remove-Item -LiteralPath $output -Force }
    $tempRoot = [System.IO.Path]::GetFullPath([System.IO.Path]::GetTempPath())
    $resolved = [System.IO.Path]::GetFullPath($suiteRoot)
    if ($resolved.StartsWith($tempRoot, [System.StringComparison]::OrdinalIgnoreCase) -and
        (Test-Path -LiteralPath $suiteRoot)) {
        Remove-Item -LiteralPath $suiteRoot -Recurse -Force
    }
}
