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

unsigned long long int get_string_literal_hex(char operand_without_extraneous[]) // Tested
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

void get_literal_value(char operand_without_extraneous[], char operand[]) // Tested
{
    // strip the apostropes and the first character
    // 'C' or 'X' and return the remaining string.
    // We use strncpy.

    strncpy(operand_without_extraneous, operand + 2, strlen(operand) - 3);
    return;
}

void update_text_record_length(FILE *temp_text_record, int text_record_length) // Tested
{
    // Text record length is in bytes, two hex digits make a byte.
    // But each character stored in a file is takes up a byte.

    // Therefore we have to multiply text_record_length by 2 to get to
    // the column where length is stored.

    // Text_record length excludes '\n' so we have to add +1 before multiplying by 2.
    // Finally subtract two to get to text-record-length column.

    // File points to the place after the '\n' character and therefore
    // the above offset arithmetic is correct.

    fseek(temp_text_record, 2 * -(text_record_length + 1) - 2, SEEK_CUR);
    fprintf(temp_text_record, "%02x", text_record_length);

    // Go back to the end of the file.
    fseek(temp_text_record, 0, SEEK_END);

    return;
}

int main()
{

    return 0;
}