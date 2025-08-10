#ifndef QUANTUM_UTILS_H
#define QUANTUM_UTILS_H

#include "quantum_state.h"
#include "quantum_circuit.h"

/* Utility functions */
void quantum_utils_print_binary(int number, int width);
void quantum_utils_print_separator(const char* title);
double quantum_utils_random_double(double min, double max);
int quantum_utils_random_int(int min, int max);

/* Example quantum algorithms */
void quantum_utils_create_bell_state(QuantumState *state, int qubit1, int qubit2);
void quantum_utils_create_ghz_state(QuantumState *state);
QuantumCircuit* quantum_utils_create_bell_circuit(void);
QuantumCircuit* quantum_utils_create_deutsch_circuit(int oracle_constant);
QuantumCircuit* quantum_utils_create_grover_circuit(int num_qubits, int target);

/* Demonstration functions */
void quantum_utils_demo_basic_gates(void);
void quantum_utils_demo_bell_states(void);
void quantum_utils_demo_superposition(void);
void quantum_utils_demo_entanglement(void);

#endif