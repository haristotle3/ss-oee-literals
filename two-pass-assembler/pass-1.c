#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()
#include <math.h>
#include "utils.h"

void init_symtab();
void init_littab_for_pass_1();
void print_littab();
void print_literals_to_intermediate_file(FILE *);
void insert_literal_to_LITTAB(char *);
void assign_addresses_to_literals(int);
int literal_found(char *);
int is_valid_literal(char *);
int passOne(FILE *, FILE *);

littab LITTAB;

int main()
{
    // input files
    FILE *input_file = fopen("input.txt", "r");

    // output files
    FILE *intermediate_file = fopen("intermediate.txt", "w");
    FILE *length = fopen("program_length.txt", "w");

    // Input file is an assembly program.
    // The program is written in a fixed format with fields
    // LABEL, OPCODE and OPERAND

    init_symtab();
    init_littab_for_pass_1();

    int program_length = passOne(input_file, intermediate_file);

    if (program_length == ERROR_VALUE)
        printf("Assembly failed.\n");
    else
        printf("Pass 1 of 2 of two completed successfully.\n");

    fclose(input_file);
    fclose(intermediate_file);

    // Program length is in decimal.
    fprintf(length, "%d", program_length);
    fclose(length);
    return 0;
}

int passOne(FILE *input_file, FILE *intermediate_file)
{
    char label[MAX_TOKEN_LENGTH];
    char mnemonic[MAX_TOKEN_LENGTH];
    char operand[MAX_TOKEN_LENGTH];

    unsigned int START = 0;
    unsigned int LOCCTR = 0;
    unsigned int start_address;

    // read first line
    fscanf(input_file, "%s\t%s\t%x", label, mnemonic, &start_address);

    if (strcmp(mnemonic, "START") == 0)
    {
        START = start_address;
        LOCCTR = START;

        // First line doesn't require a location.
        fprintf(intermediate_file, "%14s%10s%10x\n", label, mnemonic, start_address);

        // read next input line
        fscanf(input_file, "%s\t%s\t%s", label, mnemonic, operand);
    }
    else
    {
        printf("ERROR: Assembler expects START opcode in line 1.\n");
        return ERROR_VALUE;
    }

    while (strcmp(mnemonic, "END") != 0)
    {
        // if comment line, then go to next line
        if (strcmp(label, ".") == 0)
        {
            fscanf(input_file, "%s\t%s\t%s\n", label, mnemonic, operand);
            continue;
        }

        if (strcmp(mnemonic, "LTORG") == 0)
            fprintf(intermediate_file, "%04x%10s%10s%10s\n", LOCCTR, label, mnemonic, operand);
        else
            fprintf(intermediate_file, "%04x%10s%10s%10s\n", LOCCTR, label, mnemonic, operand);

        // If there is a symbol in the LABEL field
        if (strcmp(label, EMPTY) != 0)
        {
            if (!symbol_search(label))
                insert_symbol_to_SYMTAB(label, LOCCTR);
            else
            {
                printf("ERROR: %s redefined at %x\n", label, LOCCTR);
                return ERROR_VALUE;
            }
        }

        // search OPTAB for opcode.
        int opcode_found = opcode_search(mnemonic);
        int instruction_format = opcode_instruction_format(mnemonic);

        if (opcode_found && instruction_format == 1)
            LOCCTR += 1;
        else if (opcode_found && instruction_format == 2)
            LOCCTR += 2;
        else if (opcode_found && instruction_format == 3)
            LOCCTR += 3;
        else if (opcode_found && instruction_format == 4)
            LOCCTR += 4;
        else if (strcmp(mnemonic, "WORD") == 0)
            LOCCTR += 3;
        else if (strcmp(mnemonic, "RESW") == 0)
            LOCCTR += (atoi(operand) * 3);
        else if (strcmp(mnemonic, "RESB") == 0)
            LOCCTR += (atoi(operand));
        else if (strcmp(mnemonic, "BYTE") == 0)
        {
            if (is_valid_constant(operand))
            {
                if (operand[0] == 'X')
                    LOCCTR++;
                else if (operand[0] == 'C')
                    LOCCTR += strlen(operand) - 3; // Subrtact 1 each for the character C, and the two quote marks.
            }
            else
            {
                printf("ERROR: Invalid constant (%s) at %x.\n", operand, LOCCTR);
                return ERROR_VALUE;
            }
        }
        else if (strcmp(mnemonic, "BASE") == 0)
        {
            fscanf(input_file, "%s\t%s\t%s\n", label, mnemonic, operand);
            continue;
        }
        else if (strcmp(mnemonic, "LTORG") == 0)
        {
            assign_addresses_to_literals(LOCCTR);
            print_literals_to_intermediate_file(intermediate_file);
        }
        else
        {
            printf("ERROR: Invalid OPCODE (%s) at %x.\n", mnemonic, LOCCTR);
            return ERROR_VALUE;
        }

        // Handle literals.
        if (operand[0] == '=') // Literal is defined by a string starting with '=';
        {
            if (is_valid_literal(operand))
            {
                // if literal found, do nothing
                // else
                if (!literal_found(operand))
                    insert_literal_to_LITTAB(operand);
            }
            else
            {
                printf("Invalid literal (%s) at %x.\n", operand, LOCCTR);
                return ERROR_VALUE;
            }
        }

        // Read next line
        fscanf(input_file, "%s\t%s\t%s", label, mnemonic, operand);
    }

    int program_length = LOCCTR - START;
    fprintf(intermediate_file, "%04x%10s%10s%10s\n", 0000, EMPTY, "END", EMPTY);
    assign_addresses_to_literals(LOCCTR);
    print_literals_to_intermediate_file(intermediate_file);

    print_littab();

    return program_length;
}

