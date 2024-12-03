#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_BUF 512
#define WORD_SIZE 6

void ll_pass_one(FILE *object_programs, int PROGADDR);

int search_csect_name(char *csect_name);
void csect_name_insert(char *csect_name, int CSADDR, int CSLTH);

void get_symbol_name(char *symbol_name, char *input_record, int start_index);
int search_symbol(char *symbol_name);

int get_indicated_address(char *input_record, int start_index);
void symbol_insert(char *symbol_name, int target_address);

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

    char record_type;
    char csect_name[MAX_BUF];
    int CSLTH = 0;
    int CSADDR;

    char input_record[MAX_BUF];
    char symbol_name[MAX_BUF];
    int indicated_address;

    int first = 1;

    while (fscanf("%c%6s%6x%6x\n", &record_type, csect_name, &CSADDR, &CSLTH) > 0)
    {
        if (first)
        {
            CSADDR = PROGADDR; // for first control section
            first = 0;
        }

        int found = search_csect_name(csect_name);
        if (found)
        {
            printf("ERROR: Duplicate external symbol (%s)", csect_name);
            return;
        }
        else
            csect_name_insert(csect_name, CSADDR, CSLTH);

        while (record_type != 'E')
        {
            fgets(input_record, MAX_BUF, object_programs);
            record_type = input_record[0];

            if (record_type == 'D')
            {
                // for each symbol in the record
                for (int i = 1; i < strlen(input_record); i += 12)
                {
                    get_symbol_name(symbol_name, input_record, i);
                    found = search_symbol(symbol_name);

                    if (found)
                    {
                        printf("ERROR: Duplicate external symbol (%s)", csect_name);
                        return;
                    }
                    else
                    {
                        indicated_address = get_indicated_address(input_record, i);
                        symbol_insert(symbol_name, CSADDR + indicated_address);
                    }
                }
            }
        }

        CSADDR += CSLTH;
    };
}