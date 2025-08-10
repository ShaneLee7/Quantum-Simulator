#include "quantum_gates.h"
#include <math.h>
#include <stdio.h>

int validate_single_qubit_gate(const QuantumState *state, int qubit) {
    if (!state) {
        fprintf(stderr, "Error: Null quantum state\n");
        return 0;
    }
    if (qubit < 0 || qubit >= state->num_qubits) {
        fprintf(stderr, "Error: Qubit index %d out of range [0, %d)\n", qubit, state->num_qubits);
        return 0;
    }
    return 1;
}

int validate_two_qubit_gate(const QuantumState *state, int qubit1, int qubit2) {
    if (!validate_single_qubit_gate(state, qubit1) || 
        !validate_single_qubit_gate(state, qubit2)) {
        return 0;
    }
    if (qubit1 == qubit2) {
        fprintf(stderr, "Error: Cannot apply two-qubit gate to the same qubit\n");
        return 0;
    }
    return 1;
}

void gate_pauli_x(QuantumState *state, int qubit) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    int qubit_mask = 1 << qubit;
    
    /* Iterate through all basis states and swap amplitudes */
    for (int i = 0; i < state->num_states; i++) {
        if (!(i & qubit_mask)) {  /* Only process states where this qubit is 0 */
            int flipped_state = i | qubit_mask;  /* Flip this qubit to 1 */
            
            /* Swap amplitudes */
            Complex temp = state->amplitudes[i];
            state->amplitudes[i] = state->amplitudes[flipped_state];
            state->amplitudes[flipped_state] = temp;
        }
    }
}

void gate_pauli_y(QuantumState *state, int qubit) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    int qubit_mask = 1 << qubit;
    Complex i_unit = complex_create(0.0, 1.0);
    Complex neg_i_unit = complex_create(0.0, -1.0);
    
    for (int i = 0; i < state->num_states; i++) {
        if (!(i & qubit_mask)) {  /* Only process states where this qubit is 0 */
            int flipped_state = i | qubit_mask;  /* Flip this qubit to 1 */
            
            Complex temp0 = state->amplitudes[i];
            Complex temp1 = state->amplitudes[flipped_state];
            
            state->amplitudes[i] = complex_multiply(neg_i_unit, temp1);
            state->amplitudes[flipped_state] = complex_multiply(i_unit, temp0);
        }
    }
}

void gate_pauli_z(QuantumState *state, int qubit) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    int qubit_mask = 1 << qubit;
    
    for (int i = 0; i < state->num_states; i++) {
        if (i & qubit_mask) {
            state->amplitudes[i].real = -state->amplitudes[i].real;
            state->amplitudes[i].imag = -state->amplitudes[i].imag;
        }
    }
}

void gate_hadamard(QuantumState *state, int qubit) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    int qubit_mask = 1 << qubit;
    double factor = 1.0 / sqrt(2.0);
    
    for (int i = 0; i < state->num_states; i++) {
        if (!(i & qubit_mask)) {  /* Only process states where this qubit is 0 */
            int flipped_state = i | qubit_mask;  /* Corresponding state with qubit = 1 */
            
            Complex amp0 = state->amplitudes[i];
            Complex amp1 = state->amplitudes[flipped_state];
            
            state->amplitudes[i] = complex_create(
                factor * (amp0.real + amp1.real),
                factor * (amp0.imag + amp1.imag)
            );
            state->amplitudes[flipped_state] = complex_create(
                factor * (amp0.real - amp1.real),
                factor * (amp0.imag - amp1.imag)
            );
        }
    }
}

void gate_phase(QuantumState *state, int qubit, double phase) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    int qubit_mask = 1 << qubit;
    Complex phase_factor = complex_from_polar(1.0, phase);
    
    for (int i = 0; i < state->num_states; i++) {
        if (i & qubit_mask) {
            state->amplitudes[i] = complex_multiply(state->amplitudes[i], phase_factor);
        }
    }
}

