#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()
#include <ctype.h>  // for isdigit()
#include <math.h>
#include "utils.h"

int symbol_search(char input_symbol[])
{
    // Returns 0 if symbol not found.
    // Returns 1 if found.
    // Also returns 1 if symbol is determined to be an immediate number.

    FILE *symbol_table = fopen("SYMTAB.txt", "r");
    char cmp_symbol[MAX_TOKEN_LENGTH];

    char symbol[MAX_TOKEN_LENGTH];
    // Check if operand contains index register.
    if (input_symbol[strlen(input_symbol) - 1] == 'X')
    {
        strncpy(symbol, input_symbol, strlen(input_symbol) - 2);
        symbol[strlen(input_symbol) - 2] = '\0';
    }
    else
        strcpy(symbol, input_symbol);

    while (fscanf(symbol_table, "%s\t%*x\n", cmp_symbol) > 0)
    {
        char cmp_immediate[MAX_TOKEN_LENGTH] = "#";
        char cmp_indirect[MAX_TOKEN_LENGTH] = "@";
        strcat(cmp_immediate, cmp_symbol);
        strcat(cmp_indirect, cmp_symbol);

        if (strcmp(symbol, cmp_symbol) == 0)
            return 1;
        if (strcmp(symbol, cmp_immediate) == 0)
            return 1;
        if (strcmp(symbol, cmp_indirect) == 0)
            return 1;
        if (is_immediate_number(symbol))
            return 1;
    }

    fclose(symbol_table);
    return 0;
}

int symbol_value(char *input_symbol)
{
    // Returns 0 if symbol not found.
    // Returns the symbol address as assigned in (pass 1) if found
    // Returns the immediate value string, converted to numeric value.

    FILE *symbol_table = fopen("SYMTAB.txt", "r");
    char cmp_symbol[MAX_TOKEN_LENGTH];

    char symbol[MAX_TOKEN_LENGTH];
    // Check if operand contains index register.
    if (input_symbol[strlen(input_symbol) - 1] == 'X')
    {
        strncpy(symbol, input_symbol, strlen(input_symbol) - 2);
        symbol[strlen(input_symbol) - 2] = '\0';
    }
    else
        strcpy(symbol, input_symbol);

    int symbol_value;

    while (fscanf(symbol_table, "%s\t%x\n", cmp_symbol, &symbol_value) > 0)
    {
        char cmp_immediate[MAX_TOKEN_LENGTH] = "#";
        char cmp_indirect[MAX_TOKEN_LENGTH] = "@";
        strcat(cmp_immediate, cmp_symbol);
        strcat(cmp_indirect, cmp_symbol);

        if (strcmp(symbol, cmp_symbol) == 0)
            return symbol_value;
        if (strcmp(symbol, cmp_immediate) == 0)
            return symbol_value;
        if (strcmp(symbol, cmp_indirect) == 0)
            return symbol_value;
        if (is_immediate_number(symbol))
            return get_immediate_value(symbol);
    }

    fclose(symbol_table);
    return 0;
}

void insert_symbol_to_SYMTAB(char symbol[], int location)
{
    FILE *symbol_table = fopen("SYMTAB.txt", "a");
    fprintf(symbol_table, "%s\t%x\n", symbol, location);
    fclose(symbol_table);

    return;
}

int opcode_search(char mnemonic[])
{
    // Returns 0 if opcode not found.
    // Returns 1 if found.

    FILE *opcode_table = fopen("OPTAB.txt", "r");
    char cmp_mnemonic[MAX_TOKEN_LENGTH];

    // OPTAB has fields | MNEMONIC | FORMAT | OPCODE |
    while (fscanf(opcode_table, "%s\t%*d\t%*x\n", cmp_mnemonic) > 0)
    {
        char cmp_format_4[MAX_TOKEN_LENGTH] = "+";
        strcat(cmp_format_4, cmp_mnemonic);
        if (strcmp(mnemonic, cmp_mnemonic) == 0)
        {
            fclose(opcode_table);
            return 1;
        }
        if (strcmp(mnemonic, cmp_format_4) == 0)
        {
            fclose(opcode_table);
            return 1;
        }
    }

    fclose(opcode_table);
    return 0;
}

int opcode_value(char mnemonic[])
{
    // Returns -1 if opcode not found.
    // Returns hex value of the opcode if found.

    FILE *opcode_table = fopen("OPTAB.txt", "r");
    char cmp_mnemonic[MAX_TOKEN_LENGTH];
    int opcode;
    // OPTAB has fields | MNEMONIC | FORMAT | OPCODE |
    while (fscanf(opcode_table, "%s\t%*d\t%x", cmp_mnemonic, &opcode) > 0)
    {
        char cmp_format_4[MAX_TOKEN_LENGTH] = "+";
        strcat(cmp_format_4, cmp_mnemonic);
        if (strcmp(mnemonic, cmp_mnemonic) == 0)
        {
            fclose(opcode_table);
            return opcode;
        }
        if (strcmp(mnemonic, cmp_format_4) == 0)
        {
            fclose(opcode_table);
            return opcode;
        }
    }

    fclose(opcode_table);
    return -1;
}

int opcode_instruction_format(char mnemonic[])
{
    // Returns -1 if opcode not found.
    // Returns the instruction format of that opcode if found.

    FILE *opcode_table = fopen("OPTAB.txt", "r");
    char cmp_mnemonic[MAX_TOKEN_LENGTH];
    int format;
    // There are no opcodes which are exclusively 3 or exclusively 4.
    // A format 3 opcode can also be used as a format 4 opcode.

    // OPTAB has fields | MNEMONIC | FORMAT | OPCODE |
    while (fscanf(opcode_table, "%s\t%d\t%*x", cmp_mnemonic, &format) > 0)
    {
        char cmp_format_4[MAX_TOKEN_LENGTH] = "+";
        strcat(cmp_format_4, cmp_mnemonic);
        if (strcmp(mnemonic, cmp_mnemonic) == 0)
        {
            fclose(opcode_table);
            return format;
        }
        if (strcmp(mnemonic, cmp_format_4) == 0)
        {
            fclose(opcode_table);
            return format + 1;
        }
    }

    fclose(opcode_table);
    return -1;
}

int is_immediate_number(char operand[])
{
    // Returns 1 if its an immediate number
    // Returns 0 if its not.
    if (operand[0] == '#')
    {
        for (int i = 1; i < strlen(operand); i++)
            if (!isdigit(operand[i]))
                return 0;

        return 1;
    }
    else
        return 0;
}

int get_immediate_value(char operand[])
{
    // Converts immediate value string to number and returns.
    int symbol_value = 0;
    for (int i = 1; i < strlen(operand); i++)
        symbol_value = 10 * symbol_value + (operand[i] - '0');

    return symbol_value;
}

void get_literal_value(char operand_without_extraneous[], char operand[])
{
    // strip the apostropes and the first character
    // 'C' or 'X' and return the remaining string.
    // We use strncpy.

    strncpy(operand_without_extraneous, operand + 2, strlen(operand) - 3);
    operand_without_extraneous[strlen(operand) - 3] = '\0';
    return;
}

int is_valid_constant(char *constant)
{
    // Checks for correctness of apostrophes in the constant
    if ((constant[0] == 'C' || constant[0] == 'X') &&
        (constant[1] == '\'') &&
        (constant[strlen(constant) - 1] == '\''))
        return 1;

    return 0;
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

    return (int)ceil(log2(assembled_object_code) / 4.0 / 2.0);
}