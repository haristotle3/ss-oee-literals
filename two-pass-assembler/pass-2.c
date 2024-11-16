#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"

unsigned long long int assemble_instruction(char *, char *, int);
// hardest. Need to use some creativity.
void get_literal_value(char *, char *);
unsigned long long int get_string_literal_hex(char *);
int get_object_code_length(unsigned long long int);
void update_text_record_length(FILE *, int);

int increment_pc();
void init_pc_file();

FILE *PROGRAM_COUNTER_FILE;
int PROGRAM_COUNTER;
int BASE = 0;
int INDEX_REG = 0;

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
    char mnemonic[MAX_TOKEN_LENGTH];
    char operand[MAX_TOKEN_LENGTH];

    fscanf(input_file, "%s\t%s\t%x", program_name, mnemonic, &start_address);

    if (strcmp(mnemonic, "START") == 0)
        fprintf(assembly_listing, "%s\t%s\t%x\n", program_name, mnemonic, start_address);
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
    fprintf(object_program, "H%-6s%06x%06x\n", program_name, start_address, length);

    int text_record_length = 0;
    FILE *temp_text_record = fopen("Temp_text_record.txt", "w");
    fprintf(temp_text_record, "%c%06x%02x", 'T', start_address, text_record_length);
    // Later we have to use fseek() and fputc() to replace 0 (text record length)
    // to appropriate value.

    unsigned long long int assembled_object_code = 0;
    int text_record_length = 0;
    int text_record_start_address = start_address;
    init_pc_file();

    while (fscanf(input_file, "%x\t%s\t%s\t%s", &location, label, mnemonic, operand) > 0)
    {
        increment_pc();
        // No comments in intermediate file.
        if (opcode_search(mnemonic))
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
            assembled_object_code = assemble_instruction(mnemonic, operand, symbol_address);
        }
        else if (strcmp(mnemonic, "BYTE") == 0)
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
        else if (strcmp(mnemonic, "WORD") == 0)
            assembled_object_code = (unsigned long long int)strtol(operand, NULL, 16);
        else if (strcmp(mnemonic, "BASE") == 0)
        {
            BASE = symbol_value(operand);
            continue;
        }
        else if (strcmp(mnemonic, "RESW") == 0 || strcmp(mnemonic, "WORD") == 0)
            continue;

        int obj_code_length = get_object_code_length(assembled_object_code);
        if (text_record_length + obj_code_length > 69)
        {
            update_text_record_length(temp_text_record, text_record_length);

            // Start new text_record.
            text_record_length = 0;
            text_record_start_address = location;
            fprintf(temp_text_record, "\n%c%06x%02x", 'T', text_record_start_address, text_record_length);
        }
        else
            text_record_length += obj_code_length;

        // Write the assembled object code.
        // %0*x is variable padding length, length is obj_code_length.
        fprintf(temp_text_record, "%0*llx", obj_code_length, assembled_object_code);
        fprintf(assembly_listing, "%x\t%s\t%s\t%s\t%llx\n", location, label, mnemonic, operand, assembled_object_code);
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

unsigned long long int assemble_instruction(char mnemonic[], char operand[], int symbol_address)
{
    // Returns an assembled object code from given input parameters.
    unsigned long long int assembled_object_code;
    int instruction_format = opcode_instruction_format(mnemonic);

    // Register table:
    // PC and SW are not needed, however SW can be used for error flags.
    // For now its not needed.
    char registers[NUM_REGISTERS] = {'A', 'X', 'L', 'B', 'S', 'T', 'F'};

    if (instruction_format == 1)
        assembled_object_code = opcode_value(mnemonic);
    else if (instruction_format == 2)
    {
        int r1 = 0, r2 = 0;

        assembled_object_code = opcode_value(mnemonic);

        // Find r1 number
        for (int reg_number = 0; reg_number < NUM_REGISTERS; reg_number++)
            if (operand[0] == registers[reg_number])
                r1 = reg_number;

        // Find r2 number
        for (int reg_number = 0; reg_number < NUM_REGISTERS; reg_number++)
            if (operand[2] == registers[reg_number])
                r2 = reg_number;

        assembled_object_code <<= 4;
        assembled_object_code += r1;

        assembled_object_code <<= 4;
        assembled_object_code += r2;
    }
    else if (instruction_format == 3)
    {
        assembled_object_code = opcode_value(mnemonic);
        assembled_object_code <<= 2;
        // Check if indirect
        if (operand[0] == '@')
            // Set n flag on;
            assembled_object_code += 2;
        else if (operand[0] == '#')
            // Set i flag on;
            assembled_object_code += 1;
        else
            // neither immediate nor indirect
            assembled_object_code += 3;

        assembled_object_code <<= 4;
        // Dealing with x, b, p, e flags
        // I think it would be better if we had an array of registers;
    }
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

int increment_pc()
{
    if (!feof(PROGRAM_COUNTER_FILE))
    {
        fscanf(PROGRAM_COUNTER_FILE, "%x\t%*s\t%*s\t%*s\n", &PROGRAM_COUNTER);
        return 1;
    }
    else
        return 0;
}

int get_object_code_length(unsigned long long int assembled_object_code)
{
    // get the length of the object code,
    // just take log to the base 16, which gives the number of hexadecimal digits.
    // then divide by 2, since each hexadecimal digit represents a nibble,
    // therefore 2 nibbles make a byte;
    // then take the ceiling function to get length.
    // There will always be an even number of hexadecimal digits.
    // Therefore, when divided by 2, results will be have odd and even values,
    // which is exactly what we expect

    // By change of base formula,
    // log16(x) = log2(x)/log2(16) = log2(x) / 4

    return ceil(log2(assembled_object_code) / 4 / 2);
}

unsigned long long int get_string_literal_hex(char operand_without_extraneous[])
{
    // convert string to ascii value string (as an unsigned long int) and return.
    unsigned long long int obj_code = 0;

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
    // Text record length is in bytes, two hex digits make a byte.
    // But each character stored in a file is takes up a byte.

    // Therefore we have to multiply text_record_length by 2 to get to
    // the column where length is stored.

    // File points to the place after the '\n' character and therefore
    // Minus one gets to '\n', and minus 2 gets to the last character of the text record.
    // Think of it as indexing starting from 2, instead of 0,

    // Then we have to index to the Nth character if indexed from 0,
    // But here index is from 2, so we have to index to (N+2) th character

    // Example, if the record length (in characters) 60, then we have to index to 62nd
    // Character from last (since 2 bytes is for text-record-length columns)

    // Therefore we have to index to -(60 + 2 + 2)th index.
    fseek(temp_text_record, 2 * -(text_record_length)-2 - 2, SEEK_CUR);
    fprintf(temp_text_record, "%02x", text_record_length);

    // Go back to the end of the file.
    fseek(temp_text_record, 0, SEEK_END);

    return;
}