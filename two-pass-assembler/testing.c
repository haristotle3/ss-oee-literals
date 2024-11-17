#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"

FILE *PROGRAM_COUNTER_FILE;
int PROGRAM_COUNTER;
int BASE = 0;

unsigned long long int assemble_instruction(char mnemonic[], char operand[], int symbol_address)
{
    // Returns an assembled object code from given input parameters.
    unsigned long long int assembled_object_code;
    int instruction_format = opcode_instruction_format(mnemonic);

    if (instruction_format == 1)
        assembled_object_code = opcode_value(mnemonic);
    else if (instruction_format == 2)
    {
        // Register table:
        // PC and SW are not needed, however SW can be used for error flags.
        // For now its not needed.
        char registers[NUM_REGISTERS] = {'A', 'X', 'L', 'B', 'S', 'T', 'F'};
        int r1 = 0, r2 = 0;

        assembled_object_code = opcode_value(mnemonic);

        // Find r1 number
        for (int reg_number = 0; reg_number < NUM_REGISTERS; reg_number++)
            if (operand[0] == registers[reg_number])
                r1 = reg_number;

        // Find r2 number
        if (strlen(operand) == 3) // If there are two registers in operand, then only.
            for (int reg_number = 0; reg_number < NUM_REGISTERS; reg_number++)
                if (operand[2] == registers[reg_number])
                    r2 = reg_number;

        assembled_object_code <<= 4;
        assembled_object_code += r1;

        assembled_object_code <<= 4;
        assembled_object_code += r2;
    }
    else if (instruction_format == 3 || instruction_format == 4)
    {
        assembled_object_code = opcode_value(mnemonic);
        // Check if indirect
        if (operand[0] == '@')
            // Set n flag on;
            assembled_object_code += 2;
        // Check if immediate
        else if (operand[0] == '#')
            // Set i flag on;
            assembled_object_code += 1;
        else
            // neither immediate nor indirect
            assembled_object_code += 3;
        // Dealing with x, b, p, e flags
        // We don't need any other registers as variables other than program counter and base

        // X flag
        assembled_object_code <<= 1;
        if (operand[strlen(operand) - 1] == 'X')
            assembled_object_code += 1; // Set X flag to 1;

        // B and P flags
        assembled_object_code <<= 2;
        int displacement;

        if (operand[0] == '#')
            displacement = symbol_address;
        else
            displacement = symbol_address - PROGRAM_COUNTER;

        if (operand[0] != '#' && -2048 <= displacement && displacement <= +2047)
            assembled_object_code += 1; // Set P flag to 1;
        else if (operand[0] != '#' && 0 <= displacement && displacement <= 4095)
            assembled_object_code += 2; // Set B flag to 1;
        else // value too big to be addressed from base relative or PC relative.
        // It will be a format 4 instruction.
            displacement = symbol_address;

        // Set E flag
        assembled_object_code <<= 1;
        if (instruction_format == 4)
            assembled_object_code += 1;

        // Add displacement field.
        if (instruction_format == 3)
        {
            displacement &= 0x00000fff;
            assembled_object_code <<= 12;
        }
        else if (instruction_format == 4)
        {
            displacement &= 0x000fffff;
            assembled_object_code <<= 20;
        }

        assembled_object_code += displacement;
    }

    return assembled_object_code;
}

int main()
{
    PROGRAM_COUNTER = 0x002d;
    printf("FORMAT: %d\n", opcode_instruction_format("J"));
    printf("OPCODE VALUE: %x\n", opcode_value("J"));
    printf("SYMB VALUE: %x\n", symbol_value("@RETADR"));
    printf("%x\n", assemble_instruction("J", "@RETADR", symbol_value("@RETADR")));
}