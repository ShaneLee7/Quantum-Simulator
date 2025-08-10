#include "quantum_circuit.h"
#include "quantum_gates.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

QuantumCircuit* quantum_circuit_create(int num_qubits, const char* description) {
    if (num_qubits < 1 || num_qubits > MAX_QUBITS) {
        fprintf(stderr, "Error: Number of qubits must be between 1 and %d\n", MAX_QUBITS);
        return NULL;
    }
    
    QuantumCircuit *circuit = malloc(sizeof(QuantumCircuit));
    if (!circuit) {
        fprintf(stderr, "Error: Failed to allocate memory for quantum circuit\n");
        return NULL;
    }
    
    circuit->num_qubits = num_qubits;
    circuit->num_gates = 0;
    
    if (description) {
        strncpy(circuit->description, description, sizeof(circuit->description) - 1);
        circuit->description[sizeof(circuit->description) - 1] = '\0';
    } else {
        strcpy(circuit->description, "Quantum Circuit");
    }
    
    return circuit;
}

void quantum_circuit_destroy(QuantumCircuit *circuit) {
    if (circuit) {
        free(circuit);
    }
}

int quantum_circuit_add_gate(QuantumCircuit *circuit, GateType type, int qubit1, int qubit2, double parameter) {
    if (!circuit) {
        fprintf(stderr, "Error: Null circuit\n");
        return 0;
    }
    
    if (circuit->num_gates >= MAX_GATES) {
        fprintf(stderr, "Error: Circuit has reached maximum number of gates (%d)\n", MAX_GATES);
        return 0;
    }
    
    if (qubit1 < 0 || qubit1 >= circuit->num_qubits) {
        fprintf(stderr, "Error: Qubit %d out of range [0, %d)\n", qubit1, circuit->num_qubits);
        return 0;
    }
    
    if (qubit2 != -1 && (qubit2 < 0 || qubit2 >= circuit->num_qubits)) {
        fprintf(stderr, "Error: Qubit %d out of range [0, %d)\n", qubit2, circuit->num_qubits);
        return 0;
    }
    
    QuantumGate *gate = &circuit->gates[circuit->num_gates];
    gate->type = type;
    gate->qubit1 = qubit1;
    gate->qubit2 = qubit2;
    gate->parameter = parameter;
    
    circuit->num_gates++;
    return 1;
}

int quantum_circuit_add_pauli_x(QuantumCircuit *circuit, int qubit) {
    return quantum_circuit_add_gate(circuit, GATE_PAULI_X, qubit, -1, 0.0);
}

int quantum_circuit_add_pauli_y(QuantumCircuit *circuit, int qubit) {
    return quantum_circuit_add_gate(circuit, GATE_PAULI_Y, qubit, -1, 0.0);
}

int quantum_circuit_add_pauli_z(QuantumCircuit *circuit, int qubit) {
    return quantum_circuit_add_gate(circuit, GATE_PAULI_Z, qubit, -1, 0.0);
}

int quantum_circuit_add_hadamard(QuantumCircuit *circuit, int qubit) {
    return quantum_circuit_add_gate(circuit, GATE_HADAMARD, qubit, -1, 0.0);
}

int quantum_circuit_add_phase(QuantumCircuit *circuit, int qubit, double phase) {
    return quantum_circuit_add_gate(circuit, GATE_PHASE, qubit, -1, phase);
}

int quantum_circuit_add_rotation_x(QuantumCircuit *circuit, int qubit, double angle) {
    return quantum_circuit_add_gate(circuit, GATE_ROTATION_X, qubit, -1, angle);
}

int quantum_circuit_add_rotation_y(QuantumCircuit *circuit, int qubit, double angle) {
    return quantum_circuit_add_gate(circuit, GATE_ROTATION_Y, qubit, -1, angle);
}

int quantum_circuit_add_rotation_z(QuantumCircuit *circuit, int qubit, double angle) {
    return quantum_circuit_add_gate(circuit, GATE_ROTATION_Z, qubit, -1, angle);
}

int quantum_circuit_add_cnot(QuantumCircuit *circuit, int control, int target) {
    if (control == target) {
        fprintf(stderr, "Error: Control and target qubits cannot be the same\n");
        return 0;
    }
    return quantum_circuit_add_gate(circuit, GATE_CNOT, control, target, 0.0);
}

int quantum_circuit_add_cz(QuantumCircuit *circuit, int control, int target) {
    if (control == target) {
        fprintf(stderr, "Error: Control and target qubits cannot be the same\n");
        return 0;
    }
    return quantum_circuit_add_gate(circuit, GATE_CZ, control, target, 0.0);
}

