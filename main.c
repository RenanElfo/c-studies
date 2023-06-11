/* Imports */
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

/* General Structs */
typedef struct UnsignedIntVector{
    unsigned int *array;
    size_t length;
} UnsignedIntVector;

typedef struct CharVector{
    char *string;
    size_t length;
} CharVector;

/* Specific Structs */
typedef struct Transitions{
    unsigned int *current_state, *next_state;
    char *symbol;
    size_t length;
} Transitions;

typedef struct FinateStateMachine{
    CharVector symbols;
    UnsignedIntVector states, final_states;
    unsigned int initial_state;
    Transitions transitions;
} FinateStateMachine;

/* Defines */
#define INITIAL_SIZE 16
#define SIZE_INCREASE 16
#define MAX_SIZE 1024
#define FILE_NAME "machine.txt"

/* Machine Functions Prototypes */
FinateStateMachine read_machine(char *file_name);
bool run_machine(FinateStateMachine machine, char *string_input);
unsigned int next_state(FinateStateMachine machine,
                        char symbol, unsigned int current_state);
bool is_final_state(FinateStateMachine machine, unsigned int state);
UnsignedIntVector read_final_states(FILE *file_pointer);
UnsignedIntVector unique_states_number(Transitions transitions);
CharVector unique_symbols(Transitions transitions);
void display_machine(FinateStateMachine machine);
void free_machine(FinateStateMachine machine);

/* Transitions Functions Prototypes */
Transitions read_transitions(FILE *file_pointer);
Transitions initialize_transitions();
Transitions increase_transitions_size(Transitions transitions, size_t size);
void free_transitions(Transitions transitions);

/* Main Function */
int main(){
    struct FinateStateMachine machine;
    char *input_string;
    bool result;

    //printf("State transitions obtained in the file.\n");
    machine = read_machine(FILE_NAME);
    //display_machine(machine);

    printf("Input string: ");
    input_string = realloc(NULL, sizeof(*input_string)*MAX_SIZE);
    fgets(input_string, MAX_SIZE, stdin);
    input_string[strcspn(input_string, "\n")] = '\0';
    //printf("Your input string was: ");
    //puts(input_string);

    if ((result = run_machine(machine, input_string))) printf("True\n");
    else printf("False\n");

    //printf("Freeing pointers used\n");
    free(input_string);
    //printf("Freed input string\n");
    free_machine(machine);
    //printf("Freed machine\n");
    exit(EXIT_SUCCESS);
}

/* Machine Functions Declarations */
FinateStateMachine read_machine(char *file_name){
    FILE *file_pointer;
    FinateStateMachine machine;
    char *input_string;

    file_pointer = fopen(file_name, "r");
    fscanf(file_pointer, "%u\n", &machine.initial_state);
    machine.final_states = read_final_states(file_pointer);
    machine.transitions = read_transitions(file_pointer);
    machine.symbols = unique_symbols(machine.transitions);
    machine.states = unique_states_number(machine.transitions);
    fclose(file_pointer);

    return machine;
}

bool run_machine(FinateStateMachine machine, char *input_string){
    unsigned int state = machine.initial_state;
    char symbol;
    size_t index = 0;

    while ((symbol = input_string[index++]) != '\0'){
        state = next_state(machine, symbol, state);
    }

    return is_final_state(machine, state);
}

unsigned int next_state(FinateStateMachine machine,
                        char symbol, unsigned int current_state){
    size_t index = 0;
    bool found = false;

    while (!found && index < machine.transitions.length){
        if (current_state == machine.transitions.current_state[index]
                && symbol == machine.transitions.symbol[index]){
            //printf("%u %c ", current_state, symbol);
            current_state = machine.transitions.next_state[index];
            //printf("%u\n", current_state);
            found = true;
        }
        index++;
    }
    if (!found && index == machine.transitions.length){
        printf("Invalid input string. Valid symbols: %s\n",
               machine.symbols.string);
        exit(EXIT_FAILURE);
    }

    return current_state;
}

bool is_final_state(FinateStateMachine machine, unsigned int state){
    bool result = false;

    for (size_t index = 0; index < machine.final_states.length; index++){
        if (state == machine.final_states.array[index]) result = true;
    }

    return result;
}

UnsignedIntVector read_final_states(FILE *file_pointer){
    UnsignedIntVector final_states;
    int magnitude = 1, state = 0, parser;

    final_states.array = realloc(NULL, sizeof(*final_states.array)*MAX_SIZE);
    final_states.length = 0;
    while ((parser = fgetc(file_pointer)) != '\n'){
        if (parser == EOF) exit(EXIT_FAILURE);
        if (parser == ' ' && magnitude != 1){
            final_states.array[final_states.length++] = state;
            state = 0;
            magnitude = 1;
        }
        else if (parser != ' '){
            state += magnitude*(parser - '0');
            magnitude *= 10;
        }
    }
    if (magnitude != 1) final_states.array[final_states.length++] = state;

    return final_states;
}

