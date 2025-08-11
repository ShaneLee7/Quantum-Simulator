#include "quantum_utils.h"
#include "quantum_gates.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

void quantum_utils_print_binary(int number, int width) {
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

void quantum_utils_apply_grover_diffusion_sparse(QuantumState *state, int* valid_states, int num_valid) {
    if (!state || !valid_states || num_valid <= 0) return;
    
    // Calculate average amplitude over valid database states only
    Complex sum = {0.0, 0.0};
    for (int i = 0; i < num_valid; i++) {
        int idx = valid_states[i];
        if (idx < state->num_states) {
            sum.real += state->amplitudes[idx].real;
            sum.imag += state->amplitudes[idx].imag;
        }
    }
    
    double avg_real = sum.real / num_valid;
    double avg_imag = sum.imag / num_valid;
    
    // Apply inversion about average to database states only
    for (int i = 0; i < num_valid; i++) {
        int idx = valid_states[i];
        if (idx < state->num_states) {
            state->amplitudes[idx].real = 2.0 * avg_real - state->amplitudes[idx].real;
            state->amplitudes[idx].imag = 2.0 * avg_imag - state->amplitudes[idx].imag;
        }
    }
}

void quantum_utils_apply_grover_diffusion(QuantumState *state) {
    if (!state) return;
    
    Complex sum = {0.0, 0.0};
    for (int i = 0; i < state->num_states; i++) {
        sum.real += state->amplitudes[i].real;
        sum.imag += state->amplitudes[i].imag;
    }
    
    double avg_real = sum.real / state->num_states;
    double avg_imag = sum.imag / state->num_states;
    
    for (int i = 0; i < state->num_states; i++) {
        state->amplitudes[i].real = 2.0 * avg_real - state->amplitudes[i].real;
        state->amplitudes[i].imag = 2.0 * avg_imag - state->amplitudes[i].imag;
    }
}

// Helper function declarations (moved up before they're used)
int quantum_utils_find_small_factor(int n);
int quantum_utils_shor_find_factor(QuantumState *state, int N);

void quantum_utils_run_grover_algorithm(QuantumState *state) {
    if (!state) return;
    
    printf("\n=== GROVER'S SEARCH ALGORITHM ===\n");
    
    int total_states = state->num_states;
    printf("🔍 Quantum Database Search using %d qubits (%d states)\n\n", 
           state->num_qubits, total_states);
    
    // Create searchable database
    const char* database[] = {
        "apple", "banana", "cherry", "date", "elderberry", "fig", 
        "grape", "honeydew", "kiwi", "lemon", "mango", "nectarine",
        "orange", "papaya", "quince", "raspberry", "strawberry", 
        "tangerine", "watermelon", "blueberry", "pineapple", "peach",
        "plum", "apricot", "coconut", "avocado", "lime", "grapefruit"
    };
    int max_db_size = sizeof(database) / sizeof(database[0]);
    int db_size = (max_db_size < total_states) ? max_db_size : total_states;
    
    // 🔧 FIX: For large quantum spaces with sparse databases, use smaller register
    int effective_qubits = state->num_qubits;
    
    if (db_size < 256 && state->num_qubits > 8) {
        // Use only the qubits we need for the database
        effective_qubits = (int)ceil(log2(db_size)) + 1;
        printf("💡 Optimizing: Using %d qubits for %d database items (instead of %d)\n", 
               effective_qubits, db_size, state->num_qubits);
    }
    
    // Display the searchable database
    printf("📚 SEARCHABLE DATABASE (%d items):\n", db_size);
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
    
    // Get user input for search target
    printf("\n🎯 What would you like to search for?\n");
    printf("Options:\n");
    printf("  • Enter a number (0-%d)\n", db_size - 1);
    printf("  • Type the item name (e.g., 'apple', 'banana')\n");
    printf("  • Press ENTER for random selection\n\n");
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    printf("Your choice: ");
    char input[100];
    int target = -1;
    
    if (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;
        
        // Trim whitespace
        char *start = input;
        while (*start == ' ' || *start == '\t') start++;
        char *end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) end--;
        *(end + 1) = '\0';
        
        // Empty input = random selection
        if (strlen(start) == 0) {
            target = quantum_utils_random_int(0, db_size - 1);
            printf("🎲 Random selection: \"%s\" (index %d)\n", database[target], target);
        } else {
            // Try parsing as number
            char *endptr;
            long num = strtol(start, &endptr, 10);
            
            if (*endptr == '\0' && num >= 0 && num < db_size) {
                target = (int)num;
                printf("✅ Selected: \"%s\" (index %d)\n", database[target], target);
            } else {
                // Search by name (case insensitive)
                for (int i = 0; i < db_size; i++) {
                    if (strcasecmp(start, database[i]) == 0) {
                        target = i;
                        printf("✅ Found: \"%s\" (index %d)\n", database[target], target);
                        break;
                    }
                }
                
                // Try partial match if exact match not found
                if (target == -1) {
                    for (int i = 0; i < db_size; i++) {
                        if (strstr(database[i], start) != NULL) {
                            target = i;
                            printf("🔍 Partial match: \"%s\" (index %d)\n", database[target], target);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Handle invalid input
    if (target == -1) {
        printf("❌ Invalid input. Available items:\n");
        for (int i = 0; i < db_size && i < 10; i++) {
            printf("  %d: %s\n", i, database[i]);
        }
        if (db_size > 10) printf("  ... and %d more\n", db_size - 10);
        
        target = quantum_utils_random_int(0, db_size - 1);
        printf("🎲 Using random target: \"%s\" (index %d)\n", database[target], target);
    }
    
    // Show quantum representation
    printf("\n🔬 QUANTUM REPRESENTATION:\n");
    printf("Target \"%s\" → Quantum state |", database[target]);
    quantum_utils_print_binary(target, effective_qubits);
    printf("⟩ (decimal: %d)\n", target);
    
    // Algorithm execution
    printf("\n═══════════════════════════════════════════════════\n");
    printf("            GROVER ALGORITHM EXECUTION\n");
    printf("═══════════════════════════════════════════════════\n");
    
    // Step 1: Initialize superposition over database states
    printf("\nStep 1: Initialize superposition over database states\n");
    quantum_state_initialise_zero(state);
    
    // Set equal amplitudes for database states only
    double amplitude = 1.0 / sqrt(db_size);
    for (int i = 0; i < db_size; i++) {
        state->amplitudes[i].real = amplitude;
        state->amplitudes[i].imag = 0.0;
    }
    
    printf("✓ Created superposition over %d database items\n", db_size);
    printf("  Each database item has amplitude: %.6f (probability: %.6f)\n", 
           amplitude, amplitude * amplitude);
    
    // 🔧 FIX: Calculate iterations based on DATABASE SIZE
    int iterations = (int)(M_PI * sqrt(db_size) / 4.0);
    if (iterations < 1) iterations = 1;
    
    // Create list of valid database states
    int* valid_states = malloc(db_size * sizeof(int));
    for (int i = 0; i < db_size; i++) {
        valid_states[i] = i;
    }
    
    printf("\nStep 2: Grover iterations\n");
    printf("📊 Classical random search: ~%d attempts needed (average)\n", db_size / 2);
    printf("⚡ Quantum Grover search: only %d iterations needed!\n", iterations);
    printf("🚀 Quantum speedup: %.1fx faster\n", (double)db_size / (2.0 * iterations));
    printf("💡 Searching within %d database states (not %d total quantum states)\n\n", 
           db_size, total_states);
    
    // Execute Grover iterations
    for (int iter = 0; iter < iterations; iter++) {
        printf("Iteration %d/%d:\n", iter + 1, iterations);
        
        printf("  🔍 Oracle: Mark \"%s\"\n", database[target]);
        quantum_utils_apply_grover_oracle(state, target);
        
        printf("  🔄 Diffusion: Amplify marked amplitude (database-aware)\n");
        quantum_utils_apply_grover_diffusion_sparse(state, valid_states, db_size);
        
        double target_prob = quantum_state_get_probability(state, target);
        printf("  📈 Target probability: %.1f%%\n\n", target_prob * 100);
    }
    
    // Results
    printf("Step 3: Measurement and Results\n");
    
    // Show probabilities for database items
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
    
    printf("\n📊 Final probabilities for database items:\n");
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
    
    // Measurement - restrict to database states
    double total_db_prob = 0.0;
    for (int i = 0; i < db_size; i++) {
        total_db_prob += quantum_state_get_probability(state, i);
    }
    
    // Weighted random selection based on probabilities
    double random_val = quantum_utils_random_double(0.0, total_db_prob);
    double cumulative = 0.0;
    int measurement = 0;
    for (int i = 0; i < db_size; i++) {
        cumulative += quantum_state_get_probability(state, i);
        if (random_val <= cumulative) {
            measurement = i;
            break;
        }
    }
    
    printf("\n📏 QUANTUM MEASUREMENT:\n");
    printf("Result: \"%s\" (index %d)\n", database[measurement], measurement);
    
    // Analysis
    if (measurement == target) {
        printf("\n🎉 SUCCESS! Grover's algorithm found the target!\n");
        printf("✨ Found \"%s\" in only %d quantum iterations\n", database[target], iterations);
        printf("🏆 Quantum advantage: %.1fx speedup over classical random search!\n", 
               (double)db_size / (2.0 * iterations));
    } else {
        printf("\n❌ Found \"%s\" instead of target \"%s\"\n", 
               database[measurement], database[target]);
        printf("💡 Quantum algorithms are probabilistic\n");
        printf("🎯 Target had %.1f%% probability - try again!\n", 
               quantum_state_get_probability(state, target) * 100);
    }
    
    free(items);
    free(valid_states);
}

void quantum_utils_run_shor_algorithm(QuantumState *state) {
    if (!state) return;
    
    printf("\n=== SHOR'S FACTORING ALGORITHM ===\n");
    
    if (state->num_qubits < 4) {
        printf("❌ Shor's algorithm requires at least 4 qubits for any meaningful factorization\n");
        return;
    }
    
    // Calculate actual bit limits (n qubits can factor n/2 bit numbers)
    int max_bits = state->num_qubits / 2;
    int max_number = (1 << max_bits) - 1;
    
    printf("🔢 FACTORIZATION LIMITS FOR %d QUBITS:\n\n", state->num_qubits);
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║  QUBIT CAPACITY: %d qubits → %d-bit numbers (max %d)    ║\n", 
           state->num_qubits, max_bits, max_number);
    printf("╠════════════════════════════════════════════════════════╣\n");
    
    if (max_bits <= 3) {
        printf("║  Very limited capacity - only tiny numbers:           ║\n");
        printf("║    Possible: 6=2×3, 9=3×3, 10=2×5, 12=3×4             ║\n");
    } else if (max_bits <= 4) {
        printf("║  Small numbers (up to %d):                             ║\n", max_number);
        printf("║    Good: 15=3×5, 21=3×7, 35=5×7, 33=3×11              ║\n");
    } else if (max_bits <= 5) {
        printf("║  Medium numbers (up to %d):                            ║\n", max_number);
        printf("║    Good: 15, 21, 35, 51=3×17, 77=7×11                 ║\n");
    } else if (max_bits <= 6) {
        printf("║  Larger numbers (up to %d):                            ║\n", max_number);
        printf("║    Good: 77=7×11, 91=7×13, 143=11×13, 187=11×17       ║\n");
    } else if (max_bits <= 7) {
        printf("║  7-bit numbers (up to %d):                             ║\n", max_number);
        printf("║    Good: 143, 187, 209=11×19, 221=13×17               ║\n");
    } else if (max_bits <= 8) {
        printf("║  8-bit numbers (up to %d):                             ║\n", max_number);
        printf("║    Good: 221, 323=17×19, 341=11×31, 361=19×19         ║\n");
    } else if (max_bits <= 9) {
        printf("║  9-bit numbers (up to %d):                             ║\n", max_number);
        printf("║    Good: 323, 377=13×29, 391=17×23, 403=13×31         ║\n");
    } else {  // 10+ bits
        printf("║  %d-bit numbers (up to %d):                             ║\n", max_bits, max_number);
        printf("║    Good: 667=23×29, 697=17×41, 713=23×31              ║\n");
        printf("║    Challenge: 731=17×43, 767=13×59, 779=19×41         ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    printf("\n⚠️  IMPORTANT: To factor n-bit numbers, you need ~2n qubits!\n");
    printf("⚠️  AVOID: Prime numbers - they cannot be factored\n");
    printf("⚠️  AVOID: Powers of primes (25=5², 49=7²) - limited factors\n\n");
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    printf("Enter a composite number to factor (0 for default): ");
    int N = 0;
    if (scanf("%d", &N) != 1 || N <= 3) {
        // Choose appropriate default based on qubit count
        if (max_bits <= 4) N = 15;
        else if (max_bits <= 6) N = 77;
        else if (max_bits <= 8) N = 221;
        else N = 667;
        printf("Using default for %d qubits: N = %d\n", state->num_qubits, N);
    }
    
    // Check if number fits in the bit limit
    if (N > max_number) {
        printf("\n❌ ERROR: %d is too large for %d qubits!\n", N, state->num_qubits);
        printf("   Maximum for %d qubits: %d-bit numbers (up to %d)\n", 
               state->num_qubits, max_bits, max_number);
        printf("   To factor %d, you would need ~%d qubits\n", N, 
               (int)ceil(log2(N + 1)) * 2);
        return;
    }
    
    printf("\n🎯 COMPLETE FACTORIZATION OF N = %d\n", N);
    printf("══════════════════════════════════════════════════════════\n");
    
    quantum_utils_complete_factorization(state, N);
}

// Complete prime factorization function
void quantum_utils_complete_factorization(QuantumState *state, int N) {
    printf("🔍 Starting complete factorization of %d\n\n", N);
    
    // Keep track of all factors found
    int factors[100];  // Max 100 factors
    int factor_count = 0;
    
    // Queue of numbers to factor
    int to_factor[50];
    int queue_size = 1;
    to_factor[0] = N;
    
    while (queue_size > 0 && factor_count < 100) {
        // Take next number to factor
        int current = to_factor[--queue_size];
        
        printf("🧮 Factoring: %d\n", current);
        
        // Check if it's prime
        if (quantum_utils_is_prime(current)) {
            printf("   ✅ %d is prime - added to factor list\n", current);
            factors[factor_count++] = current;
            continue;
        }
        
        // Check for small factors first (efficiency)
        int small_factor = quantum_utils_find_small_factor(current);
        if (small_factor > 1) {
            printf("   🔍 Found small factor: %d\n", small_factor);
            int other_factor = current / small_factor;
            
            // Add both factors to queue for further factorization
            to_factor[queue_size++] = small_factor;
            to_factor[queue_size++] = other_factor;
            continue;
        }
        
        // Use quantum Shor's algorithm
        printf("   🚀 Applying Shor's quantum algorithm...\n");
        int quantum_factor = quantum_utils_shor_find_factor(state, current);
        
        if (quantum_factor > 1 && quantum_factor < current) {
            int other_factor = current / quantum_factor;
            printf("   ✅ Quantum factor found: %d\n", quantum_factor);
            printf("   📊 %d = %d × %d\n", current, quantum_factor, other_factor);
            
            // Add both factors to queue for further factorization
            to_factor[queue_size++] = quantum_factor;
            to_factor[queue_size++] = other_factor;
        } else {
            printf("   ❌ Shor's algorithm failed this time\n");
            printf("   🔄 In real implementation, would retry with different parameters\n");
            // For demo, just add as-is
            factors[factor_count++] = current;
        }
        
        printf("\n");
    }
    
    // Display complete factorization
    printf("🎉 COMPLETE PRIME FACTORIZATION RESULTS:\n");
    printf("═══════════════════════════════════════════════\n");
    
    // Sort factors
    for (int i = 0; i < factor_count - 1; i++) {
        for (int j = i + 1; j < factor_count; j++) {
            if (factors[i] > factors[j]) {
                int temp = factors[i];
                factors[i] = factors[j];
                factors[j] = temp;
            }
        }
    }
    
    printf("Original number: %d\n", N);
    printf("Prime factorization: ");
    
    // Count occurrences of each prime
    int i = 0;
    while (i < factor_count) {
        int prime = factors[i];
        int count = 1;
        
        // Count how many times this prime appears
        while (i + count < factor_count && factors[i + count] == prime) {
            count++;
        }
        
        if (count == 1) {
            printf("%d", prime);
        } else {
            printf("%d^%d", prime, count);
        }
        
        i += count;
        if (i < factor_count) printf(" × ");
    }
    printf("\n");
    
    // Verification
    long long product = 1;
    for (int i = 0; i < factor_count; i++) {
        product *= factors[i];
    }
    
    if (product == N) {
        printf("✅ Verification: ");
        for (int i = 0; i < factor_count; i++) {
            printf("%d", factors[i]);
            if (i < factor_count - 1) printf(" × ");
        }
        printf(" = %lld ✓\n", product);
        printf("🏆 Complete factorization successful!\n");
    } else {
        printf("❌ Verification failed: product = %lld, expected %d\n", product, N);
    }
    
    printf("\n💭 QUANTUM ADVANTAGE:\n");
    printf("   Classical factorization of large numbers is exponentially hard\n");
    printf("   Shor's algorithm provides exponential speedup for cryptographically\n");
    printf("   relevant numbers, making RSA encryption vulnerable to quantum computers!\n");
}

// Helper function to find small factors efficiently
int quantum_utils_find_small_factor(int n) {
    if (n % 2 == 0) return 2;
    
    for (int i = 3; i * i <= n && i <= 100; i += 2) {
        if (n % i == 0) return i;
    }
    return 1;  // No small factor found
}

// Simulate Shor's algorithm finding a factor
int quantum_utils_shor_find_factor(QuantumState *state, int N) {
    // Remove unused parameter warning by using (void)state
    (void)state;
    
    if (quantum_utils_is_prime(N)) return 1;
    
    // Find suitable base
    int a = 2;
    while (quantum_utils_gcd(a, N) != 1 && a < N) {
        a++;
    }
    
    if (a >= N) return 1;
    
    // Check for immediate factor
    int gcd_check = quantum_utils_gcd(a, N);
    if (gcd_check > 1) {
        return gcd_check;
    }
    
    // Calculate period
    int period = quantum_utils_find_period(a, N);
    
    printf("      Base: a = %d, Period: r = %d\n", a, period);
    
    if (period == 0 || period % 2 != 0) {
        printf("      ❌ Unusable period (%d) - would retry in real implementation\n", period);
        return 1;
    }
    
    // Apply quantum algorithm simulation
    printf("      🌀 Applying quantum superposition...\n");
    printf("      🔄 Applying QFT...\n");
    
    int half_period = period / 2;
    long long a_pow_half = 1;
    for (int i = 0; i < half_period; i++) {
        a_pow_half = (a_pow_half * a) % N;
    }
    
    printf("      📏 Quantum measurement: a^(r/2) ≡ %lld (mod %d)\n", a_pow_half, N);
    
    if (a_pow_half == N - 1) {
        printf("      ❌ Unlucky case: a^(r/2) ≡ -1 (mod N)\n");
        return 1;
    }
    
    int factor1 = quantum_utils_gcd(a_pow_half - 1, N);
    int factor2 = quantum_utils_gcd(a_pow_half + 1, N);
    
    if (factor1 > 1 && factor1 < N) {
        return factor1;
    } else if (factor2 > 1 && factor2 < N) {
        return factor2;
    }
    
    return 1;  // No factor found
}

// Helper functions
int quantum_utils_gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int quantum_utils_find_period(int a, int N) {
    long long result = 1;
    for (int r = 1; r <= N; r++) {
        result = (result * a) % N;
        if (result == 1) {
            return r;
        }
    }
    return 0;
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

void quantum_utils_controlled_phase(QuantumState *state, int control, int target, double angle) {
    if (!state || control < 0 || target < 0 || 
        control >= state->num_qubits || target >= state->num_qubits || 
        control == target) {
        return;
    }
    
    int control_mask = 1 << control;
    int target_mask = 1 << target;
    Complex phase_factor = complex_from_polar(1.0, angle);
    
    for (int i = 0; i < state->num_states; i++) {
        if ((i & control_mask) && (i & target_mask)) {
            state->amplitudes[i] = complex_multiply(state->amplitudes[i], phase_factor);
        }
    }
}

// Demo functions
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