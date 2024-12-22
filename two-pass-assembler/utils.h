#ifndef _UTILS_H
#define _UTILS_H
#endif

#define MEMORY_SIZE 16834
#define MAX_TOKEN_LENGTH 25
#define MAX_BUF 256
#define EMPTY "____"
#define LITERAL_POOL "*"
#define ERROR_VALUE -1
#define NUM_REGISTERS 7

typedef struct
{
    char symbol[MAX_TOKEN_LENGTH];
    long int value; // is the object code of the symbol
    int length;     // in bytes
    int address;
} littab_element;

typedef struct
{
    littab_element table[MEMORY_SIZE];
    int current_size;
    int unassigned_index;  // index of the first literal which is not assigned an address.
    int not_printed_index; // index of the first literal which has not been printed to intermediate.txt
} littab;

int symbol_search(char *);
int symbol_value(char *);
void insert_symbol_to_SYMTAB(char *, int);
int is_immediate_number(char *);
int is_valid_constant(char *); // undefined

int get_immediate_value(char *);
void get_literal_value(char *, char *);
unsigned long long int get_string_literal_hex(char *);
int get_object_code_length(unsigned long long int);

int opcode_search(char *);
int opcode_instruction_format(char *);
int opcode_value(char *);