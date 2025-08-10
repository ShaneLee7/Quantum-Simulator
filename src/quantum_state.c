#include "quantum_state.h"
#include "quantum_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

QuantumState* quantum_state_create(int num_qubits) {
    if (num_qubits < 1 || num_qubits > MAX_QUBITS) {
        fprintf(stderr, "Error: Number of qubits must be between 1 and %d\n", MAX_QUBITS);
        return NULL;
    }
    
    QuantumState *state = malloc(sizeof(QuantumState));
    if (!state) {
        fprintf(stderr, "Error: Failed to allocate memory for quantum state\n");
        return NULL;
    }
    
    state->num_qubits = num_qubits;
    state->num_states = 1 << num_qubits;  /* 2^num_qubits */
    
    state->amplitudes = calloc(state->num_states, sizeof(Complex));
    if (!state->amplitudes) {
        fprintf(stderr, "Error: Failed to allocate memory for amplitudes\n");
        free(state);
        return NULL;
    }
    
    return state;
}

void quantum_state_destroy(QuantumState *state) {
    if (state) {
        free(state->amplitudes);
        free(state);
    }
}

QuantumState* quantum_state_copy(const QuantumState *state) {
    if (!state) return NULL;
    
    QuantumState *copy = quantum_state_create(state->num_qubits);
    if (!copy) return NULL;
    
    for (int i = 0; i < state->num_states; i++) {
        copy->amplitudes[i] = state->amplitudes[i];
    }
    
    return copy;
}

void quantum_state_initialise_zero(QuantumState *state) {
    if (!state) return;
    
    /* Initialise to |00...0⟩ state */
    for (int i = 0; i < state->num_states; i++) {
        state->amplitudes[i] = complex_create(0.0, 0.0);
    }
    state->amplitudes[0] = complex_create(1.0, 0.0);
}

void quantum_state_initialise_equal_superposition(QuantumState *state) {
    if (!state) return;
    
    double amplitude = 1.0 / sqrt(state->num_states);
    for (int i = 0; i < state->num_states; i++) {
        state->amplitudes[i] = complex_create(amplitude, 0.0);
    }
}

void quantum_state_set_amplitude(QuantumState *state, int index, Complex amplitude) {
    if (!state || index < 0 || index >= state->num_states) {
        fprintf(stderr, "Error: Invalid state index\n");
        return;
    }
    state->amplitudes[index] = amplitude;
}

void quantum_state_normalise(QuantumState *state) {
    if (!state) return;
    
    double norm = 0.0;
    for (int i = 0; i < state->num_states; i++) {
        norm += complex_magnitude_squared(state->amplitudes[i]);
    }
    
    norm = sqrt(norm);
    if (norm < 1e-10) {
        fprintf(stderr, "Warning: Cannot normalise zero state\n");
        return;
    }
    
    for (int i = 0; i < state->num_states; i++) {
        state->amplitudes[i].real /= norm;
        state->amplitudes[i].imag /= norm;
    }
}

double quantum_state_get_probability(const QuantumState *state, int index) {
    if (!state || index < 0 || index >= state->num_states) {
        return 0.0;
    }
    return complex_magnitude_squared(state->amplitudes[index]);
}

int quantum_state_is_normalised(const QuantumState *state, double tolerance) {
    if (!state) return 0;
    
    double norm_squared = 0.0;
    for (int i = 0; i < state->num_states; i++) {
        norm_squared += complex_magnitude_squared(state->amplitudes[i]);
    }
    
    return fabs(norm_squared - 1.0) < tolerance;
}

int quantum_state_measure_all(QuantumState *state) {
    if (!state) return -1;
    
    static int seed_initialised = 0;
    if (!seed_initialised) {
        srand((unsigned int)time(NULL));
        seed_initialised = 1;
    }
    
    double random = (double)rand() / RAND_MAX;
    double cumulative_probability = 0.0;
    
    for (int i = 0; i < state->num_states; i++) {
        cumulative_probability += quantum_state_get_probability(state, i);
        if (random <= cumulative_probability) {
            /* Collapse to measured state */
            for (int j = 0; j < state->num_states; j++) {
                state->amplitudes[j] = complex_create(0.0, 0.0);
            }
            state->amplitudes[i] = complex_create(1.0, 0.0);
            return i;
        }
    }
    
    /* Fallback to last state (should rarely happen) */
    for (int j = 0; j < state->num_states; j++) {
        state->amplitudes[j] = complex_create(0.0, 0.0);
    }
    state->amplitudes[state->num_states - 1] = complex_create(1.0, 0.0);
    return state->num_states - 1;
}

int quantum_state_measure_qubit(QuantumState *state, int qubit_index) {
    if (!state || qubit_index < 0 || qubit_index >= state->num_qubits) {
        fprintf(stderr, "Error: Invalid qubit index\n");
        return -1;
    }
    
    /* Calculate probabilities for |0⟩ and |1⟩ */
    double prob_0 = 0.0, prob_1 = 0.0;
    int qubit_mask = 1 << qubit_index;
    
    for (int i = 0; i < state->num_states; i++) {
        double prob = quantum_state_get_probability(state, i);
        if (i & qubit_mask) {
            prob_1 += prob;
        } else {
            prob_0 += prob;
        }
    }
    
    /* Measure */
    static int seed_initialised = 0;
    if (!seed_initialised) {
        srand((unsigned int)time(NULL));
        seed_initialised = 1;
    }
    
    double random = (double)rand() / RAND_MAX;
    int measured_value = (random < prob_0) ? 0 : 1;
    
    /* Collapse state */
    double normalisation = (measured_value == 0) ? sqrt(prob_0) : sqrt(prob_1);
    
    if (normalisation < 1e-10) {
        fprintf(stderr, "Warning: Trying to measure qubit with zero probability\n");
        return measured_value;
    }
    
    for (int i = 0; i < state->num_states; i++) {
        if (((i & qubit_mask) != 0) != (measured_value != 0)) {
            /* This state is incompatible with measurement result */
            state->amplitudes[i] = complex_create(0.0, 0.0);
        } else {
            /* Renormalise remaining amplitudes */
            state->amplitudes[i].real /= normalisation;
            state->amplitudes[i].imag /= normalisation;
        }
    }
    
    return measured_value;
}

void quantum_state_print(const QuantumState *state) {
    if (!state) return;
    
    printf("Quantum State (%d qubits):\n", state->num_qubits);
    for (int i = 0; i < state->num_states; i++) {
        if (complex_magnitude_squared(state->amplitudes[i]) > 1e-10) {
            printf("|");
            quantum_utils_print_binary(i, state->num_qubits);
            printf("⟩: ");
            complex_print(state->amplitudes[i]);
            printf("\n");
        }
    }
}

void quantum_state_print_probabilities(const QuantumState *state) {
    if (!state) return;
    
    printf("State Probabilities:\n");
    for (int i = 0; i < state->num_states; i++) {
        double prob = quantum_state_get_probability(state, i);
        if (prob > 1e-10) {
            printf("|");
            quantum_utils_print_binary(i, state->num_qubits);
            printf("⟩: %.6f\n", prob);
        }
    }
}