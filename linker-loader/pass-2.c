#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ERROR_VALUE -1
#define MAX_BUF 512
#define WORD_SIZE 6

int ll_pass_two(FILE *object_programs, int PROGADDR);
void move_objcode_to_memory(FILE *object_programs, int tr_start_address, int tr_length, int CSLTH);

int main()
{
    FILE *object_programs = fopen("object_programs.txt", "r");
    FILE *load_address_file = fopen("load_address.txt", "r");

    int PROGADDR;
    fscanf(load_address_file, "%x", &PROGADDR);

    if (ll_pass_two(object_programs, PROGADDR) == ERROR_VALUE)
        printf("Loading failed.\n");
    else
        printf("Success!\n");
}

int ll_pass_two(FILE *object_programs, int PROGADDR)
{
    int CSADDR = PROGADDR;
    int EXECADDR = PROGADDR;

    typedef struct
    {
        char record_type;
        char csect_name[MAX_BUF];
        int csect_address;
        int CSLTH;
    } header_record;

    typedef struct
    {
        int start_address;
        short int length;
        int object_code;
    } text_record;

    header_record hr;
    text_record tr;
    char current_record_type;

    while (fscanf(object_programs, "%c%6s%6x%6x\n", &hr.record_type, hr.csect_name, &hr.csect_address, &hr.CSLTH) > 0)
    {
        current_record_type = hr.record_type;
        while (current_record_type != 'E')
        {
            fscanf(object_programs, "%c", &current_record_type);
            if (current_record_type == 'T')
            {
                fscanf(object_programs, "%6x%2x", &tr.start_address, &tr.length);
                move_objcode_to_memory(object_programs, tr.start_address, tr.length, hr.CSLTH);
            }
        }
    }
    return 1;
}

void move_objcode_to_memory(FILE *object_programs, int tr_start_address, int tr_length, int CSLTH);