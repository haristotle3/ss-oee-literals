#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()
#include <ctype.h>  // for isdigit()
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
        strncpy(symbol, input_symbol, strlen(input_symbol) - 2);
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
        strncpy(symbol, input_symbol, strlen(input_symbol) - 2);
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
