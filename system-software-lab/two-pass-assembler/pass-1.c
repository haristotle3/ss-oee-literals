#include <stdio.h>
#include <stdlib.h> // for atoi()
#include <string.h> // for strcmp()

void passOne(FILE *, FILE *, FILE *, FILE *, FILE *);

int main()
{
    // input files
    FILE* input_file = fopen("input.txt", "r");
    FILE* operation_code_table = fopen("OPTAB.txt", "r");
    
    // output files
    FILE* intermediate_file = fopen("intermediate.txt", "w");
    FILE* symbol_table = fopen("SYMTAB.txt", "w");
    FILE* length = fopen("program_length.txt", "w");
}