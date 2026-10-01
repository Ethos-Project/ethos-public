# Getting Started with Axos

Axos is executed via the `axos` command-line tool.

## Installation & Setup
Ensure `C:\Ethos\Axos\bin` is present in your `PATH`.
Verify the installation:
```powershell
axos --version
```
Expected output:
```text
axos=0.1.0
kernel=axi
...
```

## Running Your First Script
Create `hello.axos`:
```axos
TO RUN:
    WRITE "Hello from Axos!" TO SCREEN
```

Execute the script:
```powershell
axos hello.axos
```
Output:
```text
Hello from Axos!
```

## Interactive REPL
To start the Axos REPL:
```powershell
axos
```
Exit the REPL by typing `:quit` or pressing `Ctrl+C`.
