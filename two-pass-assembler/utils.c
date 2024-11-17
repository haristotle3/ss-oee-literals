#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()
#include "utils.h"

int symbol_search(char label[])
{
    // Returns 0 if symbol not found.
    // Returns 1 if found.
    FILE *symbol_table = fopen("SYMTAB.txt", "r");
    char cmp_symbol[MAX_TOKEN_LENGTH];
    char cmp_immediate[MAX_TOKEN_LENGTH] = "#";
    char cmp_indirect[MAX_TOKEN_LENGTH] = "@";

    while (fscanf(symbol_table, "%s\t%*x", cmp_symbol) > 0)
    {
        strcat(cmp_immediate, label);
        strcat(cmp_indirect, label);

        if (strcmp(label, cmp_symbol) == 0)
            return 1;
        if (strcmp(label, cmp_immediate) == 0)
            return 1;
        if (strcmp(label, cmp_indirect) == 0)
            return 1;
    }

    fclose(symbol_table);
    return 0;
}

int symbol_value(char *req_symbol)
{
    // Returns 0 if symbol not found.
    // Returns the symbol address as assigned in (pass 1) if found

    FILE *symbol_table = fopen("SYMTAB.txt", "r");
    char cmp_symbol[MAX_TOKEN_LENGTH];
    char cmp_immediate[MAX_TOKEN_LENGTH] = "#";
    char cmp_indirect[MAX_TOKEN_LENGTH] = "@";
    int symbol_value;

    while (fscanf(symbol_table, "%s\t%x", cmp_symbol, &symbol_value) > 0)
    {
        strcat(cmp_immediate, cmp_symbol);
        strcat(cmp_indirect, cmp_symbol);

        if (strcmp(req_symbol, cmp_symbol) == 0)
            return symbol_value;
        if (strcmp(req_symbol, cmp_immediate) == 0)
            return symbol_value;
        if (strcmp(req_symbol, cmp_indirect) == 0)
            return symbol_value;
    }

    return 0;
}

int opcode_search(char mnemonic[])
{
    // Returns 0 if opcode not found.
    // Returns 1 if found.

    FILE *opcode_table = fopen("OPTAB.txt", "r");
    char cmp_mnemonic[MAX_TOKEN_LENGTH];

    // OPTAB has fields | MNEMONIC | FORMAT | OPCODE |
    while (fscanf(opcode_table, "%s\t%*d\t%*x", cmp_mnemonic) > 0)
        if (strcmp(mnemonic, cmp_mnemonic) == 0)
        {
            fclose(opcode_table);
            return 1;
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
    char cmp_format_4[MAX_TOKEN_LENGTH] = "+";
    // OPTAB has fields | MNEMONIC | FORMAT | OPCODE |
    while (fscanf(opcode_table, "%s\t%*d\t%x", cmp_mnemonic, &opcode) > 0)
    {
        strcat(cmp_format_4, cmp_mnemonic);
        if (strcmp(mnemonic, cmp_mnemonic) == 0)
        {
            fclose(opcode_table);
            return opcode;
        }
        else if (strcmp(mnemonic, cmp_format_4) == 0)
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
    char cmp_format_4[MAX_TOKEN_LENGTH] = "+";
    // There are no opcodes which are exclusively 3 or exclusively 4.
    // A format 3 opcode can also be used as a format 4 opcode.

    // OPTAB has fields | MNEMONIC | FORMAT | OPCODE |
    while (fscanf(opcode_table, "%s\t%d\t%*x", cmp_mnemonic, &format) > 0)
    {
        strcat(cmp_format_4, cmp_mnemonic);
        if (strcmp(mnemonic, cmp_mnemonic) == 0)
        {
            fclose(opcode_table);
            return format;
        }
        else if (strcmp(mnemonic, cmp_format_4) == 0)
        {
            fclose(opcode_table);
            return format + 1;
        }
    }
    fclose(opcode_table);
    return -1;
}
