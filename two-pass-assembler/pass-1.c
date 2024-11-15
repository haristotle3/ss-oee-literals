#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()
#include "utils.h"

int passOne(FILE *, FILE *);
void insert_symbol_to_SYMTAB(char *, int);

int main()
{
    // input files
    FILE *input_file = fopen("input.txt", "r");

    // output files
    FILE *intermediate_file = fopen("intermediate.txt", "w");
    FILE *length = fopen("program_length.txt", "w");

    // Input file is an assembly program.
    // The program is written in a fixed format with fields
    // LABEL, OPCODE and OPERAND

    int program_length = passOne(input_file, intermediate_file);

    fclose(input_file);
    fclose(intermediate_file);

    // Program length is in decimal.
    fprintf(length, "%d", program_length);
    fclose(length);
    return 0;
}

int passOne(FILE *input_file, FILE *intermediate_file)
{
    char label[MAX_TOKEN_LENGTH];
    char opcode[MAX_TOKEN_LENGTH];
    char operand[MAX_TOKEN_LENGTH];

    unsigned int START = 0;
    unsigned int LOCCTR = 0;
    unsigned int start_address;

    // read first line
    fscanf(input_file, "%s\t%s\t%x", label, opcode, &start_address);

    if (strcmp(opcode, "START") == 0)
    {
        START = start_address;
        LOCCTR = START;

        // First line doesn't require a location.
        fprintf(intermediate_file, "%s\t%s\t%x\n", label, opcode, start_address);

        // read next input line
        fscanf(input_file, "%s\t%s\t%s", label, opcode, operand);
    }

    while (strcmp(opcode, "END") != 0)
    {
        // if comment line, then go to next line
        if (strcmp(label, ".") == 0)
        {
            fscanf(input_file, "%s\t%s\t%s\n", label, opcode, operand);
            continue;
        }

        fprintf(intermediate_file, "%x\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);

        // If there is a symbol in the LABEL field
        if (strcmp(label, EMPTY) != 0)
        {
            if (!symbol_search(label))
                insert_symbol_to_SYMTAB(label, LOCCTR);
            else
            {
                printf("ERROR: %s redefined at %x\n", label, LOCCTR);
                return -1;
            }
        }

        // search OPTAB for opcode.
        int opcode_found = opcode_search(opcode);

        if (opcode_found && opcode[0] != '+')
            LOCCTR += 3;
        else if (opcode_found && opcode[0] == '+')
            LOCCTR += 4;
        else if (opcode_found && opcode[strlen(opcode) - 1] == 'R')
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

        // Read next line
        fscanf(input_file, "%s\t%s\t%s", label, opcode, operand);
    }

    int program_length = LOCCTR - START;
    printf("Success!");

    return program_length;
}