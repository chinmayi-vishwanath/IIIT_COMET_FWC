import numpy as np

# Define input combinations for two variables A and B
# Truth table inputs: (0,0), (0,1), (1,0), (1,1)
A = np.array([0, 0, 1, 1])
B = np.array([0, 1, 0, 1])

def get_truth_tables():
    # --- Column A Gates ---
    gate_P = 1 - (A | B)           # NOR
    gate_Q = 1 - (A & B)           # NAND
    gate_R = A ^ B                 # XOR
    gate_S = 1 - (A ^ B)           # XNOR
 
    col_A = {'P': gate_P, 'Q': gate_Q, 'R': gate_R, 'S': gate_S}

    # --- Column B Gates ---
    # 1. XOR with one input inverted (Results in XNOR, then inverted again?
    # Let's look at the bubble on the input)
    gate_1 = (1 - A) ^ B           # XOR with inverted A = XNOR.
                                   # Note: If it's XNOR with inverted input, it's XOR.
    
    # 2. Bubbled OR (Inverted inputs to an OR gate)
    gate_2 = (1 - A) | (1 - B)     # Equivalent to NAND
    
    # 3. Bubbled XOR (Inverted inputs to XNOR)
    gate_3 = 1 - ((1 - A) ^ B)
    
    # 4. Bubbled AND (Inverted inputs to an AND gate)
    gate_4 = (1 - A) & (1 - B)     # Equivalent to NOR

    col_B = {1: gate_1, 2: gate_2, 3: gate_3, 4: gate_4}
    
    return col_A, col_B

def match_gates():
    col_A, col_B = get_truth_tables()
    matches = []
    
    print("--- Logic Gate Matching Results ---")
    for key_A, val_A in col_A.items():
        for key_B, val_B in col_B.items():
            if np.array_equal(val_A, val_B):
                matches.append(f"{key_A}-{key_B}")
                print(f"Gate {key_A} matches Gate {key_B}")
    
    print("\nFinal Match Sequence:", ", ".join(sorted(matches)))

if __name__ == "__main__":
    match_gates()
