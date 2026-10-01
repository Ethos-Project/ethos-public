@echo off
echo Compiling AXIOM CLI into standalone native executable...
python -m nuitka --standalone --onefile --output-filename=axiom.exe axiom.py
if %ERRORLEVEL% equ 0 (
    echo Compilation successful! You can now run axiom.exe natively.
) else (
    echo Compilation failed.
)
