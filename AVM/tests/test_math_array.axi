@C_Include("<stdio.h>")
@C_Include("<math.h>")

state state_vector: float[4]
state active: bool = True

while active:
    state_vector[0] = math.sqrt(16.0)
    state_vector[1] = math.exp(1.0)
    state_vector[2] = math.cos(0.0)
    state_vector[3] = math.pow(2.0, 3.0)

    print("--- Quantum State Vector [0..3] ---")
    print(state_vector[0])
    print(state_vector[1])
    print(state_vector[2])
    print(state_vector[3])
    
    active = False

