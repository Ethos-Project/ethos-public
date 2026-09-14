$ErrorActionPreference = "Stop"
$toolsDir = "C:\Ethos\tools"
New-Item -ItemType Directory -Force -Path $toolsDir | Out-Null

Write-Host "Downloading Node.js..."
$nodeUrl = "https://nodejs.org/dist/v20.11.1/node-v20.11.1-win-x64.zip"
$nodeZip = "$toolsDir\node.zip"
if (!(Test-Path "$toolsDir\node-v20.11.1-win-x64\node.exe")) {
    Invoke-WebRequest -Uri $nodeUrl -OutFile $nodeZip
    Write-Host "Extracting Node.js..."
    Expand-Archive -Path $nodeZip -DestinationPath $toolsDir -Force
}
$env:Path = "$toolsDir\node-v20.11.1-win-x64;" + $env:Path

Write-Host "Node version:"
node --version
Write-Host "NPM version:"
npm --version

Write-Host "Downloading Rustup..."
$env:RUSTUP_HOME = "$toolsDir\.rustup"
$env:CARGO_HOME = "$toolsDir\.cargo"
$rustupExe = "$toolsDir\rustup-init.exe"
if (!(Test-Path "$env:CARGO_HOME\bin\cargo.exe")) {
    Invoke-WebRequest -Uri "https://win.rustup.rs/x86_64" -OutFile $rustupExe
    Write-Host "Installing Rust (GNU toolchain)..."
    & $rustupExe -y --default-host x86_64-pc-windows-gnu --no-modify-path
}
$env:Path = "$env:CARGO_HOME\bin;" + $env:Path

Write-Host "Cargo version:"
cargo --version

Write-Host "Building Tauri App..."
Set-Location "C:\Ethos\ethos-public\apps\axi_ide"
npm install
npm run tauri build