void gate_rotation_x(QuantumState *state, int qubit, double angle) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    int qubit_mask = 1 << qubit;
    double cos_half = cos(angle / 2.0);
    double sin_half = sin(angle / 2.0);
    Complex neg_i_sin = complex_create(0.0, -sin_half);
    
    for (int i = 0; i < state->num_states; i++) {
        if (!(i & qubit_mask)) {  /* Only process states where this qubit is 0 */
            int flipped_state = i | qubit_mask;
            
            Complex amp0 = state->amplitudes[i];
            Complex amp1 = state->amplitudes[flipped_state];
            
            state->amplitudes[i] = complex_add(
                complex_create(cos_half * amp0.real, cos_half * amp0.imag),
                complex_multiply(neg_i_sin, amp1)
            );
            state->amplitudes[flipped_state] = complex_add(
                complex_multiply(neg_i_sin, amp0),
                complex_create(cos_half * amp1.real, cos_half * amp1.imag)
            );
        }
    }
}

void gate_rotation_y(QuantumState *state, int qubit, double angle) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    int qubit_mask = 1 << qubit;
    double cos_half = cos(angle / 2.0);
    double sin_half = sin(angle / 2.0);
    
    for (int i = 0; i < state->num_states; i++) {
        if (!(i & qubit_mask)) {  /* Only process states where this qubit is 0 */
            int flipped_state = i | qubit_mask;
            
            Complex amp0 = state->amplitudes[i];
            Complex amp1 = state->amplitudes[flipped_state];
            
            state->amplitudes[i] = complex_create(
                cos_half * amp0.real - sin_half * amp1.real,
                cos_half * amp0.imag - sin_half * amp1.imag
            );
            state->amplitudes[flipped_state] = complex_create(
                sin_half * amp0.real + cos_half * amp1.real,
                sin_half * amp0.imag + cos_half * amp1.imag
            );
        }
    }
}

void gate_rotation_z(QuantumState *state, int qubit, double angle) {
    if (!validate_single_qubit_gate(state, qubit)) return;
    
    Complex phase_0 = complex_from_polar(1.0, -angle / 2.0);
    Complex phase_1 = complex_from_polar(1.0, angle / 2.0);
    int qubit_mask = 1 << qubit;
    
    for (int i = 0; i < state->num_states; i++) {
        if (i & qubit_mask) {
            state->amplitudes[i] = complex_multiply(state->amplitudes[i], phase_1);
        } else {
            state->amplitudes[i] = complex_multiply(state->amplitudes[i], phase_0);
        }
    }
}

void gate_cnot(QuantumState *state, int control, int target) {
    if (!validate_two_qubit_gate(state, control, target)) return;
    
    int control_mask = 1 << control;
    int target_mask = 1 << target;
    
    /* Process each basis state */
    for (int i = 0; i < state->num_states; i++) {
        if (i & control_mask) {  /* Control qubit is 1 */
            int partner_state = i ^ target_mask;  /* Flip target qubit */
            
            /* Only swap if we haven't processed this pair yet */
            if (partner_state > i) {
                Complex temp = state->amplitudes[i];
                state->amplitudes[i] = state->amplitudes[partner_state];
                state->amplitudes[partner_state] = temp;
            }
        }
    }
}

void gate_cz(QuantumState *state, int control, int target) {
    if (!validate_two_qubit_gate(state, control, target)) return;
    
    int control_mask = 1 << control;
    int target_mask = 1 << target;
    
    for (int i = 0; i < state->num_states; i++) {
        if ((i & control_mask) && (i & target_mask)) {  /* Both qubits are 1 */
            state->amplitudes[i].real = -state->amplitudes[i].real;
            state->amplitudes[i].imag = -state->amplitudes[i].imag;
        }
    }
}

void gate_swap(QuantumState *state, int qubit1, int qubit2) {
    if (!validate_two_qubit_gate(state, qubit1, qubit2)) return;
    
    int mask1 = 1 << qubit1;
    int mask2 = 1 << qubit2;
    
    for (int i = 0; i < state->num_states; i++) {
        int bit1 = (i & mask1) ? 1 : 0;
        int bit2 = (i & mask2) ? 1 : 0;
        
        if (bit1 != bit2) {  /* Only swap if bits are different */
            int j = i ^ mask1 ^ mask2;  /* Swap the two bits */
            if (j > i) {  /* Only swap once */
                Complex temp = state->amplitudes[i];
                state->amplitudes[i] = state->amplitudes[j];
                state->amplitudes[j] = temp;
            }
        }
    }
}

void gate_identity(QuantumState *state, int qubit) {
    /* Identity gate does nothing - included for completeness */
    (void)state;
    (void)qubit;
}