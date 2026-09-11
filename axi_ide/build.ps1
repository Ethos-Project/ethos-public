$gpp = "C:\Ethos\ethos-logos\RELEASES\FOSS\axi_compiler\bootstrap\mingw\bin\mingw64\bin\g++.exe"
& $gpp -std=c++17 src\main.cpp -o axi_ide.exe -lcomctl32 -luxtheme -lgdi32 -ldwmapi -lole32 -luuid -mwindows
if ($LASTEXITCODE -eq 0) {
    Write-Host "Build Succeeded!" -ForegroundColor Green
    Copy-Item "C:\Ethos\ethos-public\scintilla\bin\Scintilla.dll" "Scintilla.dll" -Force
} else {
    Write-Host "Build Failed." -ForegroundColor Red
}




