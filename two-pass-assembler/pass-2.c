#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 25

int passTwo(FILE *, FILE *, int);

int main()
{
    // input files
    FILE *input_file = fopen("intermediate.txt", "r");
    FILE *operation_code_table = fopen("OPTAB.txt", "r");
    FILE *symbol_table = fopen("SYMTAB.txt", "r");
    FILE *length = fopen("program_length.txt", "r");

    int code_length;
    fscanf(length, "%d", &code_length);

    // output files
    FILE *object_program = fopen("object_program.txt", "w");

    passTwo(input_file, object_program, code_length);

    fclose(input_file);
    fclose(operation_code_table);
    fclose(symbol_table);
    fclose(length);
    fclose(object_program);

    return 0;
}

int passTwo(FILE *input_file, FILE *object_program, int code_length)
{

    FILE *temp_obj = fopen("temp.txt", "w");
    FILE *PC_file = fopen("intermediate.txt", "r");
    int program_counter;
    fscanf(PC_file, "%*s\t%*s\t%*s");
    fscanf(PC_file, "%x\t%*s\t%*s\t%*s", &program_counter);
    int START = program_counter;

    char location[MAX_TOKEN_LENGTH];
    char label[MAX_TOKEN_LENGTH];
    char opcode[MAX_TOKEN_LENGTH];
    char operand[MAX_TOKEN_LENGTH];

    fscanf(input_file, "%s\t%s\t%s", label, opcode, operand);
    if (strcmp(opcode, "START") == 0)
    {
        fprintf(object_program, "H%-6s%06x%06x\n", label, atoi(operand), code_length);
    }

    while (fscanf(input_file, "%s\t%s\t%s\t%s", location, label, opcode, operand) > 0)
    {
        fscanf(input_file, "%x\t%*s\t%*s\t%*s", &program_counter);
        int print_opcode = 0;
        int print_xbpe = 0;
        int target_address = 0;

        // search OPTAB for OPCODE.
        FILE *opcode_table = fopen("OPTAB.txt", "r");
        char searched_opcode[MAX_TOKEN_LENGTH];
        int opcode_value;

        int opcode_found = 0;
        while (fscanf(opcode_table, "%s\t%x", searched_opcode, &opcode_value) > 0)
        {
            if (strcmp(searched_opcode, opcode) == 0)
            {
                opcode_found = 1;
                break;
            }
        }

        if (opcode_found)
        {
            // Find symbol in symbol field.
            char searched_symbol[MAX_TOKEN_LENGTH];
            int symbol_value;

            FILE *symbol_table = fopen("SYMTAB.txt", "r");

            while (fscanf(symbol_table, "%s\t%x", searched_symbol, &symbol_value) > 0)
            {
                if (strcmp(searched_symbol, operand) == 0)
                    break;
            }

            if (strcmp(operand, "****") == 0)
            {
                fprintf(temp_obj, "%x\n", opcode_value);
            }
            else
            {
                print_opcode = opcode_value;
                // Now finding target address.
                // Check for n and i flag
                // if neither immediate nor indirect
                print_opcode += 3;

                // Check PC relative
                if ((symbol_value - program_counter) >= -2048 &&
                    (symbol_value - program_counter) <= 2047)
                {
                    target_address = (symbol_value - program_counter);
                    print_xbpe += 1 << 1;
                }

                if (searched_symbol[strlen(searched_symbol) - 1] == 'X')
                    print_xbpe += 1 << 3;

                // print everything in a temp file.
                fprintf(temp_obj, "%02x%01x%03x\n", print_opcode, print_xbpe, target_address);
            }
        }
        else if (strcmp(opcode, "BYTE") == 0 || strcmp(opcode, "WORD") == 0)
        {
            for (int i = 2; i < strlen(operand); i++)
            {
                fprintf(temp_obj, "%2x\n", operand[i]);
            }
            fprintf(temp_obj, "\n");
        }
        else if (strcmp(opcode, "TIX") == 0)
        {
            // Find symbol in symbol field.
            char searched_symbol[MAX_TOKEN_LENGTH];
            int symbol_value;

            FILE *symbol_table = fopen("SYMTAB.txt", "r");

            while (fscanf(symbol_table, "%2s\t%4x", searched_symbol, &symbol_value) > 0)
            {
                if (strcmp(searched_symbol, operand) == 0)
                    break;
            }
            fprintf(temp_obj, "%2x%4x\n", opcode_value, symbol_value);
        }
        else if (strcmp(opcode, "RESB") == 0 || strcmp(opcode, "RESW") == 0)
        {
            fprintf(temp_obj, "****\n");
        }
    }
    fclose(temp_obj);

    temp_obj = fopen("temp.txt", "r");
    fprintf(object_program, "T%06x%02x", START, code_length);

    char record[MAX_TOKEN_LENGTH];
    while (!feof(temp_obj))
    {
        fscanf(temp_obj, "%s", record);
        fprintf(object_program, "%s ", record);
    }
    fprintf(object_program, "\nE%06x", START);

    fclose(PC_file);
    printf("Success!");
    return 1;
}