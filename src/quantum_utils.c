#include "quantum_utils.h"
#include "quantum_gates.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void quantum_utils_print_binary(int number, int width) {
    /* Print binary with qubit 0 as rightmost bit (little-endian) */
    for (int i = width - 1; i >= 0; i--) {
        printf("%d", (number >> i) & 1);
    }
}

void quantum_utils_print_separator(const char* title) {
    printf("\n");
    printf("=================================================\n");
    printf(" %s\n", title);
    printf("=================================================\n\n");
}

double quantum_utils_random_double(double min, double max) {
    static int seed_initialised = 0;
    if (!seed_initialised) {
        srand((unsigned int)time(NULL));
        seed_initialised = 1;
    }
    return min + (max - min) * ((double)rand() / RAND_MAX);
}

int quantum_utils_random_int(int min, int max) {
    static int seed_initialised = 0;
    if (!seed_initialised) {
        srand((unsigned int)time(NULL));
        seed_initialised = 1;
    }
    return min + rand() % (max - min + 1);
}

void quantum_utils_create_bell_state(QuantumState *state, int qubit1, int qubit2) {
    if (!state || qubit1 < 0 || qubit2 < 0 || 
        qubit1 >= state->num_qubits || qubit2 >= state->num_qubits || 
        qubit1 == qubit2) {
        fprintf(stderr, "Error: Invalid qubits for Bell state creation\n");
        return;
    }
    
    quantum_state_initialise_zero(state);
    gate_hadamard(state, qubit1);
    gate_cnot(state, qubit1, qubit2);
}

void quantum_utils_create_ghz_state(QuantumState *state) {
    if (!state || state->num_qubits < 2) {
        fprintf(stderr, "Error: Need at least 2 qubits for GHZ state\n");
        return;
    }
    
    quantum_state_initialise_zero(state);
    gate_hadamard(state, 0);
    
    for (int i = 1; i < state->num_qubits; i++) {
        gate_cnot(state, 0, i);
    }
}

QuantumCircuit* quantum_utils_create_bell_circuit(void) {
    QuantumCircuit *circuit = quantum_circuit_create(2, "Bell State Circuit");
    if (!circuit) return NULL;
    
    quantum_circuit_add_hadamard(circuit, 0);
    quantum_circuit_add_cnot(circuit, 0, 1);
    quantum_circuit_add_measure_all(circuit);
    
    return circuit;
}

QuantumCircuit* quantum_utils_create_deutsch_circuit(int oracle_constant) {
    QuantumCircuit *circuit = quantum_circuit_create(2, "Deutsch Algorithm Circuit");
    if (!circuit) return NULL;
    
    /* Initialise */
    quantum_circuit_add_pauli_x(circuit, 1);  /* Put ancilla in |1⟩ */
    quantum_circuit_add_hadamard(circuit, 0);  /* Put input in superposition */
    quantum_circuit_add_hadamard(circuit, 1);  /* Put ancilla in |−⟩ */
    
    /* Oracle */
    if (oracle_constant) {
        quantum_circuit_add_pauli_z(circuit, 0);  /* Constant function f(x) = 1 */
    }
    /* For balanced function f(x) = x, we would add CNOT(0,1) */
    /* For constant function f(x) = 0, we do nothing */
    
    /* Final Hadamard and measurement */
    quantum_circuit_add_hadamard(circuit, 0);
    quantum_circuit_add_measure(circuit, 0);
    
    return circuit;
}

QuantumCircuit* quantum_utils_create_grover_circuit(int num_qubits, int target) {
    if (num_qubits < 1 || target < 0 || target >= (1 << num_qubits)) {
        fprintf(stderr, "Error: Invalid parameters for Grover circuit\n");
        return NULL;
    }
    
    QuantumCircuit *circuit = quantum_circuit_create(num_qubits, "Grover's Algorithm Circuit");
    if (!circuit) return NULL;
    
    /* Initialise superposition */
    for (int i = 0; i < num_qubits; i++) {
        quantum_circuit_add_hadamard(circuit, i);
    }
    
    /* Simplified Grover iteration (oracle + diffusion) */
    int iterations = (int)(M_PI * sqrt(1 << num_qubits) / 4);
    
    for (int iter = 0; iter < iterations; iter++) {
        /* Oracle: flip phase of target state */
        /* This is a simplified oracle that flips all states */
        for (int i = 0; i < num_qubits; i++) {
            quantum_circuit_add_pauli_z(circuit, i);
        }
        
        /* Diffusion operator */
        for (int i = 0; i < num_qubits; i++) {
            quantum_circuit_add_hadamard(circuit, i);
            quantum_circuit_add_pauli_z(circuit, i);
        }
        for (int i = 0; i < num_qubits; i++) {
            quantum_circuit_add_hadamard(circuit, i);
        }
    }
    
    quantum_circuit_add_measure_all(circuit);
    
    return circuit;
}

