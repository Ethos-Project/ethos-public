$ErrorActionPreference = "Stop"
$toolsDir = "C:\Ethos\tools"
$env:Path = "$toolsDir\node-v20.11.1-win-x64;" + $env:Path
$env:CARGO_HOME = "$toolsDir\.cargo"
$env:RUSTUP_HOME = "$toolsDir\.rustup"
$env:Path = "$env:CARGO_HOME\bin;" + $env:Path

Write-Host "Cargo version:"
cargo --version

Write-Host "Re-building Tauri App..."
Set-Location "C:\Ethos\ethos-public\apps\axi_ide"
npm run tauri build
