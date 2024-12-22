#ifndef _UTILS_H
#define _UTILS_H
#endif

#define MEMORY_SIZE 16834
#define MAX_TOKEN_LENGTH 25
#define EMPTY "____"
#define ERROR_VALUE -1
#define NUM_REGISTERS 7

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