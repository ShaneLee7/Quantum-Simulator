#ifndef QUANTUM_GATES_H
#define QUANTUM_GATES_H

#include "quantum_state.h"

/* Single qubit gates */
void gate_pauli_x(QuantumState *state, int qubit);
void gate_pauli_y(QuantumState *state, int qubit);
void gate_pauli_z(QuantumState *state, int qubit);
void gate_hadamard(QuantumState *state, int qubit);
void gate_phase(QuantumState *state, int qubit, double phase);
void gate_rotation_x(QuantumState *state, int qubit, double angle);
void gate_rotation_y(QuantumState *state, int qubit, double angle);
void gate_rotation_z(QuantumState *state, int qubit, double angle);

/* Two qubit gates */
void gate_cnot(QuantumState *state, int control, int target);
void gate_cz(QuantumState *state, int control, int target);
void gate_swap(QuantumState *state, int qubit1, int qubit2);

/* Utility gates */
void gate_identity(QuantumState *state, int qubit);

/* Gate validation */
int validate_single_qubit_gate(const QuantumState *state, int qubit);
int validate_two_qubit_gate(const QuantumState *state, int qubit1, int qubit2);

#endif