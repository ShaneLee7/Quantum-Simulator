#include "quantum_utils.h"
#include "quantum_gates.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

void quantum_utils_print_binary(int number, int width) {
    for (int i = width - 1; i >= 0; i--) {
        printf("%d", (number >> i) & 1);
    }
}

void quantum_utils_print_separator(const char* title) {
    printf("\n=================================================\n");
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

// =============================================================================
// COMMON INPUT PARSING HELPERS (REDUCES ~80 LINES OF DUPLICATION)
// =============================================================================

InputParseResult quantum_utils_parse_user_input(const char* prompt, const char** database, int db_size) {
    InputParseResult result = {0};
    result.target = -1;
    result.db_size = db_size;
    result.database = database;
    
    printf("%s", prompt);
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    if (fgets(result.input, sizeof(result.input), stdin)) {
        result.input[strcspn(result.input, "\n")] = 0;
        
        // Trim whitespace
        char *start = result.input;
        while (*start == ' ' || *start == '\t') start++;
        char *end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';
        
        // Empty input = random selection
        if (strlen(start) == 0) {
            result.target = quantum_utils_random_int(0, db_size - 1);
            printf(" Random selection: \"%s\" (index %d)\n", database[result.target], result.target);
            return result;
        }
        
        // Try parsing as number
        char *endptr;
        long num = strtol(start, &endptr, 10);
        
        if (*endptr == '\0' && num >= 0 && num < db_size) {
            result.target = (int)num;
            printf(" Selected: \"%s\" (index %d)\n", database[result.target], result.target);
            return result;
        }
        
        // Search by name (case insensitive)
        for (int i = 0; i < db_size; i++) {
            if (strcasecmp(start, database[i]) == 0) {
                result.target = i;
                printf(" Found: \"%s\" (index %d)\n", database[result.target], result.target);
                return result;
            }
        }
        
        // Try partial match
        for (int i = 0; i < db_size; i++) {
            if (strstr(database[i], start) != NULL) {
                result.target = i;
                printf(" Partial match: \"%s\" (index %d)\n", database[result.target], result.target);
                return result;
            }
        }
    }
    
    // Fallback to random
    result.target = quantum_utils_random_int(0, db_size - 1);
    printf(" Using random target: \"%s\" (index %d)\n", database[result.target], result.target);
    return result;
}

void quantum_utils_display_probabilities(QuantumState *state, const char** database, int db_size, int target) {
    typedef struct { int idx; double prob; } ItemProb;
    ItemProb *items = malloc(db_size * sizeof(ItemProb));
    
    for (int i = 0; i < db_size; i++) {
        items[i] = (ItemProb){i, quantum_state_get_probability(state, i)};
    }
    
    // Sort by probability (descending)
    for (int i = 0; i < db_size - 1; i++) {
        for (int j = i + 1; j < db_size; j++) {
            if (items[j].prob > items[i].prob) {
                ItemProb temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
    
    printf("\n Final probabilities:\n");
    printf("╔═════╤═══════════════╤══════════╗\n");
    printf("║  #  │ Item          │   Prob   ║\n");
    printf("╠═════╪═══════════════╪══════════╣\n");
    
    int items_shown = 0;
    for (int i = 0; i < db_size && items_shown < 10; i++) {
        if (items[i].prob > 0.001 || items[i].idx == target) {
            printf("║ %2d  │ %-13s │  %5.1f%%  ║", 
                   items[i].idx, database[items[i].idx], items[i].prob * 100);
            if (items[i].idx == target) printf(" ← TARGET");
            printf("\n");
            items_shown++;
        }
    }
    
    printf("╚═════╧═══════════════╧══════════╝\n");
    free(items);
}

// =============================================================================
// STATE PREPARATION UTILITIES
// =============================================================================

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

// =============================================================================
// CIRCUIT CREATION UTILITIES
// =============================================================================

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
    
    quantum_circuit_add_pauli_x(circuit, 1);
    quantum_circuit_add_hadamard(circuit, 0);
    quantum_circuit_add_hadamard(circuit, 1);
    
    if (oracle_constant) {
        quantum_circuit_add_pauli_z(circuit, 0);
    }
    
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
    
    for (int i = 0; i < num_qubits; i++) {
        quantum_circuit_add_hadamard(circuit, i);
    }
    
    int total_states = 1 << num_qubits;
    int iterations = (int)(M_PI * sqrt(total_states) / 4.0);
    if (iterations < 1) iterations = 1;
    
    for (int iter = 0; iter < iterations; iter++) {
        quantum_utils_add_grover_oracle(circuit, target, num_qubits);
        quantum_utils_add_grover_diffusion(circuit, num_qubits);
    }
    
    quantum_circuit_add_measure_all(circuit);
    return circuit;
}

// =============================================================================
// GROVER'S ALGORITHM COMPONENTS
// =============================================================================

void quantum_utils_add_grover_oracle(QuantumCircuit *circuit, int target, int num_qubits) {
    if (!circuit) return;
    
    for (int i = 0; i < num_qubits; i++) {
        if (!(target & (1 << i))) {
            quantum_circuit_add_pauli_x(circuit, i);
        }
    }
    
    if (num_qubits == 1) {
        quantum_circuit_add_pauli_z(circuit, 0);
    } else if (num_qubits == 2) {
        quantum_circuit_add_cz(circuit, 0, 1);
    } else {
        for (int i = 0; i < num_qubits - 1; i++) {
            quantum_circuit_add_cnot(circuit, i, num_qubits - 1);
        }
        quantum_circuit_add_pauli_z(circuit, num_qubits - 1);
        for (int i = num_qubits - 2; i >= 0; i--) {
            quantum_circuit_add_cnot(circuit, i, num_qubits - 1);
        }
    }
    
    for (int i = 0; i < num_qubits; i++) {
        if (!(target & (1 << i))) {
            quantum_circuit_add_pauli_x(circuit, i);
        }
    }
}

void quantum_utils_add_grover_diffusion(QuantumCircuit *circuit, int num_qubits) {
    if (!circuit) return;
    
    for (int i = 0; i < num_qubits; i++) {
        quantum_circuit_add_hadamard(circuit, i);
    }
    
    for (int i = 0; i < num_qubits; i++) {
        quantum_circuit_add_pauli_x(circuit, i);
    }
    
    if (num_qubits == 1) {
        quantum_circuit_add_pauli_z(circuit, 0);
    } else if (num_qubits == 2) {
        quantum_circuit_add_cz(circuit, 0, 1);
    } else {
        for (int i = 0; i < num_qubits - 1; i++) {
            quantum_circuit_add_cnot(circuit, i, num_qubits - 1);
        }
        quantum_circuit_add_pauli_z(circuit, num_qubits - 1);
        for (int i = num_qubits - 2; i >= 0; i--) {
            quantum_circuit_add_cnot(circuit, i, num_qubits - 1);
        }
    }
    
    for (int i = 0; i < num_qubits; i++) {
        quantum_circuit_add_pauli_x(circuit, i);
    }
    
    for (int i = 0; i < num_qubits; i++) {
        quantum_circuit_add_hadamard(circuit, i);
    }
}

void quantum_utils_apply_grover_oracle(QuantumState *state, int target) {
    if (!state || target < 0 || target >= state->num_states) return;
    
    state->amplitudes[target].real = -state->amplitudes[target].real;
    state->amplitudes[target].imag = -state->amplitudes[target].imag;
}


void quantum_utils_apply_grover_diffusion(QuantumState *state, int* valid_states, int num_valid) {
    if (!state) return;
    
    Complex sum = {0.0, 0.0};
    int states_to_process;
    
    // If valid_states is NULL, use all states (full diffusion)
    if (valid_states == NULL) {
        states_to_process = state->num_states;
        for (int i = 0; i < state->num_states; i++) {
            sum.real += state->amplitudes[i].real;
            sum.imag += state->amplitudes[i].imag;
        }
    } else {
        // Sparse diffusion - only process valid states
        states_to_process = num_valid;
        for (int i = 0; i < num_valid; i++) {
            int idx = valid_states[i];
            if (idx < state->num_states) {
                sum.real += state->amplitudes[idx].real;
                sum.imag += state->amplitudes[idx].imag;
            }
        }
    }
    
    double avg_real = sum.real / states_to_process;
    double avg_imag = sum.imag / states_to_process;
    
    // Apply inversion about average
    if (valid_states == NULL) {
        // Full diffusion
        for (int i = 0; i < state->num_states; i++) {
            state->amplitudes[i].real = 2.0 * avg_real - state->amplitudes[i].real;
            state->amplitudes[i].imag = 2.0 * avg_imag - state->amplitudes[i].imag;
        }
    } else {
        // Sparse diffusion
        for (int i = 0; i < num_valid; i++) {
            int idx = valid_states[i];
            if (idx < state->num_states) {
                state->amplitudes[idx].real = 2.0 * avg_real - state->amplitudes[idx].real;
                state->amplitudes[idx].imag = 2.0 * avg_imag - state->amplitudes[idx].imag;
            }
        }
    }
}

// 🔧 BACKWARD COMPATIBILITY: Wrapper for old function signature
void quantum_utils_apply_grover_diffusion_full(QuantumState *state) {
    quantum_utils_apply_grover_diffusion(state, NULL, 0);
}

// =============================================================================
// GROVER'S ALGORITHM IMPLEMENTATION
// =============================================================================

void quantum_utils_run_grover_algorithm(QuantumState *state) {
    if (!state) return;
    
    printf("\n=== GROVER'S SEARCH ALGORITHM ===\n");
    printf(" Quantum Database Search using %d qubits (%d states)\n\n", 
           state->num_qubits, state->num_states);
    
    // Create searchable database
    const char* database[] = {
        "apple", "banana", "cherry", "date", "elderberry", "fig", 
        "grape", "honeydew", "kiwi", "lemon", "mango", "nectarine",
        "orange", "papaya", "quince", "raspberry", "strawberry", 
        "tangerine", "watermelon", "blueberry", "pineapple", "peach",
        "plum", "apricot", "coconut", "avocado", "lime", "grapefruit"
    };
    int max_db_size = sizeof(database) / sizeof(database[0]);
    int db_size = (max_db_size < state->num_states) ? max_db_size : state->num_states;
    
    // Display database
    printf(" SEARCHABLE DATABASE (%d items):\n", db_size);
    printf("╔═════╤═══════════════╤═════╤═══════════════╗\n");
    printf("║  #  │ Item          │  #  │ Item          ║\n");
    printf("╠═════╪═══════════════╪═════╪═══════════════╣\n");
    
    for (int i = 0; i < db_size; i += 2) {
        printf("║ %2d  │ %-13s │", i, database[i]);
        if (i + 1 < db_size) {
            printf(" %2d  │ %-13s ║\n", i + 1, database[i + 1]);
        } else {
            printf("     │               ║\n");
        }
    }
    printf("╚═════╧═══════════════╧═════╧═══════════════╝\n");
    
    // Get user input using consolidated parser
    char prompt[200];
    snprintf(prompt, sizeof(prompt), 
        "\n Search options:\n"
        "  • Enter number (0-%d)\n"
        "  • Type item name\n"
        "  • Press ENTER for random\n\n"
        "Your choice: ", db_size - 1);
    
    InputParseResult input = quantum_utils_parse_user_input(prompt, database, db_size);
    int target = input.target;
    
    // Show quantum representation
    int effective_qubits = (db_size < 256 && state->num_qubits > 8) ? 
                          (int)ceil(log2(db_size)) + 1 : state->num_qubits;
    
    printf("\n QUANTUM REPRESENTATION:\n");
    printf("Target \"%s\" → |", database[target]);
    quantum_utils_print_binary(target, effective_qubits);
    printf("⟩ (decimal: %d)\n", target);
    
    // Algorithm execution
    printf("\n═══════════════════════════════════════════════════\n");
    printf("            GROVER ALGORITHM EXECUTION\n");
    printf("═══════════════════════════════════════════════════\n");
    
    // Initialize superposition over database states
    printf("\nStep 1: Initialize superposition over database states\n");
    quantum_state_initialise_zero(state);
    
    double amplitude = 1.0 / sqrt(db_size);
    for (int i = 0; i < db_size; i++) {
        state->amplitudes[i].real = amplitude;
        state->amplitudes[i].imag = 0.0;
    }
    
    printf("✓ Created superposition over %d database items\n", db_size);
    
    // Calculate iterations and execute
    int iterations = (int)(M_PI * sqrt(db_size) / 4.0);
    if (iterations < 1) iterations = 1;
    
    int* valid_states = malloc(db_size * sizeof(int));
    for (int i = 0; i < db_size; i++) {
        valid_states[i] = i;
    }
    
    printf("\nStep 2: Grover iterations (%d needed)\n", iterations);
           (double)db_size / (2.0 * iterations));
    
    for (int iter = 0; iter < iterations; iter++) {
        printf("Iteration %d/%d:\n", iter + 1, iterations);
        quantum_utils_apply_grover_oracle(state, target);
        quantum_utils_apply_grover_diffusion(state, valid_states, db_size);
        printf("  Target probability: %.1f%%\n\n", 
               quantum_state_get_probability(state, target) * 100);
    }
    
    // Results
    printf("Step 3: Measurement and Results\n");
    quantum_utils_display_probabilities(state, database, db_size, target);
    
    // Measurement
    double total_prob = 0.0;
    for (int i = 0; i < db_size; i++) {
        total_prob += quantum_state_get_probability(state, i);
    }
    
    double random_val = quantum_utils_random_double(0.0, total_prob);
    double cumulative = 0.0;
    int measurement = 0;
    for (int i = 0; i < db_size; i++) {
        cumulative += quantum_state_get_probability(state, i);
        if (random_val <= cumulative) {
            measurement = i;
            break;
        }
    }
    
    printf("\n MEASUREMENT: \"%s\" (index %d)\n", database[measurement], measurement);
    
    if (measurement == target) {
        printf("\n SUCCESS! Found target in %d iterations!\n", iterations);
    } else {
        printf("\n Found different item - quantum algorithms are probabilistic.\n");
    }
    
    free(valid_states);
}

// =============================================================================
// MATH HELPERS FOR SHOR'S ALGORITHM
// =============================================================================

int quantum_utils_gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int quantum_utils_is_prime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int quantum_utils_find_period(int a, int N) {
    long long result = 1;
    for (int r = 1; r <= N; r++) {
        result = (result * a) % N;
        if (result == 1) return r;
    }
    return 0;
}

int quantum_utils_find_small_factor(int n) {
    if (n % 2 == 0) return 2;
    for (int i = 3; i * i <= n && i <= 100; i += 2) {
        if (n % i == 0) return i;
    }
    return 1;
}

int quantum_utils_shor_find_factor(QuantumState *state, int N) {
    (void)state;  // Unused in simulation
    
    if (quantum_utils_is_prime(N)) return 1;
    
    int a = 2;
    while (quantum_utils_gcd(a, N) != 1 && a < N) a++;
    if (a >= N) return 1;
    
    int gcd_check = quantum_utils_gcd(a, N);
    if (gcd_check > 1) return gcd_check;
    
    int period = quantum_utils_find_period(a, N);
    printf("      Base: a=%d, Period: r=%d\n", a, period);
    
    if (period == 0 || period % 2 != 0) {
        printf("       Unusable period\n");
        return 1;
    }
    
    printf("       Quantum simulation...\n");
    
    int half_period = period / 2;
    long long a_pow_half = 1;
    for (int i = 0; i < half_period; i++) {
        a_pow_half = (a_pow_half * a) % N;
    }
    
    if (a_pow_half == N - 1) {
        printf("       Unlucky case\n");
        return 1;
    }
    
    int factor1 = quantum_utils_gcd(a_pow_half - 1, N);
    int factor2 = quantum_utils_gcd(a_pow_half + 1, N);
    
    if (factor1 > 1 && factor1 < N) return factor1;
    if (factor2 > 1 && factor2 < N) return factor2;
    return 1;
}

// =============================================================================
// SHOR'S ALGORITHM IMPLEMENTATION
// =============================================================================

void quantum_utils_complete_factorization(QuantumState *state, int N) {
    printf(" Complete factorization of %d\n\n", N);
    
    int factors[100], factor_count = 0;
    int to_factor[50], queue_size = 1;
    to_factor[0] = N;
    
    while (queue_size > 0 && factor_count < 100) {
        int current = to_factor[--queue_size];
        printf(" Factoring: %d\n", current);
        
        if (quantum_utils_is_prime(current)) {
            printf("    Prime factor: %d\n", current);
            factors[factor_count++] = current;
            continue;
        }
        
        int small_factor = quantum_utils_find_small_factor(current);
        if (small_factor > 1) {
            printf("    Small factor: %d\n", small_factor);
            to_factor[queue_size++] = small_factor;
            to_factor[queue_size++] = current / small_factor;
            continue;
        }
        
        printf("    Quantum Shor's algorithm...\n");
        int quantum_factor = quantum_utils_shor_find_factor(state, current);
        
        if (quantum_factor > 1 && quantum_factor < current) {
            printf("    Quantum factor: %d\n", quantum_factor);
            to_factor[queue_size++] = quantum_factor;
            to_factor[queue_size++] = current / quantum_factor;
        } else {
            factors[factor_count++] = current;
        }
        printf("\n");
    }
    
    // Sort and display results
    for (int i = 0; i < factor_count - 1; i++) {
        for (int j = i + 1; j < factor_count; j++) {
            if (factors[i] > factors[j]) {
                int temp = factors[i];
                factors[i] = factors[j];
                factors[j] = temp;
            }
        }
    }
    
    printf(" FACTORIZATION RESULTS:\n");
    printf("Original: %d\n", N);
    printf("Factors: ");
    
    int i = 0;
    while (i < factor_count) {
        int prime = factors[i], count = 1;
        while (i + count < factor_count && factors[i + count] == prime) count++;
        
        if (count == 1) printf("%d", prime);
        else printf("%d^%d", prime, count);
        
        i += count;
        if (i < factor_count) printf(" × ");
    }
    printf("\n");
}

void quantum_utils_run_shor_algorithm(QuantumState *state) {
    if (!state) return;
    
    printf("\n=== SHOR'S FACTORING ALGORITHM ===\n");
    
    if (state->num_qubits < 4) {
        printf(" Need at least 4 qubits\n");
        return;
    }
    
    int max_bits = state->num_qubits / 2;
    int max_number = (1 << max_bits) - 1;
    
    printf(" CAPACITY: %d qubits → %d-bit numbers (max %d)\n\n", 
           state->num_qubits, max_bits, max_number);
    
    // Clear input buffer and get number
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    printf("Enter number to factor (0 for default): ");
    int N = 0;
    if (scanf("%d", &N) != 1 || N <= 3) {
        N = (max_bits <= 4) ? 15 : (max_bits <= 6) ? 77 : 
            (max_bits <= 8) ? 221 : 667;
        printf("Using default: N = %d\n", N);
    }
    
    if (N > max_number) {
        printf(" %d too large for %d qubits (max %d)\n", N, state->num_qubits, max_number);
        return;
    }
    
    quantum_utils_complete_factorization(state, N);
}

// =============================================================================
// QUANTUM FOURIER TRANSFORM
// =============================================================================

void quantum_utils_controlled_phase(QuantumState *state, int control, int target, double angle) {
    if (!state || control < 0 || target < 0 || 
        control >= state->num_qubits || target >= state->num_qubits || 
        control == target) return;
    
    int control_mask = 1 << control;
    int target_mask = 1 << target;
    Complex phase_factor = complex_from_polar(1.0, angle);
    
    for (int i = 0; i < state->num_states; i++) {
        if ((i & control_mask) && (i & target_mask)) {
            state->amplitudes[i] = complex_multiply(state->amplitudes[i], phase_factor);
        }
    }
}

void quantum_utils_simplified_qft(QuantumState *state) {
    if (!state) return;
    
    int n = state->num_qubits;
    
    for (int i = 0; i < n; i++) {
        gate_hadamard(state, i);
        for (int j = i + 1; j < n; j++) {
            double angle = M_PI / (1 << (j - i));
            quantum_utils_controlled_phase(state, j, i, angle);
        }
    }
    
    for (int i = 0; i < n / 2; i++) {
        gate_swap(state, i, n - 1 - i);
    }
}

// =============================================================================
// DEMO FUNCTIONS (TEMPLATED TO REDUCE REPETITION)
// =============================================================================

void quantum_utils_run_demo(const char* title, int num_qubits, DemoOperation operation) {
    quantum_utils_print_separator(title);
    
    QuantumState *state = quantum_state_create(num_qubits);
    if (!state) return;
    
    operation(state);
    quantum_state_destroy(state);
}

// Demo operation implementations
void demo_basic_gates_ops(QuantumState *state) {
    printf("Starting with |0⟩:\n");
    quantum_state_initialise_zero(state);
    quantum_state_print(state);
    
    printf("\nPauli-X (bit flip):\n");
    gate_pauli_x(state, 0);
    quantum_state_print(state);
    
    printf("\nHadamard (superposition):\n");
    gate_hadamard(state, 0);
    quantum_state_print(state);
    
    printf("\nProbabilities:\n");
    quantum_state_print_probabilities(state);
    
    printf("\nPauli-Z (phase flip):\n");
    gate_pauli_z(state, 0);
    quantum_state_print(state);
}

void demo_bell_states_ops(QuantumState *state) {
    printf("Creating Bell state |Φ+⟩ = (|00⟩ + |11⟩)/√2:\n");
    quantum_utils_create_bell_state(state, 0, 1);
    quantum_state_print(state);
    
    printf("\nProbabilities:\n");
    quantum_state_print_probabilities(state);
    
    printf("\nMeasurements:\n");
    for (int i = 0; i < 5; i++) {
        QuantumState *copy = quantum_state_copy(state);
        int result = quantum_state_measure_all(copy);
        printf("Measurement %d: |", i + 1);
        quantum_utils_print_binary(result, 2);
        printf("⟩\n");
        quantum_state_destroy(copy);
    }
}

void demo_superposition_ops(QuantumState *state) {
    printf("Equal superposition of all 3-qubit states:\n");
    quantum_state_initialise_equal_superposition(state);
    quantum_state_print_probabilities(state);
    
    printf("\nHadamard gates from |000⟩:\n");
    quantum_state_initialise_zero(state);
    for (int i = 0; i < 3; i++) {
        gate_hadamard(state, i);
    }
    quantum_state_print_probabilities(state);
}

void demo_entanglement_ops(QuantumState *state) {
    printf("GHZ state |GHZ⟩ = (|000⟩ + |111⟩)/√2:\n");
    quantum_utils_create_ghz_state(state);
    quantum_state_print(state);
    
    printf("\nCorrelated measurements:\n");
    for (int trial = 0; trial < 3; trial++) {
        QuantumState *copy = quantum_state_copy(state);
        printf("Trial %d:\n", trial + 1);
        
        for (int qubit = 0; qubit < 3; qubit++) {
            int result = quantum_state_measure_qubit(copy, qubit);
            printf("  Qubit %d: %d\n", qubit, result);
        }
        quantum_state_destroy(copy);
    }
}

// Public demo functions using template
void quantum_utils_demo_basic_gates(void) {
    quantum_utils_run_demo("BASIC QUANTUM GATES DEMONSTRATION", 1, demo_basic_gates_ops);
}

void quantum_utils_demo_bell_states(void) {
    quantum_utils_run_demo("BELL STATES DEMONSTRATION", 2, demo_bell_states_ops);
}

void quantum_utils_demo_superposition(void) {
    quantum_utils_run_demo("SUPERPOSITION DEMONSTRATION", 3, demo_superposition_ops);
}

void quantum_utils_demo_entanglement(void) {
    quantum_utils_run_demo("ENTANGLEMENT DEMONSTRATION", 3, demo_entanglement_ops);
}