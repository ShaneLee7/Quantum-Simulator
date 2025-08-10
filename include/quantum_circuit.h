#ifndef QUANTUM_CIRCUIT_H
#define QUANTUM_CIRCUIT_H

#include "quantum_state.h"

#define MAX_GATES 1000

typedef enum {
    GATE_PAULI_X,
    GATE_PAULI_Y,
    GATE_PAULI_Z,
    GATE_HADAMARD,
    GATE_PHASE,
    GATE_ROTATION_X,
    GATE_ROTATION_Y,
    GATE_ROTATION_Z,
    GATE_CNOT,
    GATE_CZ,
    GATE_SWAP,
    GATE_MEASURE,
    GATE_MEASURE_ALL
} GateType;

typedef struct {
    GateType type;
    int qubit1;
    int qubit2;  /* For two-qubit gates, -1 for single-qubit gates */
    double parameter;  /* For parameterised gates */
} QuantumGate;

typedef struct {
    int num_qubits;
    int num_gates;
    QuantumGate gates[MAX_GATES];
    char description[256];
} QuantumCircuit;

/* Circuit management */
QuantumCircuit* quantum_circuit_create(int num_qubits, const char* description);
void quantum_circuit_destroy(QuantumCircuit *circuit);

/* Gate addition */
int quantum_circuit_add_gate(QuantumCircuit *circuit, GateType type, int qubit1, int qubit2, double parameter);
int quantum_circuit_add_pauli_x(QuantumCircuit *circuit, int qubit);
int quantum_circuit_add_pauli_y(QuantumCircuit *circuit, int qubit);
int quantum_circuit_add_pauli_z(QuantumCircuit *circuit, int qubit);
int quantum_circuit_add_hadamard(QuantumCircuit *circuit, int qubit);
int quantum_circuit_add_phase(QuantumCircuit *circuit, int qubit, double phase);
int quantum_circuit_add_rotation_x(QuantumCircuit *circuit, int qubit, double angle);
int quantum_circuit_add_rotation_y(QuantumCircuit *circuit, int qubit, double angle);
int quantum_circuit_add_rotation_z(QuantumCircuit *circuit, int qubit, double angle);
int quantum_circuit_add_cnot(QuantumCircuit *circuit, int control, int target);
int quantum_circuit_add_cz(QuantumCircuit *circuit, int control, int target);
int quantum_circuit_add_swap(QuantumCircuit *circuit, int qubit1, int qubit2);
int quantum_circuit_add_measure(QuantumCircuit *circuit, int qubit);
int quantum_circuit_add_measure_all(QuantumCircuit *circuit);

/* Circuit execution */
int quantum_circuit_execute(const QuantumCircuit *circuit, QuantumState *state);

/* Circuit utilities */
void quantum_circuit_print(const QuantumCircuit *circuit);
void quantum_circuit_clear(QuantumCircuit *circuit);

#endif