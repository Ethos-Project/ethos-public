$ErrorActionPreference = "Stop"
$toolsDir = "C:\Ethos\tools"
$mingwDir = "C:\Ethos\ethos_public_git\axi_compiler\bootstrap\mingw\bin\mingw64\bin"
$env:Path = "$toolsDir\node-v20.11.1-win-x64;$env:CARGO_HOME\bin;$mingwDir;" + $env:Path
$env:CARGO_HOME = "$toolsDir\.cargo"
$env:RUSTUP_HOME = "$toolsDir\.rustup"
$env:Path = "$env:CARGO_HOME\bin;" + $env:Path

Write-Host "Re-building Tauri App with MinGW..."
Set-Location "C:\Ethos\ethos_public_git\apps\axi_ide"
npm run tauri build

