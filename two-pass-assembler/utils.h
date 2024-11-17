#ifndef _UTILS_H
#define _UTILS_H
#endif

#define MAX_TOKEN_LENGTH 25
#define EMPTY "****"
#define ERROR_VALUE -1
#define NUM_REGISTERS 7

int symbol_search(char *);
int symbol_value(char *);
void insert_symbol_to_SYMTAB(char *, int);
int is_immediate_number(char *);
int get_immediate_value(char *);

int opcode_search(char *);
int opcode_instruction_format(char *);
int opcode_value(char *);