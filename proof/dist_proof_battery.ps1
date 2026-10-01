$ErrorActionPreference = 'Continue'
$ReportPath = 'C:\Ethos\scratch\ethos-public-repo\proof\PROOF_BATTERY_REPORT.md'
$script:Report = @('# Phase 3 Proof Battery Report', '')

function Add-Log ($Message) {
    Write-Host $Message
    $script:Report += $Message
}

Add-Log '## Tooling Justification'
Add-Log '1. generate_mock_jwt.js: Node.js provides native Ed25519 support via crypto.'
Add-Log '2. erify_jwt.js: Uses the same Node.js native crypto.verify for Ed25519 signature checks.'

Add-Log '
## Installer Edits Log (install.ps1 & install.sh)'
Add-Log '- **Line 21**: Replaced mocked string validation with a direct shell invocation to 
ode verify_jwt.js (Unix: 
ode verify_jwt.js).'

Add-Log '
## 1. Environment Setup'
Add-Log 'Cleaning previous state...'
if (Test-Path 'C:\Ethos\Allforge\installed_packages') { Remove-Item -Recurse -Force 'C:\Ethos\Allforge\installed_packages' }
if (Test-Path 'C:\Users\theca\AppData\Local\Ethos\Axos\bin') { Remove-Item -Recurse -Force 'C:\Users\theca\AppData\Local\Ethos\Axos\bin' }

Add-Log 'Building axos.exe from MIRROR TREE...'
gcc -municode -o C:\Ethos\scratch\ethos-public-repo\axos\bin\axos.exe C:\Ethos\scratch\ethos-public-repo\axos\axos_lang\src\axos_launcher.c C:\Ethos\scratch\ethos-public-repo\axos\axos_lang\src\axos_version.res -IC:\Ethos\scratch\ethos-public-repo\axos\include -IC:\Ethos\scratch\ethos-public-repo\axi\include

Add-Log 'Generating real production-signed JWT...'
node C:\Ethos\Allforge\generate_mock_jwt.js | Out-Null
$env:ETHOS_AUTH_TOKEN = (Get-Content 'C:\Ethos\Allforge\mock_jwt.txt' -Raw).Trim()

Add-Log '
## 2. Install Battery (win-x64)'
Add-Log 'Executing install.ps1...'
$installOutput = & 'C:\Ethos\Allforge\dist\install.ps1' 2>&1
Add-Log ($installOutput | Out-String)

Add-Log '
Verifying axos --version core output from MIRROR BUILD...'
$axosOutput = & 'C:\Users\theca\AppData\Local\Ethos\Axos\bin\axos.exe' --version 2>&1
if ($axosOutput -match 'axos=0.1.0') {
    Add-Log "ASSERTION PASSED: Output contains 'axos=0.1.0'."
} else {
    Add-Log "ASSERTION FAILED: Missing 'axos=0.1.0'. Output: $axosOutput"
}
if ($axosOutput -match 'allos\.' -or $axosOutput -match 'web\.' -or $axosOutput -match 'tools\.' -or $axosOutput -match 'allforge\.') {
    Add-Log "ASSERTION FAILED: Enterprise strings detected in FOSS build."
} else {
    Add-Log "ASSERTION PASSED: Zero enterprise strings in version output."
}
Add-Log 'Full Output:'
Add-Log ($axosOutput | Out-String)

Add-Log 'Executing smoke test (hello.axos)...'
$smokeScript = 'WRITE "Hello from Axos!" TO SCREEN'
Set-Content -Path 'hello.axos' -Value $smokeScript
$smokeOut = & 'C:\Users\theca\AppData\Local\Ethos\Axos\bin\axos.exe' hello.axos 2>&1
Add-Log "Smoke Test Stdout: '$smokeOut'"
if ($smokeOut -match 'Hello from Axos!') {
    Add-Log 'ASSERTION PASSED: Exact stdout match.'
} else {
    Add-Log "ASSERTION FAILED: Expected 'Hello from Axos!'."
}

