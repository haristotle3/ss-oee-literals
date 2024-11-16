#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utils.h"

FILE *PROGRAM_COUNTER_FILE;
int PROGRAM_COUNTER;
int BASE = 0;

void init_pc_file() // Tested
{
    PROGRAM_COUNTER_FILE = fopen("intermediate.txt", "r");

    // Read first line and ignore.
    fscanf(PROGRAM_COUNTER_FILE, "%*s\t%*s\t%*s");

    // Initialize program counter variable.
    fscanf(PROGRAM_COUNTER_FILE, "%x\t%*s\t%*s\t%*s", &PROGRAM_COUNTER);

    return;
}

int increment_pc() // Tested
{
    if (!feof(PROGRAM_COUNTER_FILE))
    {
        fscanf(PROGRAM_COUNTER_FILE, "%x\t%*s\t%*s\t%*s\n", &PROGRAM_COUNTER);
        return 1;
    }
    else
        return 0;
}

int get_object_code_length(unsigned long int assembled_object_code) // Tested
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

int main()
{
    printf("Object Code %0*x, Length: %x\n", 2 * get_object_code_length(0x4b101036), 0x4b101036, get_object_code_length(0x4b101036));
    printf("Object Code %0*x, Length: %x\n", 2 * get_object_code_length(0x0f2016), 0x0f2016, get_object_code_length(0x0f2016));
    printf("Object Code %0*x, Length: %x\n", 2 * get_object_code_length(0x3e2003), 0x3e2003, get_object_code_length(0x3e2003));
    printf("Object Code %0*x, Length: %x\n", 2 * get_object_code_length(0x454f46), 0x454f46, get_object_code_length(0x454f46));
    printf("Object Code %0*x, Length: %x\n", 2 * get_object_code_length(0x0f200d), 0x0f200d, get_object_code_length(0x0f200d));
    printf("Object Code %0*x, Length: %x\n", 2 * get_object_code_length(0x05), 0x05, get_object_code_length(0x05));

    return 0;
}