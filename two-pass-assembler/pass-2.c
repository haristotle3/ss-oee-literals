#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int assemble_instruction(opcode, operand, assembled_address);
void get_literal_value(operand_without_extraneous, operand);
unsigned int get_string_literal_hex(operand);

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
    else
    {
        printf("ERROR: Assembler expects START opcode in line 1.\n");
        return ERROR_VALUE;
    }

    // Program length in decimal.
    FILE *program_length = fopen("program_length.txt", "r");
    fscanf(program_length, "%d", &length);
    fclose(program_length);

    // Write header record to object program.
    fprintf(object_program, "H%6s%06x%06x\n", program_name, start_address, length);

    int text_record_length = 0;
    FILE *temp_text_record = fopen("Temp_text_record.txt", "w");
    fprintf(temp_text_record, "%c%06x%02x", 'T', start_address, text_record_length);
    // Later we have to use fseek() and fputc() to replace 0 (text record length)
    // to appropriate value.

    unsigned long int assembled_object_code = 0;
    int text_record_length = 1 + 6 + 2;
    int text_record_start_address = start_address;

    while (fscanf(input_file, "%x\t%s\t%s\t%s", &location, label, opcode, operand) > 0)
    {
        // No comments in intermediate file.
        if (opcode_search(opcode))
        {
            int assembled_address;
            if (strcmp(operand, EMPTY) != 0)
            {
                if (symbol_search(operand))
                    assembled_address = symbol_value(operand);
                else
                {
                    printf("ERROR: %s doesn't exist in SYMTAB.\n", operand);
                    return ERROR_VALUE;
                }
            }
            else
                assembled_address = 0;

            // assemble object code;
            assembled_object_code = assemble_instruction(opcode, operand, assembled_address);
        }
        else if (strcmp(opcode, "BYTE") == 0)
        {
            char operand_without_extraneous[MAX_TOKEN_LENGTH];
            get_literal_value(operand_without_extraneous, operand);

            if (operand[0] == 'X')
                assembled_object_code = strtol(operand_without_extraneous, NULL, 16);
            else if (operand[0] == 'C')
                assembled_object_code = get_string_literal_hex(operand);
            else
            {
                printf("ERROR: Unknown literal %s\n", operand);
                return ERROR_VALUE;
            }
        }
        else if (strcmp(opcode, "WORD") == 0)
        {
            char operand_without_extraneous[MAX_TOKEN_LENGTH];
            get_literal_value(operand_without_extraneous, operand);

            assembled_object_code = strtol(operand_without_extraneous, NULL, 16);
        }

        int obj_code_length = get_object_code_length(assembled_object_code);
        if (text_record_length + obj_code_length > 69)
        {
            update_text_record_length(temp_text_record, text_record_length);

            // Start new text_record.
            text_record_length = 0;
            fprintf(temp_text_record, "\n%c%06x%02x", 'T', text_record_start_address, text_record_length);
            text_record_start_address = location;
        }

        fprintf(temp_text_record, "%x", assembled_object_code);
        fprintf(assembly_listing, "%s\t%s\t%x\t%x\n", label, opcode, operand, assembled_object_code);
    }

    fprintf(temp_text_record, "E%x", start_address);
    return 1;
}
