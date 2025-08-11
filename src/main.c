#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Define M_PI if not available */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "quantum_state.h"
#include "quantum_gates.h"
#include "quantum_circuit.h"
#include "quantum_utils.h"
#include "complex_math.h"

void print_welcome_message(void) {
    printf("╔═════════════════════════════════════════════════════════════════╗\n");
    printf("║                    Quantum Computer Simulator                   ║\n");
    printf("║                           Written in C                          ║\n");
    printf("║                                                                 ║\n");
    printf("║    Interactive quantum simulator supporting up to 20 qubits     ║\n");
    printf("╚═════════════════════════════════════════════════════════════════╝\n\n");
}

void interactive_mode(void) {
    quantum_utils_print_separator("INTERACTIVE QUANTUM SIMULATOR");
    
    int num_qubits;
    printf("Enter number of qubits (1-%d): ", MAX_QUBITS);
    if (scanf("%d", &num_qubits) != 1 || num_qubits < 1 || num_qubits > MAX_QUBITS) {
        printf("Invalid input. Using 3 qubits.\n");
        num_qubits = 3;
    }
    
    QuantumState *state = quantum_state_create(num_qubits);
    if (!state) return;
    
    quantum_state_initialise_zero(state);
    
    int choice;
    do {
        printf("\n--- Quantum State Manipulation ---\n");
        printf("Current state:\n");
        quantum_state_print_probabilities(state);
        
        printf("\n=== Single Qubit Gates ===\n");
        printf("1. Apply Hadamard gate\n");
        printf("2. Apply Pauli-X gate\n");
        printf("3. Apply Pauli-Y gate\n");
        printf("4. Apply Pauli-Z gate\n");
        printf("5. Apply Phase gate\n");
        printf("6. Apply Rotation-X gate\n");
        printf("7. Apply Rotation-Y gate\n");
        printf("8. Apply Rotation-Z gate\n");
        
        printf("\n=== Two Qubit Gates ===\n");
        printf("9. Apply CNOT gate\n");
        printf("10. Apply CZ gate\n");
        printf("11. Apply SWAP gate\n");
        
        printf("\n=== Quantum Algorithms ===\n");
        printf("12. Run Grover's Algorithm\n");
        printf("13. Run Shor's Algorithm (simplified)\n");
        printf("14. Create Bell state\n");
        printf("15. Create GHZ state\n");
        
        printf("\n=== Measurement & Utility ===\n");
        printf("16. Measure specific qubit\n");
        printf("17. Measure all qubits\n");
        printf("18. Reset to |0...0⟩\n");
        printf("19. Create equal superposition\n");
        printf("20. Show quantum state amplitudes\n");
        
        printf("\n0. Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            choice = 0;
        }
        
        int qubit, qubit2, control, target;
        double angle, phase;
        
        switch (choice) {
            case 1:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                if (scanf("%d", &qubit) == 1 && qubit >= 0 && qubit < num_qubits) {
                    gate_hadamard(state, qubit);
                    printf("Applied Hadamard to qubit %d\n", qubit);
                }
                break;
                
            case 2:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                if (scanf("%d", &qubit) == 1 && qubit >= 0 && qubit < num_qubits) {
                    gate_pauli_x(state, qubit);
                    printf("Applied Pauli-X to qubit %d\n", qubit);
                }
                break;
                
            case 3:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                if (scanf("%d", &qubit) == 1 && qubit >= 0 && qubit < num_qubits) {
                    gate_pauli_y(state, qubit);
                    printf("Applied Pauli-Y to qubit %d\n", qubit);
                }
                break;
                
            case 4:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                if (scanf("%d", &qubit) == 1 && qubit >= 0 && qubit < num_qubits) {
                    gate_pauli_z(state, qubit);
                    printf("Applied Pauli-Z to qubit %d\n", qubit);
                }
                break;
                
            case 5:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                scanf("%d", &qubit);
                printf("Enter phase angle (radians): ");
                scanf("%lf", &phase);
                if (qubit >= 0 && qubit < num_qubits) {
                    gate_phase(state, qubit, phase);
                    printf("Applied phase gate (%.3f rad) to qubit %d\n", phase, qubit);
                }
                break;
                
            case 6:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                scanf("%d", &qubit);
                printf("Enter rotation angle (radians): ");
                scanf("%lf", &angle);
                if (qubit >= 0 && qubit < num_qubits) {
                    gate_rotation_x(state, qubit, angle);
                    printf("Applied RX(%.3f) to qubit %d\n", angle, qubit);
                }
                break;
                
            case 7:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                scanf("%d", &qubit);
                printf("Enter rotation angle (radians): ");
                scanf("%lf", &angle);
                if (qubit >= 0 && qubit < num_qubits) {
                    gate_rotation_y(state, qubit, angle);
                    printf("Applied RY(%.3f) to qubit %d\n", angle, qubit);
                }
                break;
                
            case 8:
                printf("Enter qubit index (0-%d): ", num_qubits - 1);
                scanf("%d", &qubit);
                printf("Enter rotation angle (radians): ");
                scanf("%lf", &angle);
                if (qubit >= 0 && qubit < num_qubits) {
                    gate_rotation_z(state, qubit, angle);
                    printf("Applied RZ(%.3f) to qubit %d\n", angle, qubit);
                }
                break;
                
            case 9:
                if (num_qubits < 2) {
                    printf("Need at least 2 qubits for CNOT\n");
                    break;
                }
                printf("Enter control qubit (0-%d): ", num_qubits - 1);
                scanf("%d", &control);
                printf("Enter target qubit (0-%d): ", num_qubits - 1);
                scanf("%d", &target);
                if (control >= 0 && control < num_qubits && 
                    target >= 0 && target < num_qubits && control != target) {
                    gate_cnot(state, control, target);
                    printf("Applied CNOT with control=%d, target=%d\n", control, target);
                }
                break;
                
            case 10:
                if (num_qubits < 2) {
                    printf("Need at least 2 qubits for CZ\n");
                    break;
                }
                printf("Enter control qubit (0-%d): ", num_qubits - 1);
                scanf("%d", &control);
                printf("Enter target qubit (0-%d): ", num_qubits - 1);
                scanf("%d", &target);
                if (control >= 0 && control < num_qubits && 
                    target >= 0 && target < num_qubits && control != target) {
                    gate_cz(state, control, target);
                    printf("Applied CZ with control=%d, target=%d\n", control, target);
                }
                break;
                
            case 11:
                if (num_qubits < 2) {
                    printf("Need at least 2 qubits for SWAP\n");
                    break;
                }
                printf("Enter first qubit (0-%d): ", num_qubits - 1);
                scanf("%d", &qubit);
                printf("Enter second qubit (0-%d): ", num_qubits - 1);
                scanf("%d", &qubit2);
                if (qubit >= 0 && qubit < num_qubits && 
                    qubit2 >= 0 && qubit2 < num_qubits && qubit != qubit2) {
                    gate_swap(state, qubit, qubit2);
                    printf("Applied SWAP between qubits %d and %d\n", qubit, qubit2);
                }
                break;
                
            case 12:
                quantum_utils_run_grover_algorithm(state);
                break;
                
            case 13:
                quantum_utils_run_shor_algorithm(state);
                break;
                
            case 14:
                if (num_qubits >= 2) {
                    printf("Enter first qubit (0-%d): ", num_qubits - 1);
                    scanf("%d", &qubit);
                    printf("Enter second qubit (0-%d): ", num_qubits - 1);
                    scanf("%d", &qubit2);
                    if (qubit >= 0 && qubit < num_qubits && 
                        qubit2 >= 0 && qubit2 < num_qubits && qubit != qubit2) {
                        quantum_utils_create_bell_state(state, qubit, qubit2);
                        printf("Created Bell state with qubits %d and %d\n", qubit, qubit2);
                    }
                } else {
                    printf("Need at least 2 qubits for Bell state\n");
                }
                break;
                
            case 15:
                if (num_qubits >= 2) {
                    quantum_utils_create_ghz_state(state);
                    printf("Created GHZ state with all %d qubits\n", num_qubits);
                } else {
                    printf("Need at least 2 qubits for GHZ state\n");
                }
                break;
                
            case 16:
                printf("Enter qubit to measure (0-%d): ", num_qubits - 1);
                if (scanf("%d", &qubit) == 1 && qubit >= 0 && qubit < num_qubits) {
                    int result = quantum_state_measure_qubit(state, qubit);
                    printf("Measured qubit %d: %d\n", qubit, result);
                }
                break;
                
            case 17:
                {
                    int result = quantum_state_measure_all(state);
                    printf("Measurement result: |");
                    quantum_utils_print_binary(result, num_qubits);
                    printf("⟩ (decimal: %d)\n", result);
                }
                break;
                
            case 18:
                quantum_state_initialise_zero(state);
                printf("Reset to |0...0⟩ state\n");
                break;
                
            case 19:
                quantum_state_initialise_equal_superposition(state);
                printf("Created equal superposition of all states\n");
                break;
                
            case 20:
                printf("\nQuantum state amplitudes:\n");
                quantum_state_print(state);
                break;
                
            case 0:
                printf("Exiting interactive mode...\n");
                break;
                
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
        
    } while (choice != 0);
    
    quantum_state_destroy(state);
}

int main(void) {
    print_welcome_message();
    interactive_mode();
    
    printf("\nThank you for using the Quantum Computer Simulator!\n");
    return 0;
}