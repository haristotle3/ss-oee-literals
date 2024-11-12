#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 25

int passTwo(FILE *, FILE *, FILE *, FILE *, int);

int main()
{
    // input files
    FILE *input_file = fopen("intermediate.txt", "r");
    FILE *operation_code_table = fopen("OPTAB.txt", "r");
    FILE *symbol_table = fopen("SYMTAB.txt", "r");
    FILE *length = fopen("program_length", "r");

    int code_length;
    fscanf(length, "%d", &code_length);
    printf("%d\n", code_length);
    
    // output files
    FILE *object_program = fopen("object_program.txt", "w");

    passTwo(input_file, operation_code_table, symbol_table, object_program, code_length);

    return 0;
}

int passTwo(FILE *input_file, FILE *operation_code_table, FILE *symbol_table, FILE *object_program, int code_length)
{
    char location[MAX_TOKEN_LENGTH];
    char label[MAX_TOKEN_LENGTH];
    char opcode[MAX_TOKEN_LENGTH];
    char operand[MAX_TOKEN_LENGTH];

    fscanf(input_file, "%s\t%s\t%s", label, opcode, operand);

    printf("%d\n", code_length);
    printf("%x\n", code_length);

    if (strcmp(opcode, "START") == 0)
    {
        fprintf(object_program, "H%-6s%06x%06x\n", label, atoi(operand), code_length);
    }
}