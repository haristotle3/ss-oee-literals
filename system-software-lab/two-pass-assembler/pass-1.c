#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()

#define MAX_TOKEN_LENGTH 25

int passOne(FILE *, FILE *, FILE *, FILE *, FILE *);

int main()
{
    // input files
    FILE *input_file = fopen("input.txt", "r");
    FILE *operation_code_table = fopen("OPTAB.txt", "r");

    // output files
    FILE *intermediate_file = fopen("intermediate.txt", "w");
    FILE *symbol_table = fopen("SYMTAB.txt", "w");
    FILE *length = fopen("program_length.txt", "w");

    // Input file is an assembly program.
    // The program is written in a fixed format with fields
    // LABEL, OPCODE and OPERAND

    int program_length = passOne(input_file, operation_code_table, intermediate_file, symbol_table, length);

    fclose(input_file);
    fclose(operation_code_table);
    fclose(intermediate_file);
    fclose(symbol_table);

    fprintf(length, "%d", program_length);
    return 0;
}

int passOne(FILE *input_file, FILE *opcode_table, FILE *intermediate_file, FILE *symbol_table, FILE *length)
{
    char label[MAX_TOKEN_LENGTH];
    char opcode[MAX_TOKEN_LENGTH];
    char operand[MAX_TOKEN_LENGTH];

    unsigned int START = 0;
    unsigned int LOCCTR = 0;
    // read first line
    fscanf(input_file, "%s\t%s\t%s", label, opcode, operand);

    if (strcmp(opcode, "START") == 0)
    {
        START = atoi(operand);
        LOCCTR = START;

        // First line doesn't require a location.
        fprintf(intermediate_file, "%s\t%s\t%s\n", label, opcode, operand);
        fscanf(input_file, "%s\t%s\t%s", label, opcode, operand);
    }

    while (strcmp(opcode, "END") != 0)
    {
        if (strcmp(label, ".") == 0)
        {
            fscanf(input_file, "%s\t%s\t%s\n", label, opcode, operand);
            continue;
        }

        fprintf(intermediate_file, "%x\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);

        if (strcmp(label, "****") != 0)
        {
            // searching SYMTAB for label.
            FILE *search_symtab = fopen("SYMTAB.txt", "r");
            char search_label[MAX_TOKEN_LENGTH];

            while (fscanf(search_symtab, "%s\t%s", search_label) > 0)
            {
                if (strcmp(search_label, label) == 0)
                {
                    printf("ERROR: %s redefined at %d\n", label, LOCCTR);
                    return -1;
                }
            }
            // symbol not found.
            fprintf(symbol_table, "%s\t%x\n", label, LOCCTR);
            fclose(search_symtab);
        }

        // search OPTAB for opcode.
        FILE *search_optab = fopen("OPTAB.txt", "r");
        char search_opcode[MAX_TOKEN_LENGTH];
        int opcode_found = 0;

        while (fscanf(search_optab, "%s\t%s", search_opcode) > 0)
        {
            if (strcmp(search_opcode, opcode) == 0)
            {
                opcode_found = 1;
                break;
            }
        }

        fclose(search_optab);

        if (opcode_found && opcode[0] != '+')
            LOCCTR += 3;
        else if (opcode_found && opcode[0] == '+')
            LOCCTR += 4;
        else if (opcode[strlen(opcode) - 1] == 'R')
            LOCCTR += 2;
        else if (strcmp(opcode, "WORD") == 0)
            LOCCTR += 3;
        else if (strcmp(opcode, "RESW") == 0)
            LOCCTR += (atoi(operand) * 3);
        else if (strcmp(opcode, "RESB") == 0)
            LOCCTR += (atoi(operand));
        else if (strcmp(opcode, "BYTE") == 0)
        {
            if (operand[0] == 'X')
                LOCCTR++;
            else if (operand[0] == 'C')
                LOCCTR += strlen(operand) - 3; // Subrtact 1 each for the character C, and the two quote marks.
        }
        else
        {
            printf("ERROR: Invalid OPCODE (%s) at %x.\n", opcode, LOCCTR);
            return -1;
        }

        fscanf(input_file, "%s\t%s\t%s", label, opcode, operand);
    }

    int program_length = LOCCTR - START;
    printf("Success!");

    return program_length;
}