UnsignedIntVector unique_states_number(Transitions transitions){
    UnsignedIntVector unique;
    unsigned int state;
    bool is_in;

    unique.array = realloc(
        NULL, sizeof(*unique.array)*(transitions.length)
    );
    unique.length = 0;
    for (size_t s_index = 0; s_index < transitions.length; s_index++){
        state = transitions.current_state[s_index];
        is_in = false;
        for (size_t u_index = 0; u_index < unique.length; u_index++){
            if (unique.array[u_index] == state) is_in = true;
        }
        if (!is_in) unique.array[unique.length++] = state;
    }

    return unique;
}

CharVector unique_symbols(Transitions transitions){
    CharVector unique;
    char symbol;
    bool is_in;

    unique.string = realloc(
        NULL, sizeof(*unique.string)*(transitions.length + 1)
    );
    unique.length = 0;
    for (size_t t_index = 0; t_index < transitions.length; t_index++){
        symbol = transitions.symbol[t_index];
        is_in = false;
        for (size_t u_index = 0; u_index < unique.length; u_index++){
            if (unique.string[u_index] == symbol) is_in = true;
        }
        if (!is_in) unique.string[unique.length++] = symbol;
    }
    unique.string[unique.length] = '\0';

    return unique;
}

void display_machine(FinateStateMachine machine){
    printf("Your machine is:\n");
    printf("    - Symbols: %s\n", machine.symbols);
    printf("    - States:");
    for (size_t index = 0; index < machine.states.length; index++){
        printf(" %u", machine.states.array[index]);
    }
    printf("\n    - Initial state: %u\n", machine.initial_state);
    printf("    - Final states:");
    for (size_t index = 0; index < machine.final_states.length; index++){
        printf(" %u", machine.final_states.array[index]);
    }
    printf("\n    - State transitions:\n");
    for (int index = 0; index < machine.transitions.length; index++){
        printf("        - (Current: %u, Symbol: %c, Next: %u)\n",
               machine.transitions.current_state[index],
               machine.transitions.symbol[index],
               machine.transitions.next_state[index]);
    }
}

void free_machine(FinateStateMachine machine){
    free(machine.final_states.array);
    free(machine.symbols.string);
    free_transitions(machine.transitions);
}

/* Transition Functions Declarations */
Transitions read_transitions(FILE *file_pointer){
    unsigned int current_state, next_state;
    char symbol;
    Transitions transitions = initialize_transitions();
    size_t size = INITIAL_SIZE;

    if (file_pointer == NULL) exit(EXIT_FAILURE);
    while (EOF != fscanf(
            file_pointer, "%u %c %u", &current_state, &symbol, &next_state)){
        transitions.current_state[transitions.length] = current_state;
        transitions.next_state[transitions.length] = next_state;
        transitions.symbol[transitions.length++] = symbol;
        if (transitions.length == size){
            transitions = increase_transitions_size(
                transitions, (size += SIZE_INCREASE)
            );
        }
    }
    transitions.symbol[transitions.length] = '\0';

    return transitions;
}

Transitions initialize_transitions(){
    Transitions transitions;

    transitions.length = 0;
    transitions.current_state = realloc(
        NULL, sizeof(*transitions.current_state)*INITIAL_SIZE
    );
    if (!transitions.current_state) exit(EXIT_FAILURE);
    transitions.next_state = realloc(
        NULL, sizeof(*transitions.next_state)*INITIAL_SIZE
    );
    if (!transitions.next_state) exit(EXIT_FAILURE);
    transitions.symbol = realloc(
        NULL, sizeof(*transitions.symbol)*INITIAL_SIZE
    );
    if (!transitions.symbol) exit(EXIT_FAILURE);

    return transitions;
}

Transitions increase_transitions_size(Transitions transitions, size_t size){
    if (size > MAX_SIZE) exit(EXIT_FAILURE);
    transitions.current_state = realloc(
        transitions.current_state, sizeof(*transitions.current_state)*size
    );
    if (!transitions.current_state) exit(EXIT_FAILURE);
    transitions.next_state = realloc(
        transitions.next_state, sizeof(*transitions.next_state)*size
    );
    if (!transitions.next_state) exit(EXIT_FAILURE);
    transitions.symbol = realloc(
        transitions.symbol, sizeof(*transitions.symbol)*size
    );
    if (!transitions.symbol) exit(EXIT_FAILURE);

    return transitions;
}

void free_transitions(Transitions transitions){
    free(transitions.current_state);
    free(transitions.next_state);
    free(transitions.symbol);
}
