#ifndef QUANTUM_UTILS_H
#define QUANTUM_UTILS_H

#include "quantum_state.h"
#include "quantum_circuit.h"

/* Utility functions for printing and formatting */
void quantum_utils_print_binary(int number, int width);
void quantum_utils_print_separator(const char* title);

/* Random number generation */
double quantum_utils_random_double(double min, double max);
int quantum_utils_random_int(int min, int max);

/* State preparation utilities */
void quantum_utils_create_bell_state(QuantumState *state, int qubit1, int qubit2);
void quantum_utils_create_ghz_state(QuantumState *state);

/* Circuit creation utilities */
QuantumCircuit* quantum_utils_create_bell_circuit(void);
QuantumCircuit* quantum_utils_create_deutsch_circuit(int oracle_constant);
QuantumCircuit* quantum_utils_create_grover_circuit(int num_qubits, int target);

/* Grover's algorithm components */
void quantum_utils_add_grover_oracle(QuantumCircuit *circuit, int target, int num_qubits);
void quantum_utils_add_grover_diffusion(QuantumCircuit *circuit, int num_qubits);
void quantum_utils_apply_grover_oracle(QuantumState *state, int target);
void quantum_utils_apply_grover_diffusion(QuantumState *state);
void quantum_utils_apply_grover_diffusion_sparse(QuantumState *state, int* valid_states, int num_valid);

/* Algorithm implementations */
void quantum_utils_run_grover_algorithm(QuantumState *state);
void quantum_utils_run_shor_algorithm(QuantumState *state);

/* Shor's algorithm components */
void quantum_utils_complete_factorization(QuantumState *state, int N);
int quantum_utils_find_small_factor(int n);
int quantum_utils_shor_find_factor(QuantumState *state, int N);

/* Quantum Fourier Transform utilities */
void quantum_utils_simplified_qft(QuantumState *state);
void quantum_utils_controlled_phase(QuantumState *state, int control, int target, double angle);

/* Helper functions for Shor's algorithm */
int quantum_utils_gcd(int a, int b);
int quantum_utils_find_period(int a, int N);
int quantum_utils_is_prime(int n);

/* Demonstration functions */
void quantum_utils_demo_basic_gates(void);
void quantum_utils_demo_bell_states(void);
void quantum_utils_demo_superposition(void);
void quantum_utils_demo_entanglement(void);

#endif /* QUANTUM_UTILS_H */