# Axos — Natural-Syntax Programming Language
**Root:** `C:\Ethos\Axos\`  
**CLI:** `axos.exe` (Axi Virtual Machine runtime powered by `avm_core.dll`)  
**Version:** 0.1.0  

## Overview
Axos is a natural-syntax, human-readable programming language targeting the Axi Virtual Machine (AVM). It enforces strict semantics where words declare structure and logic, and symbols compute arithmetic expressions.

## Getting Started
To execute Axos programs:

```powershell
# Run a script directly
axos script.axos

# Run a script with arguments
axos run script.axos arg1 arg2

# Open the interactive REPL
axos

# Run with DAP debug substrate
axos debug script.axos
```

## Toolchain & Architecture
- **Language Launcher:** `C:\Ethos\Axos\bin\axos.exe`
- **Virtual Machine Core:** `C:\Ethos\Axos\bin\avm_core.dll`
- **Version Control:** `C:\Ethos\Axi\bin\axi.exe` (Axi DVCS)
