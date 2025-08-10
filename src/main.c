#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "quantum_state.h"
#include "quantum_gates.h"
#include "quantum_circuit.h"
#include "quantum_utils.h"
#include "complex_math.h"

void print_welcome_message(void) {
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                    Quantum Computer Simulator                    ║\n");
    printf("║                         Written in C                             ║\n");
    printf("║                                                                  ║\n");
    printf("║          A quantum simulator supporting up to 20 qubit           ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n\n");
}

void demo_circuit_execution(void) {
    quantum_utils_print_separator("QUANTUM CIRCUIT EXECUTION");
    
    /* Create and execute Bell circuit */
    QuantumCircuit *bell_circuit = quantum_utils_create_bell_circuit();
    if (!bell_circuit) return;
    
    quantum_circuit_print(bell_circuit);
    
    QuantumState *state = quantum_state_create(2);
    if (!state) {
        quantum_circuit_destroy(bell_circuit);
        return;
    }
    
    quantum_state_initialise_zero(state);
    printf("Initial state:\n");
    quantum_state_print(state);
    
    quantum_circuit_execute(bell_circuit, state);
    
    printf("\nFinal state after circuit execution:\n");
    quantum_state_print(state);
    
    quantum_state_destroy(state);
    quantum_circuit_destroy(bell_circuit);
}

void demo_deutsch_algorithm(void) {
    quantum_utils_print_separator("DEUTSCH ALGORITHM DEMONSTRATION");
    
    printf("Testing constant function f(x) = 0:\n");
    QuantumCircuit *deutsch_circuit = quantum_utils_create_deutsch_circuit(0);
    if (deutsch_circuit) {
        QuantumState *state = quantum_state_create(2);
        if (state) {
            quantum_state_initialise_zero(state);
            quantum_circuit_execute(deutsch_circuit, state);
            quantum_state_destroy(state);
        }
        quantum_circuit_destroy(deutsch_circuit);
    }
    
    printf("\nTesting constant function f(x) = 1:\n");
    deutsch_circuit = quantum_utils_create_deutsch_circuit(1);
    if (deutsch_circuit) {
        QuantumState *state = quantum_state_create(2);
        if (state) {
            quantum_state_initialise_zero(state);
            quantum_circuit_execute(deutsch_circuit, state);
            quantum_state_destroy(state);
        }
        quantum_circuit_destroy(deutsch_circuit);
    }
}

void demo_custom_circuit(void) {
    quantum_utils_print_separator("CUSTOM CIRCUIT DEMONSTRATION");
    
    QuantumCircuit *circuit = quantum_circuit_create(3, "Custom 3-Qubit Circuit");
    if (!circuit) return;
    
    /* Build a custom circuit */
    quantum_circuit_add_hadamard(circuit, 0);
    quantum_circuit_add_hadamard(circuit, 1);
    quantum_circuit_add_cnot(circuit, 0, 2);
    quantum_circuit_add_cnot(circuit, 1, 2);
    quantum_circuit_add_rotation_z(circuit, 2, M_PI / 4.0);
    quantum_circuit_add_measure_all(circuit);
    
    quantum_circuit_print(circuit);
    
    QuantumState *state = quantum_state_create(3);
    if (state) {
        quantum_state_initialise_zero(state);
        printf("Initial state:\n");
        quantum_state_print_probabilities(state);
        
        quantum_circuit_execute(circuit, state);
        
        printf("\nFinal state:\n");
        quantum_state_print_probabilities(state);
        
        quantum_state_destroy(state);
    }
    
    quantum_circuit_destroy(circuit);
}

void interactive_mode(void) {
    quantum_utils_print_separator("INTERACTIVE MODE");
    
    int num_qubits;
    printf("Enter number of qubits (1-%d): ", MAX_QUBITS);
    if (scanf("%d", &num_qubits) != 1 || num_qubits < 1 || num_qubits > MAX_QUBITS) {
        printf("Invalid input. Using 2 qubits.\n");
        num_qubits = 2;
    }
    
    QuantumState *state = quantum_state_create(num_qubits);
    if (!state) return;
    
    quantum_state_initialise_zero(state);
    
    int choice;
    do {
        printf("\n--- Quantum State Manipulation ---\n");
        printf("Current state:\n");
        quantum_state_print_probabilities(state);
        
        printf("\n1. Apply Hadamard gate\n");
        printf("2. Apply Pauli-X gate\n");
        printf("3. Apply CNOT gate\n");
        printf("4. Measure all qubits\n");
        printf("5. Reset to |0...0⟩\n");
        printf("6. Create Bell state (2 qubits)\n");
        printf("0. Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            choice = 0;
        }
        
        int qubit, control, target;
        
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
                
            case 4:
                {
                    int result = quantum_state_measure_all(state);
                    printf("Measurement result: |");
                    quantum_utils_print_binary(result, num_qubits);
                    printf("⟩ (decimal: %d)\n", result);
                }
                break;
                
            case 5:
                quantum_state_initialise_zero(state);
                printf("Reset to |0...0⟩ state\n");
                break;
                
            case 6:
                if (num_qubits >= 2) {
                    quantum_utils_create_bell_state(state, 0, 1);
                    printf("Created Bell state with qubits 0 and 1\n");
                } else {
                    printf("Need at least 2 qubits for Bell state\n");
                }
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
    
    /* Run demonstrations */
    quantum_utils_demo_basic_gates();
    quantum_utils_demo_superposition();
    quantum_utils_demo_bell_states();
    quantum_utils_demo_entanglement();
    
    demo_circuit_execution();
    demo_deutsch_algorithm();
    demo_custom_circuit();
    
    /* Interactive mode */
    char choice;
    printf("\nWould you like to enter interactive mode? (y/n): ");
    if (scanf(" %c", &choice) == 1 && (choice == 'y' || choice == 'Y')) {
        interactive_mode();
    }
    
    printf("\nThank you for using the Quantum Computer Simulator!\n");
    printf("This simulator demonstrates the beauty of quantum mechanics\n");
    printf("and the fascinating world of quantum computing.\n\n");
    
    return 0;
}