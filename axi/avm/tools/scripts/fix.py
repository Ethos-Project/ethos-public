import sys
import re

path = r'C:\Ethos\ethos-logos\FOSS\axi_compiler\README.md'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

quickstart = '''
## 🚀 Quickstart

1. Download the latest `axi_compiler_v3.1_public.exe` from the **Releases** tab.
2. Write your first script (`hello.axi`):
```axi
HOW TO start():
    WRITE "Hello from Axiom!" TO SCREEN

start -> start
```
3. Compile and run natively:
```bash
axi compile hello.axi -o hello.exe
./hello.exe
```
'''

content = content.replace('---', quickstart + '\n---\n', 1)

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
