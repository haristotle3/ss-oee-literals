#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"

unsigned int assemble_instruction(opcode, operand, symbol_address);
// hardest. Need to use some creativity.
void get_literal_value(char operand_without_extraneous[], char operand[]);
unsigned long int get_string_literal_hex(char operand_without_extraneous[]);
int get_object_code_length(unsigned long int assembled_object_code);
void update_text_record_length(FILE *temp_text_record, int text_record_length);

void increment_pc();
void init_pc_file();

FILE *PROGRAM_COUNTER_FILE;
int PROGRAM_COUNTER;
int BASE = 0;

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

    if (passTwo(input_file, object_program, assembly_listing) == ERROR_VALUE)
        printf("Assembly failed.\n");
    else
        printf("Success!\n");

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
    int text_record_length = 0;
    int text_record_start_address = start_address;
    init_pc_file();

    while (fscanf(input_file, "%x\t%s\t%s\t%s", &location, label, opcode, operand) > 0)
    {
        increment_pc();
        // No comments in intermediate file.
        if (opcode_search(opcode))
        {
            int symbol_address;
            if (strcmp(operand, EMPTY) != 0)
            {
                if (symbol_search(operand))
                    symbol_address = symbol_value(operand);
                else
                {
                    printf("ERROR: %s doesn't exist in SYMTAB.\n", operand);
                    return ERROR_VALUE;
                }
            }
            else
                symbol_address = 0;

            // assemble object code
            assembled_object_code = assemble_instruction(opcode, operand, symbol_address);
        }
        else if (strcmp(opcode, "BYTE") == 0)
        {
            char operand_without_extraneous[MAX_TOKEN_LENGTH];
            get_literal_value(operand_without_extraneous, operand);

            if (operand[0] == 'X')
                assembled_object_code = strtol(operand_without_extraneous, NULL, 16);
            else if (operand[0] == 'C')
                assembled_object_code = get_string_literal_hex(operand_without_extraneous);
            else
            {
                printf("ERROR: Unknown literal %s\n", operand);
                return ERROR_VALUE;
            }
        }
        else if (strcmp(opcode, "WORD") == 0)
            assembled_object_code = strtol(operand, NULL, 16);

        int obj_code_length = get_object_code_length(assembled_object_code);
        if (text_record_length + obj_code_length > 69)
        {
            update_text_record_length(temp_text_record, text_record_length);

            // Start new text_record.
            text_record_length = 0;
            fprintf(temp_text_record, "\n%c%06x%02x", 'T', text_record_start_address, text_record_length);
            text_record_start_address = location;
        }
        else
            text_record_length += obj_code_length;

        // Write the assembled object code.
        // %0*x is variable padding length, length is obj_code_length.
        fprintf(temp_text_record, "%0*x", obj_code_length, assembled_object_code);
        fprintf(assembly_listing, "%s\t%s\t%x\t%x\n", label, opcode, operand, assembled_object_code);
    }

    fclose(temp_text_record);

    // Write to object program
    temp_text_record = fopen("Temp_text_record.txt", "r");

    for (char ch = fgetc(temp_text_record); ch != EOF; ch = fgetc(temp_text_record))
        fputc(ch, object_program);

    fclose(temp_text_record);

    // Write the end record
    fprintf(object_program, "E%06x\n", start_address);
    fclose(PROGRAM_COUNTER_FILE);
    return 1;
}

unsigned int assemble_instruction(char opcode[], char operand[], int symbol_address)
{
    // Returns an assembled object code from given input parameters.
}

void init_pc_file()
{
    PROGRAM_COUNTER_FILE = fopen("intermediate.txt", "r");

    // Read first line and ignore.
    fscanf(PROGRAM_COUNTER_FILE, "%*s\t%*s\t%*s");

    // Initialize program counter variable.
    fscanf(PROGRAM_COUNTER_FILE, "%x\t%*s\t%*s\t%*s", &PROGRAM_COUNTER);

    return;
}

void increment_pc()
{
    if (!feof(PROGRAM_COUNTER_FILE))
        fscanf(PROGRAM_COUNTER_FILE, "%x\t%*s\t%*s\t%*s\n", &PROGRAM_COUNTER);

    return;
}

int get_object_code_length(unsigned long int assembled_object_code)
{
    // get the length of the code,
    // take log to the base 16 and apply ceiling function to it.
    // then divide the result by 2, since one hexadecimal digit is one nibble,
    // 2 hexadecimal digits make 1 byte.

    // By change of base formula,
    // log16(x) = log2(x)/log2(16) = log2(x) / 4

    return ceil(log2(assembled_object_code) / 4 / 2);
}

unsigned long int get_string_literal_hex(char operand_without_extraneous[])
{
    // convert string to ascii value string (as an unsigned long int) and return.
    unsigned long int obj_code = 0;

    for (int i = 0; i < strlen(operand_without_extraneous); i++)
    {
        obj_code <<= 8;
        obj_code += operand_without_extraneous[i];
    }

    return obj_code;
}

void get_literal_value(char operand_without_extraneous[], char operand[])
{
    // strip the apostropes and the first character
    // 'C' or 'X' and return the remaining string.
    // We use strncpy.

    strncpy(operand_without_extraneous, operand + 2, strlen(operand) - 3);
    return;
}

void update_text_record_length(FILE *temp_text_record, int text_record_length)
{
    fseek(temp_text_record, 1 + 6, SEEK_SET);
    fprintf(temp_text_record, "%02x", text_record_length);

    return;
}