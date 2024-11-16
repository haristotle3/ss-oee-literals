#ifndef _UTILS_H
#define _UTILS_H
#endif

#define MAX_TOKEN_LENGTH 25
#define EMPTY "****"
#define ERROR_VALUE -1
#define NUM_REGISTERS 7

int symbol_search(char *); 
int symbol_value(char *);
int opcode_search(char *);
int opcode_instruction_format(char *);
int opcode_value(char *);