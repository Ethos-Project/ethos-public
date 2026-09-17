with open('compiler.axi', 'r', encoding='utf-8') as f:
    text = f.read()

# Fix the include regex so we can pass quotes!
# We want @C_Include("%127[^)])"
text = text.replace('sscanf(p, "@C_Include(\\"%127[^\\"]\\")", inc);', 'sscanf(p, "@C_Include(%127[^)])", inc);')

with open('compiler.axi', 'w', encoding='utf-8') as f:
    f.write(text)
