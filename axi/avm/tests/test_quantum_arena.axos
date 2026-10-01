@C_Include("<stdio.h>")
@C_Include("<math.h>")
@C_Include("<complex.h>")

@StaticArena
state psi: cfloat[4]

@StaticArena
state embedding_arena: float[768]

state active: bool = True

while active:
    psi[0] = c_make(1.0, 0.0)
    psi[1] = c_make(0.0, 1.0)
    psi[2] = c_exp(c_make(0.0, 3.14159265))
    psi[3] = c_make(0.7071067, 0.7071067)

    embedding_arena[0] = 0.5
    embedding_arena[767] = 0.99

    state abs_psi2: float = c_abs(psi[2])
    state real_psi2: float = c_real(psi[2])

    print("--- Milestone 3: Quantum State & Arena Verification ---")
    print("Magnitude of e^(i*pi) (Expected 1.0):")
    print(abs_psi2)
    print("Real component of e^(i*pi) (Expected -1.0):")
    print(real_psi2)
    print("768-dim Arena Tail Value (Expected 0.99):")
    print(embedding_arena[767])

    active = False

