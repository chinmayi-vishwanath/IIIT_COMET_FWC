import numpy as np

# Generate all 16 combinations for 4 variables A, B, C, D
# A is MSB, D is LSB
inputs = np.array([[(i>>3)&1, (i>>2)&1, (i>>1)&1, i&1] for i in range(16)])
A, B, C, D = inputs[:,0], inputs[:,1], inputs[:,2], inputs[:,3]

def solve_mux():
    # Define the MUX inputs based on the circuit diagram
    I0 = C
    I1 = D
    I2 = 1 - C
    I3 = (1 - C) & (1 - D)
    
    # Selection Logic
    # F = I0 when (A,B)=(0,0); I1 when (0,1); I2 when (1,0); I3 when (1,1)
    F = ((1-A) & (1-B) & I0) | \
        ((1-A) & B & I1)     | \
        (A & (1-B) & I2)     | \
        (A & B & I3)
    
    # Find minterms (where output is 1)
    minterms = np.where(F == 1)[0]
    
    print("--- Truth Table Minterms ---")
    print(f"Minterms found: {list(minterms)}")
    print(f"Boolean Function: F = Σm{tuple(minterms)}")

if __name__ == "__main__":
    solve_mux()
