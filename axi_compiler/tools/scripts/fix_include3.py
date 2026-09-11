with open('domus_genesis.axi', 'r', encoding='utf-8') as f:
    text = f.read()
text = text.replace('\"\\"llama_shim.h\\""', '<llama_shim.h>')
with open('domus_genesis.axi', 'w', encoding='utf-8') as f:
    f.write(text)