void quantum_utils_demo_basic_gates(void) {
    quantum_utils_print_separator("BASIC QUANTUM GATES DEMONSTRATION");
    
    QuantumState *state = quantum_state_create(1);
    if (!state) return;
    
    printf("Starting with |0⟩ state:\n");
    quantum_state_initialise_zero(state);
    quantum_state_print(state);
    
    printf("\nApplying Pauli-X gate (bit flip):\n");
    gate_pauli_x(state, 0);
    quantum_state_print(state);
    
    printf("\nApplying Hadamard gate (creates superposition):\n");
    gate_hadamard(state, 0);
    quantum_state_print(state);
    
    printf("\nProbabilities:\n");
    quantum_state_print_probabilities(state);
    
    printf("\nApplying Pauli-Z gate (phase flip):\n");
    gate_pauli_z(state, 0);
    quantum_state_print(state);
    
    quantum_state_destroy(state);
}

void quantum_utils_demo_bell_states(void) {
    quantum_utils_print_separator("BELL STATES DEMONSTRATION");
    
    QuantumState *state = quantum_state_create(2);
    if (!state) return;
    
    printf("Creating Bell state |Φ+⟩ = (|00⟩ + |11⟩)/√2:\n");
    quantum_utils_create_bell_state(state, 0, 1);
    quantum_state_print(state);
    
    printf("\nProbabilities:\n");
    quantum_state_print_probabilities(state);
    
    printf("\nMeasuring the Bell state:\n");
    for (int i = 0; i < 5; i++) {
        QuantumState *copy = quantum_state_copy(state);
        int result = quantum_state_measure_all(copy);
        printf("Measurement %d: |", i + 1);
        quantum_utils_print_binary(result, 2);
        printf("⟩\n");
        quantum_state_destroy(copy);
    }
    
    quantum_state_destroy(state);
}

void quantum_utils_demo_superposition(void) {
    quantum_utils_print_separator("SUPERPOSITION DEMONSTRATION");
    
    QuantumState *state = quantum_state_create(3);
    if (!state) return;
    
    printf("Creating equal superposition of all 3-qubit states:\n");
    quantum_state_initialise_equal_superposition(state);
    quantum_state_print_probabilities(state);
    
    printf("\nApplying Hadamard to all qubits starting from |000⟩:\n");
    quantum_state_initialise_zero(state);
    for (int i = 0; i < 3; i++) {
        gate_hadamard(state, i);
    }
    quantum_state_print_probabilities(state);
    
    quantum_state_destroy(state);
}

void quantum_utils_demo_entanglement(void) {
    quantum_utils_print_separator("ENTANGLEMENT DEMONSTRATION");
    
    QuantumState *state = quantum_state_create(3);
    if (!state) return;
    
    printf("Creating GHZ state |GHZ⟩ = (|000⟩ + |111⟩)/√2:\n");
    quantum_utils_create_ghz_state(state);
    quantum_state_print(state);
    
    printf("\nMeasuring individual qubits (note the correlations):\n");
    for (int trial = 0; trial < 3; trial++) {
        QuantumState *copy = quantum_state_copy(state);
        printf("Trial %d:\n", trial + 1);
        
        for (int qubit = 0; qubit < 3; qubit++) {
            int result = quantum_state_measure_qubit(copy, qubit);
            printf("  Qubit %d: %d\n", qubit, result);
        }
        
        quantum_state_destroy(copy);
    }
    
    quantum_state_destroy(state);
}