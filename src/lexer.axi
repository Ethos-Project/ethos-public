@C_Include("<stdio.h>")
@C_Include("<string.h>")
@C_Include("<stdbool.h>")

# ---------------------------------------------------------
# 1. THE MEMORY ARENAS (Static Pointers)
# ---------------------------------------------------------
@StaticArena
state source_text: string = ""

@StaticArena
state current_char: string = ""

@StaticArena
state token_type_out: float = 0.0

# ---------------------------------------------------------
# 2. THE CHIP (The Scanner Node)
# ---------------------------------------------------------
node scan_identifier() -> float:
    @C_Native
    """
    // We can run raw C inside the node to leverage strcmp and pointer scanning!
    if (strcmp(source_text, "state") == 0) {
        printf("[LEXER CHIP] Scanned Keyword: state\n");
        token_type_out = 100.0;
    } else if (strcmp(source_text, "node") == 0) {
        printf("[LEXER CHIP] Scanned Keyword: node\n");
        token_type_out = 200.0;
    } else {
        printf("[LEXER CHIP] Scanned Identifier: %s\n", source_text);
        token_type_out = 300.0;
    }
    """

# ---------------------------------------------------------
# 3. THE WIRES (Execution Loop)
# ---------------------------------------------------------
state active: bool = True

while active:
    print("--- Booting Axiom String Scanner Circuit ---")
    
    source_text = "state"
    source_text -> scan_identifier
    print(token_type_out)
    
    source_text = "node"
    source_text -> scan_identifier
    print(token_type_out)
    
    source_text = "my_custom_var"
    source_text -> scan_identifier
    print(token_type_out)
    
    active = False

