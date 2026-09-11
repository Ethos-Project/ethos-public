@C_Include("<stdio.h>")
@C_Include("<math.h>")

state v1: float[3]
state v2: float[3]
state active: bool = True

while active:
    v1[0] = 1.0
    v1[1] = 2.0
    v1[2] = 3.0

    v2[0] = 4.0
    v2[1] = 5.0
    v2[2] = 6.0

    state d: float = vec_dot(v1, v2, 3)
    state n: float = vec_norm(v1, 3)

    print("--- Vector Math Verification ---")
    print("Dot Product (Expected 32.0):")
    print(d)
    print("Vector Norm (Expected ~3.741657):")
    print(n)

    active = False

