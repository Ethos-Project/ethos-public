@C_Include("<stdio.h>")
@C_Include("<string.h>")
@C_Include("<stdbool.h>")

@StaticArena
state token_id_in: float = 0.0

@StaticArena
state token_id_out: float = 0.0

# 2. THE CHIP (The Lexer Node)
node check_if_keyword(input_val: float) -> float:
    # Pure Axiom Dataflow logic! No C needed!
    if token_id_in == 1.0:
        print("[LEXER CHIP] Recognized keyword: state")
        token_id_out = 100.0
        
    if token_id_in == 2.0:
        print("[LEXER CHIP] Recognized keyword: node")
        token_id_out = 100.0
        
    if token_id_in == 3.0:
        print("[LEXER CHIP] Unknown token mapped. Skipping.")
        token_id_out = 404.0

# 3. THE WIRES (Execution Loop)
state active: bool = True

while active:
    print("--- Booting Axiom Lexer Circuit ---")
    
    print("Sending Token ID 1.0 ('state')...")
    token_id_in = 1.0
    token_id_in -> check_if_keyword
    print(token_id_out)
    
    print("Sending Token ID 2.0 ('node')...")
    token_id_in = 2.0
    token_id_in -> check_if_keyword
    print(token_id_out)
    
    print("Sending Token ID 3.0 ('random_word')...")
    token_id_in = 3.0
    token_id_in -> check_if_keyword
    print(token_id_out)
    
    active = False

