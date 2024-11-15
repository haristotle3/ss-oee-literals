#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int main()
{
    // input files
    FILE *input_file = fopen("intermediate.txt", "r");

    // output files
    FILE *object_program = fopen("object_program.txt", "w");
    FILE *assembly_listing = fopen("program_listing.txt", "w");
    // Input file is an assembly program.
    // The program is written in a fixed format with fields
    // LABEL, OPCODE and OPERAND

    fclose(input_file);
    fclose(object_program);
    fclose(assembly_listing);

    return 0;
}

int passTwo(FILE *input_file, FILE *object_program, FILE *assembly_listing)
{
    char program_name[MAX_TOKEN_LENGTH];
    int start_address;
    int length;

    int location;
    char label[MAX_TOKEN_LENGTH];
    char opcode[MAX_TOKEN_LENGTH];
    char operand[MAX_TOKEN_LENGTH];

    fscanf(input_file, "%s\t%s\t%x", &program_name, opcode, &start_address);

    if (strcmp(opcode, "START") == 0)
        fprintf(assembly_listing, "%s\t%s\t%x\n", program_name, opcode, start_address);

    // Program length in decimal.
    FILE *program_length = fopen("program_length.txt", "r");
    fscanf(program_length, "%d", &length);
    fclose(program_length);

    // Write header record to object program.
    fprintf(object_program, "H%6s%06x%06x", program_name, start_address, length);

    int text_record_length = 0;
    FILE *temp_text_record = fopen("Temp_text_record.txt", "w");
    fprintf(temp_text_record, "%c%06x%02x", 'T', start_address, text_record_length);
    // Later we have to use fseek() and fputc() to replace 0 (text record length)
    // to appropriate value.

    while (fscanf(input_file, "%x\t%s\t%s\t%s", &location, label, opcode, operand) > 0)
    {
        // No comments in intermediate file.
        if (opcode_search(opcode))
        {
            if (strcmp(operand, EMPTY) != 0)
            {
                if (symbol_search(operand))
                {
                }
            }
        }
        else if (strcmp(opcode, "BYTE") || strcmp(opcode, "WORD"))
        {
        }
    }
}
