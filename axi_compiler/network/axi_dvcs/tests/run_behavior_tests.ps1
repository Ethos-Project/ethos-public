param(
    [Parameter(Mandatory = $true)]
    [string]$axiExecutable
)

$ErrorActionPreference = 'Stop'
$script:Passed = 0
$script:Failed = 0

function Invoke-axi {
    param([string]$WorkingRoot, [string[]]$Arguments)
    $stdout = Join-Path $WorkingRoot 'stdout.txt'
    $stderr = Join-Path $WorkingRoot 'stderr.txt'
    $process = Start-Process -FilePath $axiExecutable -ArgumentList $Arguments `
        -WorkingDirectory $WorkingRoot -Wait -PassThru -NoNewWindow `
        -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    [pscustomobject]@{
        ExitCode = $process.ExitCode
        Stdout = if (Test-Path -LiteralPath $stdout) { Get-Content -Raw $stdout } else { '' }
        Stderr = if (Test-Path -LiteralPath $stderr) { Get-Content -Raw $stderr } else { '' }
    }
}

function Assert-Case {
    param([string]$Name, [scriptblock]$Body)
    try {
        & $Body
        Write-Host "PASS: $Name"
        $script:Passed++
    }
    catch {
        Write-Host "FAIL: $Name -- $($_.Exception.Message)"
        $script:Failed++
    }
}