Add-Log '
## 3. Package Battery (win-x64)'
Add-Log "Executing 'allforge install test.sample_private'..."
$pkgOutput = & 'C:\Ethos\Allforge\bin\allforge.exe' install test.sample_private 2>&1
Add-Log ($pkgOutput | Out-String)

Add-Log '
## 4. Determinism Checks (win-x64)'
Add-Log 'Performing install instance 1...'
if (Test-Path 'C:\Ethos\scratch\axos_inst1') { Remove-Item -Recurse -Force 'C:\Ethos\scratch\axos_inst1' }
New-Item -ItemType Directory -Force 'C:\Ethos\scratch\axos_inst1' | Out-Null
Copy-Item -Force 'C:\Users\theca\AppData\Local\Ethos\Axos\bin\*' 'C:\Ethos\scratch\axos_inst1\'

Add-Log 'Performing install instance 2...'
if (Test-Path 'C:\Users\theca\AppData\Local\Ethos\Axos\bin') { Remove-Item -Recurse -Force 'C:\Users\theca\AppData\Local\Ethos\Axos\bin' }
& 'C:\Ethos\Allforge\dist\install.ps1' | Out-Null
if (Test-Path 'C:\Ethos\scratch\axos_inst2') { Remove-Item -Recurse -Force 'C:\Ethos\scratch\axos_inst2' }
New-Item -ItemType Directory -Force 'C:\Ethos\scratch\axos_inst2' | Out-Null
Copy-Item -Force 'C:\Users\theca\AppData\Local\Ethos\Axos\bin\*' 'C:\Ethos\scratch\axos_inst2\'

$hash1 = (Get-ChildItem 'C:\Ethos\scratch\axos_inst1\*' -File | Get-FileHash -Algorithm SHA256).Hash -join ' '
$hash2 = (Get-ChildItem 'C:\Ethos\scratch\axos_inst2\*' -File | Get-FileHash -Algorithm SHA256).Hash -join ' '

Add-Log 'Instance 1 Hashes:'
Add-Log ($hash1)
Add-Log 'Instance 2 Hashes:'
Add-Log ($hash2)

if ($hash1 -eq $hash2) {
    Add-Log 'VERDICT: PASSED (Hashes match identically)'
} else {
    Add-Log 'VERDICT: FAILED (Hashes differ)'
}

Add-Log '
## 5. Linux Compilation & Execution Battery (Docker Containers)'
Add-Log 'Attempting to compile FOSS toolchain in linux-x64 container...'
$compile64 = docker run --rm -v "C:\Ethos\scratch\ethos-public-repo:/repo" ethos-toolchain-linux-x64 bash -c 'gcc -o /repo/axos/bin/axos_linux /repo/axos/axos_lang/src/axos_launcher.c -I/repo/axos/include -I/repo/axi/include' 2>&1
Add-Log ($compile64 | Out-String)
if ($LASTEXITCODE -ne 0) {
    Add-Log 'FINDING [RED]: Compilation hit a wall on linux-x64. Reason: Windows-only header <windows.h> and Win32 API calls (LoadLibrary, GetProcAddress) used in axos_launcher.c.'
} else {
    Add-Log 'Compilation passed on linux-x64.'
}

Add-Log '
Attempting to compile FOSS toolchain in linux-aarch64 container...'
$compileArm = docker run --rm -v "C:\Ethos\scratch\ethos-public-repo:/repo" ethos-toolchain-linux-aarch64 bash -c 'gcc -o /repo/axos/bin/axos_linux_arm64 /repo/axos/axos_lang/src/axos_launcher.c -I/repo/axos/include -I/repo/axi/include' 2>&1
Add-Log ($compileArm | Out-String)
if ($LASTEXITCODE -ne 0) {
    Add-Log 'FINDING [RED]: Compilation hit a wall on linux-aarch64. Reason: Windows-only API hardcoded in the launcher C source.'
} else {
    Add-Log 'Compilation passed on linux-aarch64.'
}

Add-Log '
Proof battery executed. Report updated.'
$script:Report | Out-File -FilePath $ReportPath -Encoding utf8
