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
        fscanf(input_file, "%d\t%*s\t%*s\t%*s", &program_counter);
        int print_opcode = 0;
        int print_xbpe = 0;
        int target_address = 0;

        // search OPTAB for OPCODE.
        FILE *opcode_table = fopen("OPTAB.txt", "r");
        char searched_opcode[MAX_TOKEN_LENGTH];
        char opcode_value[MAX_TOKEN_LENGTH];

        int opcode_found = 0;
        while (fscanf(opcode_table, "%s\t%s", searched_opcode, opcode_value) > 0)
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
            char symbol_value[MAX_TOKEN_LENGTH];

            if (strcmp(operand, "****") != 0)
            {
                FILE *symbol_table = fopen("SYMTAB.txt", "r");

                int symbol_found = 0;
                while (fscanf(symbol_table, "%s\t%s", searched_symbol, symbol_value) > 0)
                {
                    if (strcmp(searched_symbol, operand) == 0)
                    {
                        symbol_found = 1;
                        break;
                    }
                }
                fclose(symbol_table);
            }
            print_opcode = atoi(opcode_value);

            // Now finding target address.
            // Check for n and i flag
            if (searched_symbol[0] == '#')
                print_opcode += 1;
            else if (searched_symbol[0 == '@'])
                print_opcode += 2;
            else // if neither immediate nor indirect
                print_opcode += 3;

            // Check PC relative
            if ((atoi(symbol_value) - program_counter) >= -2048 &&
                (atoi(symbol_value) - program_counter) <= 2047)
            {
                target_address = (atoi(symbol_value) - program_counter);
                print_xbpe += 1 << 1;
            }
            else if ((atoi(symbol_value) - program_counter) <= 4095)
            {
                // Base relative.
            }

            if (searched_symbol[strlen(searched_symbol) - 1] == 'X')
                print_xbpe += 1 << 3;

            if (searched_symbol[0] == '+')
                print_xbpe += 1;

            // print everything in a temp file.
            fprintf(temp_obj, "%2x%1x%3x\n", print_opcode, print_xbpe, target_address);
            printf("%2x%1x%3x\n", print_opcode, print_xbpe, target_address);
        }
        else if (strcmp(opcode, "BYTE") == 0 || strcmp(opcode, "WORD") == 0)
        {
            fprintf(temp_obj, "%06x\n", operand);
            printf("%06x\n", operand);
        }
    }

    fclose(temp_obj);
    fclose(PC_file);
    printf("Success!");
    return 1;
}