#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_BUF 512
#define WORD_SIZE 6

void ll_pass_one(FILE *object_programs, int PROGADDR);
int get_csect_length(char *input_record);
void get_csect_name(char *csect_name, char *input_record);
int search_csect_name(char *csect_name);

int main()
{
    FILE *object_programs = fopen("object_programs.txt", "r");
    FILE *load_address_file = fopen("load_address.txt", "r");

    // get PROGADDR from `operating system`
    int PROGADDR;
    fscanf(load_address_file, "%x", &PROGADDR);

    ll_pass_one(object_programs, PROGADDR);
}

void ll_pass_one(FILE *object_programs, int PROGADDR)
{
    // create ESTAB.
    FILE *ESTAB = fopen("ESTAB.txt", "w");
    fclose(ESTAB);
    int CSADDR = PROGADDR; // for first control section
    int CSLTH = 0;

    char input_record[MAX_BUF];
    char csect_name[MAX_BUF];
    char symbol_name[MAX_BUF];
    int indicated_address;

    while (fgets(input_record, MAX_BUF, object_programs) != EOF)
    {
        CSLTH = get_csect_length(input_record);
        get_csect_name(csect_name, input_record);

        // strncpy(csect_name, input_record + 1, WORD_SIZE);
        // csect_name[WORD_SIZE] = '\0';

        int found = search_csect_name(csect_name);

        if (found)
        {
            printf("ERROR: Duplicate control section symbol (%s)\n", csect_name);
            return;
        }
        else
            insert_csect_to_estab(csect_name, CSADDR, CSLTH);

        // while record_type != E
        while (input_record[0] != 'E')
        {
            fgets(input_record, MAX_BUF, object_programs);
            if (input_record[0] == 'D')
            {
                // for each symbol in record
                for (int i = 1; i < strlen(input_record); i += 12)
                {
                    get_symbol_name(symbol_name, input_record, i);
                    found = search_symbol();

                    if (found)
                    {
                        printf("ERROR: Duplicate external symbol (%s)\n", symbol_name);
                        return;
                    }
                    else
                    {
                        indicated_address = get_indicated_address(input_record, i);
                    }
                }
            }
        }
    }
}