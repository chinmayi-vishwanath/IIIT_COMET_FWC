import numpy as np
import matplotlib.pyplot as plt

# 1. Generate all possible input combinations for A, B, C
# Combinations: (0,0,0), (0,0,1), (0,1,0), (0,1,1), (1,0,0), (1,0,1), (1,1,0), (1,1,1)
A = np.array([0, 0, 0, 0, 1, 1, 1, 1])
B = np.array([0, 0, 1, 1, 0, 0, 1, 1])
C = np.array([0, 1, 0, 1, 0, 1, 0, 1])

def solve_logic():
    # Gate 1: XOR(A, B)
    G1 = A ^ B
    
    # Gate 2: XNOR(A, B)
    G2 = 1 - (A ^ B)
    
    # Gate 3: Final 3-input XNOR(G1, G2, C)
    # Logic: 1 if (G1 ^ G2 ^ C) is 0
    F = 1 - (G1 ^ G2 ^ C)
    
    # Identify the index where F is 1
    indices = np.where(F == 1)[0]
    
    print(f"{'A':<5} {'B':<5} {'C':<5} | {'F':<5}")
    print("-" * 25)
    for i in range(len(A)):
        print(f"{A[i]:<5} {B[i]:<5} {C[i]:<5} | {F[i]:<5}")
        
    print("\nCombinations where F = 1:")
    for idx in indices:
        print(f"Option: A={A[idx]}, B={B[idx]}, C={C[idx]}")

# Execute the solver
solve_logic()

# Optional: Visualization (consistent with your sample code style)
plt.step(range(8), (1 - ( (A^B) ^ (1-(A^B)) ^ C )), where='post', label='Output F')
plt.xlabel('Input Combination Index')
plt.ylabel('Logic Level')
plt.title('Logic Circuit Output F')
plt.grid(True)
plt.ylim(-0.5, 1.5)
plt.legend()
plt.show()