void init_symtab()
{
    // Erases contents of SYMTAB if it exists, else creates anew.
    FILE *symbol_table = fopen("SYMTAB.txt", "w");
    fclose(symbol_table);

    return;
}

void init_littab_for_pass_1()
{
    LITTAB.current_size = 0;
    LITTAB.unassigned_index = 0;
    LITTAB.not_printed_index = 0;

    return;
}

void insert_literal_to_LITTAB(char *literal)
{
    LITTAB.current_size++;

    int insert_index = LITTAB.current_size - 1; // index where literal will be inserted.
    char literal_as_operand[MAX_TOKEN_LENGTH];
    char operand_without_extraneous[MAX_TOKEN_LENGTH];

    strcpy(LITTAB.table[insert_index].symbol, literal);
    // Copy literal without the '=' symbol
    strncpy(literal_as_operand, literal + 1, strlen(literal));
    get_literal_value(operand_without_extraneous, literal_as_operand);

    if (literal[1] == 'X')
        LITTAB.table[insert_index].value = strtol(operand_without_extraneous, NULL, 16);
    else if (literal[1] == 'C')
        LITTAB.table[insert_index].value = get_string_literal_hex(operand_without_extraneous);

    LITTAB.table[insert_index].length = get_object_code_length(LITTAB.table[insert_index].value);

    // Address is unassigned.

    return;
}

void print_literals_to_intermediate_file(FILE *intermediate_file)
{
    for (int i = LITTAB.not_printed_index; i < LITTAB.current_size; i++)
    {
        littab_element literal = LITTAB.table[i];
        fprintf(intermediate_file, "%04x%10s%10s%10s\n", literal.address, "*", literal.symbol, EMPTY);
        LITTAB.not_printed_index++;
    }

    return;
}

void assign_addresses_to_literals(int LOCCTR)
{
    for (int i = LITTAB.unassigned_index; i < LITTAB.current_size; i++)
    {
        LITTAB.table[i].address = LOCCTR;
        LOCCTR += LITTAB.table[i].length;
        LITTAB.unassigned_index++;
    }
}

int literal_found(char *operand)
{
    // simple linear search
    for (int i = 0; i < LITTAB.current_size; i++)
    {
        if (strcmp(operand, LITTAB.table[i].symbol) == 0)
            return 1;
    }

    return 0;
}

int is_valid_literal(char *operand)
{
    // checks whether the operand is in the format =C'value' or not
    // Checks apostrophes are correct or not.
    // if (operand[1] == '*' && strlen(operand) == 1)
    //     return 1;
    if ((operand[1] == 'C' || operand[1] == 'X') &&
        (operand[2] == '\'') &&
        (operand[strlen(operand) - 1] == '\''))
        return 1;

    return 0;
}

void print_littab()
{
    // creates littab.txt and prints the table.
    // also creates and littab_length.txt and prints the length.
    FILE *littab = fopen("littab.txt", "w");
    FILE *littab_length = fopen("littab_length.txt", "w");
    fprintf(littab, "%10s%10s%10s%10s\n", "NAME|", "VALUE|", "LENGTH|", "ADDRESS|");
    fprintf(littab, "----------------------------------------\n");

    for (int i = 0; i < LITTAB.current_size; i++)
    {
        littab_element literal = LITTAB.table[i];
        fprintf(littab, "%10s%10x%10x%10x\n", literal.symbol, literal.value, literal.length, literal.address);
    }

    fprintf(littab, "----------------------------------------\n");
    fprintf(littab_length, "%d", LITTAB.current_size);

    fclose(littab);
    fclose(littab_length);
    return;
}