int quantum_circuit_add_swap(QuantumCircuit *circuit, int qubit1, int qubit2) {
    if (qubit1 == qubit2) {
        fprintf(stderr, "Error: Cannot swap a qubit with itself\n");
        return 0;
    }
    return quantum_circuit_add_gate(circuit, GATE_SWAP, qubit1, qubit2, 0.0);
}

int quantum_circuit_add_measure(QuantumCircuit *circuit, int qubit) {
    return quantum_circuit_add_gate(circuit, GATE_MEASURE, qubit, -1, 0.0);
}

int quantum_circuit_add_measure_all(QuantumCircuit *circuit) {
    return quantum_circuit_add_gate(circuit, GATE_MEASURE_ALL, 0, -1, 0.0);
}

int quantum_circuit_execute(const QuantumCircuit *circuit, QuantumState *state) {
    if (!circuit || !state) {
        fprintf(stderr, "Error: Null circuit or state\n");
        return 0;
    }
    
    if (circuit->num_qubits != state->num_qubits) {
        fprintf(stderr, "Error: Circuit and state have different numbers of qubits\n");
        return 0;
    }
    
    printf("Executing circuit: %s\n", circuit->description);
    
    for (int i = 0; i < circuit->num_gates; i++) {
        const QuantumGate *gate = &circuit->gates[i];
        
        switch (gate->type) {
            case GATE_PAULI_X:
                gate_pauli_x(state, gate->qubit1);
                break;
            case GATE_PAULI_Y:
                gate_pauli_y(state, gate->qubit1);
                break;
            case GATE_PAULI_Z:
                gate_pauli_z(state, gate->qubit1);
                break;
            case GATE_HADAMARD:
                gate_hadamard(state, gate->qubit1);
                break;
            case GATE_PHASE:
                gate_phase(state, gate->qubit1, gate->parameter);
                break;
            case GATE_ROTATION_X:
                gate_rotation_x(state, gate->qubit1, gate->parameter);
                break;
            case GATE_ROTATION_Y:
                gate_rotation_y(state, gate->qubit1, gate->parameter);
                break;
            case GATE_ROTATION_Z:
                gate_rotation_z(state, gate->qubit1, gate->parameter);
                break;
            case GATE_CNOT:
                gate_cnot(state, gate->qubit1, gate->qubit2);
                break;
            case GATE_CZ:
                gate_cz(state, gate->qubit1, gate->qubit2);
                break;
            case GATE_SWAP:
                gate_swap(state, gate->qubit1, gate->qubit2);
                break;
            case GATE_MEASURE:
                {
                    int result = quantum_state_measure_qubit(state, gate->qubit1);
                    printf("Measured qubit %d: %d\n", gate->qubit1, result);
                }
                break;
            case GATE_MEASURE_ALL:
                {
                    int result = quantum_state_measure_all(state);
                    printf("Measured all qubits: %d (binary: ", result);
                    for (int j = state->num_qubits - 1; j >= 0; j--) {
                        printf("%d", (result >> j) & 1);
                    }
                    printf(")\n");
                }
                break;
            default:
                fprintf(stderr, "Error: Unknown gate type\n");
                return 0;
        }
    }
    
    return 1;
}

const char* gate_type_to_string(GateType type) {
    switch (type) {
        case GATE_PAULI_X: return "X";
        case GATE_PAULI_Y: return "Y";
        case GATE_PAULI_Z: return "Z";
        case GATE_HADAMARD: return "H";
        case GATE_PHASE: return "P";
        case GATE_ROTATION_X: return "RX";
        case GATE_ROTATION_Y: return "RY";
        case GATE_ROTATION_Z: return "RZ";
        case GATE_CNOT: return "CNOT";
        case GATE_CZ: return "CZ";
        case GATE_SWAP: return "SWAP";
        case GATE_MEASURE: return "M";
        case GATE_MEASURE_ALL: return "M_ALL";
        default: return "UNKNOWN";
    }
}

void quantum_circuit_print(const QuantumCircuit *circuit) {
    if (!circuit) return;
    
    printf("\n=== %s ===\n", circuit->description);
    printf("Qubits: %d, Gates: %d\n\n", circuit->num_qubits, circuit->num_gates);
    
    for (int i = 0; i < circuit->num_gates; i++) {
        const QuantumGate *gate = &circuit->gates[i];
        printf("Gate %d: %s", i + 1, gate_type_to_string(gate->type));
        
        if (gate->qubit2 == -1) {
            printf(" on qubit %d", gate->qubit1);
        } else {
            printf(" on qubits %d,%d", gate->qubit1, gate->qubit2);
        }
        
        if (gate->parameter != 0.0) {
            printf(" (parameter: %.4f)", gate->parameter);
        }
        
        printf("\n");
    }
    printf("\n");
}

void quantum_circuit_clear(QuantumCircuit *circuit) {
    if (circuit) {
        circuit->num_gates = 0;
    }
}