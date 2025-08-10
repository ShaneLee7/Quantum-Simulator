#ifndef QUANTUM_STATE_H
#define QUANTUM_STATE_H

#include "complex_math.h"

#define MAX_QUBITS 20
#define MAX_STATES (1 << MAX_QUBITS)  /* 2^20 */

/**
 * Quantum state representation
 * Stores the state vector for a quantum system
 */
typedef struct {
    int num_qubits;
    int num_states;  /* 2^num_qubits */
    Complex *amplitudes;
} QuantumState;

/* State management */
QuantumState* quantum_state_create(int num_qubits);
void quantum_state_destroy(QuantumState *state);
QuantumState* quantum_state_copy(const QuantumState *state);

/* State initialisation */
void quantum_state_initialise_zero(QuantumState *state);
void quantum_state_initialise_equal_superposition(QuantumState *state);
void quantum_state_set_amplitude(QuantumState *state, int index, Complex amplitude);

/* State operations */
void quantum_state_normalise(QuantumState *state);
double quantum_state_get_probability(const QuantumState *state, int index);
int quantum_state_is_normalised(const QuantumState *state, double tolerance);

/* Measurement */
int quantum_state_measure_all(QuantumState *state);
int quantum_state_measure_qubit(QuantumState *state, int qubit_index);

/* Utility functions */
void quantum_state_print(const QuantumState *state);
void quantum_state_print_probabilities(const QuantumState *state);

#endif