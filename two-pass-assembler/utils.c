#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()
#include "utils.h"

int symbol_search(char label[])
{
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

int opcode_search(char opcode[])
{
    FILE *opcode_table = fopen("OPTAB.txt", "r");
    char cmp_opcode[MAX_TOKEN_LENGTH];

    while (fscanf(opcode_table, "%s\t%*x", cmp_opcode) > 0)
        if (strcmp(opcode, cmp_opcode) == 0)
            return 1;

    fclose(opcode_table);
    return 0;
}

int symbol_value(char *req_symbol)
{
    FILE *symbol_table = fopen("SYMTAB.txt", "r");
    char cmp_symbol[MAX_TOKEN_LENGTH];
    int symbol_value;

    while (fscanf(symbol_table, "%s\t%x", cmp_symbol, &symbol_value))
        if (strcmp(cmp_symbol, req_symbol) == 0)
            return symbol_value;

    return 0;
}