function Require([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function New-IsolatedRoot {
    $parent = [System.IO.Path]::GetTempPath()
    $root = Join-Path $parent ("axi-wrap-v1-{0}" -f [guid]::NewGuid().ToString('N'))
    [System.IO.Directory]::CreateDirectory($root) | Out-Null
    return $root
}

function Write-ValidManifest([string]$Root, [string]$Session = 'session-one') {
$manifest = @"
Manifest {
  format: axi-wrap-manifest-v1
  session_id: $Session
  issue_ids: ["issue-2", "issue-1"]
  context_ids: ["context-2", "context-1"]
  paths: ["nested/payload.bin"]
}
"@
    [System.IO.File]::WriteAllText((Join-Path $Root '.axi\wrap-manifest.toon'), $manifest,
        [System.Text.UTF8Encoding]::new($false))
}

function Read-SessionRef([string]$Root, [string]$Session = 'session-one') {
    return [System.IO.File]::ReadAllText(
        (Join-Path $Root ".axi\refs\wip\$Session")).Trim()
}

function Get-ObjectFiles([string]$Root) {
    return @(Get-ChildItem -LiteralPath (Join-Path $Root '.axi\objects') `
        -File -Recurse -ErrorAction Stop)
}

$suiteRoot = New-IsolatedRoot
try {
    Assert-Case 'missing store fails without mutation' {
        $root = Join-Path $suiteRoot 'missing'
        [System.IO.Directory]::CreateDirectory($root) | Out-Null
        $result = Invoke-axi $root @('wrap', '--root', $root)
        Require ($result.ExitCode -eq 3) "expected exit 3, got $($result.ExitCode)"
        Require (-not (Test-Path -LiteralPath (Join-Path $root '.axi'))) 'wrap created .axi'
    }

    Assert-Case 'unversioned store fails closed' {
        $root = Join-Path $suiteRoot 'unversioned'
        [System.IO.Directory]::CreateDirectory((Join-Path $root '.axi')) | Out-Null
        [System.IO.File]::WriteAllText((Join-Path $root '.axi\evidence.txt'), 'preserve')
        $result = Invoke-axi $root @('init', '--root', $root)
        Require ($result.ExitCode -eq 3) "expected exit 3, got $($result.ExitCode)"
        Require ([System.IO.File]::ReadAllText((Join-Path $root '.axi\evidence.txt')) -eq 'preserve') 'existing store mutated'
        Require (-not (Test-Path -LiteralPath (Join-Path $root '.axi\FORMAT'))) 'FORMAT created in unversioned store'
    }

    Assert-Case 'init creates exact v1 layout' {
        $root = Join-Path $suiteRoot 'init'
        [System.IO.Directory]::CreateDirectory($root) | Out-Null
        $result = Invoke-axi $root @('init', '--root', $root)
        Require ($result.ExitCode -eq 0) "expected exit 0, got $($result.ExitCode)"
        Require ([System.IO.File]::ReadAllText((Join-Path $root '.axi\FORMAT')) -eq "axi-dvcs 1`n") 'FORMAT mismatch'
        foreach ($relative in @('objects', 'refs\wip', 'refs\previous', 'tmp')) {
            Require ([System.IO.Directory]::Exists((Join-Path $root ".axi\$relative"))) "missing $relative"
        }
        Require ([System.IO.File]::Exists((Join-Path $root '.axi\wrap-manifest.toon'))) 'missing manifest'
    }

    Assert-Case 'wrap persists ref/object and resume restores exact bytes' {
        $root = Join-Path $suiteRoot 'roundtrip'
        [System.IO.Directory]::CreateDirectory($root) | Out-Null
        $init = Invoke-axi $root @('init', '--root', $root)
        Require ($init.ExitCode -eq 0) "init exit $($init.ExitCode)"
        [System.IO.Directory]::CreateDirectory((Join-Path $root 'nested')) | Out-Null
        $bytes = [byte[]](0, 1, 2, 10, 13, 127, 128, 255)
        [System.IO.File]::WriteAllBytes((Join-Path $root 'nested\payload.bin'), $bytes)
        Write-ValidManifest $root
        $wrap = Invoke-axi $root @('wrap', '--root', $root)
        Require ($wrap.ExitCode -eq 0) "wrap exit $($wrap.ExitCode): $($wrap.Stderr)"
        Require ($wrap.Stdout -match 'LOCAL: durable and verified') 'missing local durability line'
        Require ($wrap.Stdout -match 'SERVER: not attempted; local-only WIP') 'missing server-not-attempted line'
        $refPath = Join-Path $root '.axi\refs\wip\session-one'
        $ref = [System.IO.File]::ReadAllText($refPath)
        Require ($ref -cmatch '^[0-9a-f]{64}\n$') 'malformed ref'
        $id = $ref.Trim()
        Require ([System.IO.File]::Exists((Join-Path $root ".axi\objects\$($id.Substring(0,2))\$($id.Substring(2))"))) 'missing object'
        $destination = Join-Path $suiteRoot 'restore'
        [System.IO.Directory]::CreateDirectory($destination) | Out-Null
        $resume = Invoke-axi $root @('resume', '--root', $root, '--session', 'session-one', '--to', $destination)
        Require ($resume.ExitCode -eq 0) "resume exit $($resume.ExitCode): $($resume.Stderr)"
        $actual = [System.IO.File]::ReadAllBytes((Join-Path $destination 'nested\payload.bin'))
        Require ([System.Linq.Enumerable]::SequenceEqual($bytes, $actual)) 'restored bytes differ'
    }

    Assert-Case 'identical wrap is idempotent without ref or object churn' {
        $root = Join-Path $suiteRoot 'idempotent'
        [System.IO.Directory]::CreateDirectory((Join-Path $root 'nested')) | Out-Null
        Require ((Invoke-axi $root @('init', '--root', $root)).ExitCode -eq 0) 'init failed'
        [System.IO.File]::WriteAllText((Join-Path $root 'nested\payload.bin'), 'same-state')
        Write-ValidManifest $root
        $first = Invoke-axi $root @('wrap', '--root', $root)
        Require ($first.ExitCode -eq 0) "first wrap exit $($first.ExitCode)"
        $firstRef = Read-SessionRef $root
        $firstObjects = Get-ObjectFiles $root
        $second = Invoke-axi $root @('wrap', '--root', $root)
        Require ($second.ExitCode -eq 0) "second wrap exit $($second.ExitCode)"
        Require ($second.Stdout -match 'LOCAL: already durable and verified') 'missing idempotent status'
        Require ((Read-SessionRef $root) -ceq $firstRef) 'current ref changed'
        Require ((Get-ObjectFiles $root).Count -eq $firstObjects.Count) 'object count changed'
        Require (-not (Test-Path -LiteralPath (Join-Path $root '.axi\refs\previous\session-one'))) 'previous ref created'
    }

    Assert-Case 'changed payload advances current and previous refs' {
        $root = Join-Path $suiteRoot 'lineage'
        [System.IO.Directory]::CreateDirectory((Join-Path $root 'nested')) | Out-Null
        Require ((Invoke-axi $root @('init', '--root', $root)).ExitCode -eq 0) 'init failed'
        $payload = Join-Path $root 'nested\payload.bin'
        [System.IO.File]::WriteAllText($payload, 'first-state')
        Write-ValidManifest $root
        Require ((Invoke-axi $root @('wrap', '--root', $root)).ExitCode -eq 0) 'first wrap failed'
        $firstRef = Read-SessionRef $root
        [System.IO.File]::WriteAllText($payload, 'second-state')
        Require ((Invoke-axi $root @('wrap', '--root', $root)).ExitCode -eq 0) 'second wrap failed'
        $secondRef = Read-SessionRef $root
        Require ($secondRef -cne $firstRef) 'current ref did not advance'
        $previous = [System.IO.File]::ReadAllText(
            (Join-Path $root '.axi\refs\previous\session-one')).Trim()
        Require ($previous -ceq $firstRef) 'previous ref does not preserve prior object'
        Require ((Get-ObjectFiles $root).Count -eq 2) 'expected exactly two durable objects'
    }

    Assert-Case 'unsafe manifest paths fail before refs are created' {
        foreach ($unsafe in @('../escape.bin', '.axi/FORMAT', 'nested\payload.bin', 'C:/escape.bin')) {
            $caseName = [BitConverter]::ToString([System.Text.Encoding]::UTF8.GetBytes($unsafe)).Replace('-', '')
            $root = Join-Path $suiteRoot "unsafe-$caseName"
            [System.IO.Directory]::CreateDirectory((Join-Path $root 'nested')) | Out-Null
            Require ((Invoke-axi $root @('init', '--root', $root)).ExitCode -eq 0) 'init failed'
            $escaped = $unsafe.Replace('\', '\\')
            $manifest = "Manifest {`n  format: axi-wrap-manifest-v1`n  session_id: unsafe`n  issue_ids: []`n  context_ids: []`n  paths: [`"$escaped`"]`n}`n"
            [System.IO.File]::WriteAllText((Join-Path $root '.axi\wrap-manifest.toon'), $manifest,
                [System.Text.UTF8Encoding]::new($false))
            $result = Invoke-axi $root @('wrap', '--root', $root)
            Require ($result.ExitCode -eq 4) "path $unsafe expected exit 4, got $($result.ExitCode)"
            Require (-not (Test-Path -LiteralPath (Join-Path $root '.axi\refs\wip\unsafe'))) "path $unsafe created ref"
        }
    }

    Assert-Case 'malformed current ref fails closed without overwrite' {
        $root = Join-Path $suiteRoot 'malformed-ref'
        [System.IO.Directory]::CreateDirectory((Join-Path $root 'nested')) | Out-Null
        Require ((Invoke-axi $root @('init', '--root', $root)).ExitCode -eq 0) 'init failed'
        [System.IO.File]::WriteAllText((Join-Path $root 'nested\payload.bin'), 'payload')
        Write-ValidManifest $root
        Require ((Invoke-axi $root @('wrap', '--root', $root)).ExitCode -eq 0) 'first wrap failed'
        $refPath = Join-Path $root '.axi\refs\wip\session-one'
        [System.IO.File]::WriteAllText($refPath, "not-an-object`n")
        $result = Invoke-axi $root @('wrap', '--root', $root)
        Require ($result.ExitCode -eq 8) "expected exit 8, got $($result.ExitCode)"
        Require ([System.IO.File]::ReadAllText($refPath) -ceq "not-an-object`n") 'malformed ref was overwritten'
    }

    Assert-Case 'corrupt referenced object blocks wrap and resume' {
        $root = Join-Path $suiteRoot 'corrupt-object'
        [System.IO.Directory]::CreateDirectory((Join-Path $root 'nested')) | Out-Null
        Require ((Invoke-axi $root @('init', '--root', $root)).ExitCode -eq 0) 'init failed'
        [System.IO.File]::WriteAllText((Join-Path $root 'nested\payload.bin'), 'payload')
        Write-ValidManifest $root
        Require ((Invoke-axi $root @('wrap', '--root', $root)).ExitCode -eq 0) 'first wrap failed'
        $id = Read-SessionRef $root
        $objectPath = Join-Path $root ".axi\objects\$($id.Substring(0,2))\$($id.Substring(2))"
        $objectBytes = [System.IO.File]::ReadAllBytes($objectPath)
        $objectBytes[$objectBytes.Length - 1] = $objectBytes[$objectBytes.Length - 1] -bxor 0x01
        [System.IO.File]::WriteAllBytes($objectPath, $objectBytes)
        $wrap = Invoke-axi $root @('wrap', '--root', $root)
        Require ($wrap.ExitCode -eq 8) "wrap expected exit 8, got $($wrap.ExitCode)"
        Require ((Read-SessionRef $root) -ceq $id) 'corrupt ref was advanced'
        $destination = Join-Path $suiteRoot 'corrupt-restore'
        [System.IO.Directory]::CreateDirectory($destination) | Out-Null
        $resume = Invoke-axi $root @('resume', '--root', $root, '--session', 'session-one', '--to', $destination)
        Require ($resume.ExitCode -eq 8) "resume expected exit 8, got $($resume.ExitCode)"
        Require ((Get-ChildItem -LiteralPath $destination -Force).Count -eq 0) 'resume wrote from corrupt object'
    }

    Assert-Case 'resume rejects a nonempty destination without mutation' {
        $root = Join-Path $suiteRoot 'nonempty-destination'
        [System.IO.Directory]::CreateDirectory((Join-Path $root 'nested')) | Out-Null
        Require ((Invoke-axi $root @('init', '--root', $root)).ExitCode -eq 0) 'init failed'
        [System.IO.File]::WriteAllText((Join-Path $root 'nested\payload.bin'), 'payload')
        Write-ValidManifest $root
        Require ((Invoke-axi $root @('wrap', '--root', $root)).ExitCode -eq 0) 'wrap failed'
        $destination = Join-Path $suiteRoot 'nonempty-restore'
        [System.IO.Directory]::CreateDirectory($destination) | Out-Null
        $sentinel = Join-Path $destination 'sentinel.txt'
        [System.IO.File]::WriteAllText($sentinel, 'preserve')
        $resume = Invoke-axi $root @('resume', '--root', $root, '--session', 'session-one', '--to', $destination)
        Require ($resume.ExitCode -eq 9) "expected exit 9, got $($resume.ExitCode)"
        Require ([System.IO.File]::ReadAllText($sentinel) -ceq 'preserve') 'sentinel changed'
        Require (-not (Test-Path -LiteralPath (Join-Path $destination 'nested\payload.bin'))) 'payload was written'
    }

    Assert-Case 'nested discovery finds store and ignores incomplete temp files' {
        $root = Join-Path $suiteRoot 'discovery'
        $nested = Join-Path $root 'nested\deeper'
        [System.IO.Directory]::CreateDirectory($nested) | Out-Null
        Require ((Invoke-axi $root @('init', '--root', $root)).ExitCode -eq 0) 'init failed'
        [System.IO.File]::WriteAllText((Join-Path $root 'nested\payload.bin'), 'payload')
        Write-ValidManifest $root
        $orphan = Join-Path $root '.axi\tmp\interrupted-object.tmp'
        [System.IO.File]::WriteAllText($orphan, 'incomplete')
        $wrap = Invoke-axi $nested @('wrap')
        Require ($wrap.ExitCode -eq 0) "discovered wrap exit $($wrap.ExitCode): $($wrap.Stderr)"
        Require ((Read-SessionRef $root).Length -eq 64) 'store ref was not written'
        Require ([System.IO.File]::ReadAllText($orphan) -ceq 'incomplete') 'orphan temp was mutated'
    }
}
finally {
    $expectedPrefix = [System.IO.Path]::GetFullPath([System.IO.Path]::GetTempPath())
    $resolved = [System.IO.Path]::GetFullPath($suiteRoot)
    if (-not $resolved.StartsWith($expectedPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "refusing cleanup outside temp: $resolved"
    }
    if (Test-Path -LiteralPath $suiteRoot) {
        Remove-Item -LiteralPath $suiteRoot -Recurse -Force
    }
}

Write-Host "TEST SUMMARY: $script:Passed passed; $script:Failed failed"
if ($script:Failed -ne 0) { exit 1 }
exit 0
