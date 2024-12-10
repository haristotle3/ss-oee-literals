#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ERROR_VALUE -1
#define MAX_BUF 512
#define MEMORY_SIZE 16384
#define WORD_SIZE 6

char memory_array[MEMORY_SIZE];

int ll_pass_two(FILE *object_programs, int PROGADDR);
void move_objcode_to_memory(FILE *object_programs, int tr_start_address, int tr_length, int CSLTH);
int search_symbol(char *symbol_to_search);
void modify_address(int address_to_modify, int half_bytes, char operator, char * symbol);
void print_memory(int PROGADDR, int total_length);

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
    int total_length = 0;
    FILE *memory_state = fopen("memory_state.txt", "w");
    fclose(memory_state);
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

    typedef struct
    {
        int start_address;
        short int length;
        char operator;
        char symbol[MAX_BUF];
    } modification_record;

    header_record hr;
    text_record tr;
    modification_record mr;
    char current_record_type;
    char input_line[MAX_BUF];

    while (fscanf(object_programs, "%c%6s%6x%6x\n", &hr.record_type, hr.csect_name, &hr.csect_address, &hr.CSLTH) > 0)
    {
        current_record_type = hr.record_type;
        int end_record_address = PROGADDR;
        if (current_record_type == 'R')
        {
            fscanf(object_programs, "%s\n", &input_line);
            continue;
        }
        if (current_record_type == '.')
        {
            fscanf(object_programs, "%s\n", &input_line);
            continue;
        }
        while (current_record_type != 'E')
        {

            fscanf(object_programs, "%c", &current_record_type);

            if (current_record_type == 'R')
            {
                fscanf(object_programs, "%s\n", &input_line);
                continue;
            }
            if (current_record_type == '.')
            {
                fscanf(object_programs, "%s\n", &input_line);
                continue;
            }
            if (current_record_type == 'T')
            {
                fscanf(object_programs, "%6x%2x", &tr.start_address, &tr.length);
                move_objcode_to_memory(object_programs, tr.start_address, tr.length, CSADDR);
            }

            if (current_record_type == 'M')
            {
                printf("I reached till here.");
                fscanf(object_programs, "%6x%2x%c%s\n", &mr.start_address, &mr.length, &mr.operator, mr.symbol);
                int symbol_value = search_symbol(mr.symbol);
                if (symbol_value == ERROR_VALUE)
                {
                    printf("ERROR: undefined external symbol (%s).\n", mr.symbol);
                    return ERROR_VALUE;
                }
                else // found
                    modify_address(CSADDR + mr.start_address, mr.length, mr.operator, mr.symbol);
            }
        }

        if (!feof(object_programs))
        {
            fscanf(object_programs, "%x", &end_record_address);
            EXECADDR = CSADDR + end_record_address;
        }

        CSADDR += hr.CSLTH;
        total_length += hr.CSLTH;
    }

    print_memory(PROGADDR, total_length);
    return 1;
}

void move_objcode_to_memory(FILE *object_programs, int tr_start_address, int tr_length, int CSADDR)
{
    int current_mem_address = CSADDR + tr_start_address;
    char obj_instruction;

    for (int i = 0; i < tr_length; i++)
    {
        fscanf(object_programs, "%2x", &obj_instruction);
        memory_array[current_mem_address + i] = obj_instruction;
    }

    return;
}

// void move_objcode_to_memory(FILE *object_programs, int tr_start_address, int tr_length, int CSADDR)
// {
//     FILE *memory_state = fopen("memory_state.txt", "a");
//     short int obj_instruction;
//     int current_mem_address = CSADDR + tr_start_address;

//     for (int i = 0; i < tr_length; i++)
//     {
//         if (i % 4 == 0)
//             fprintf(memory_state, "\n%8x ", current_mem_address);

//         fscanf(object_programs, "%2x", &obj_instruction);
//         fprintf(memory_state, "%2x", obj_instruction);
//         current_mem_address++;
//     }

//     fclose(memory_state);
//     return;
// }

int search_symbol(char *symbol_to_search)
{
    // Search symbol, and return its address;

    FILE *ESTAB = fopen("ESTAB.txt", "r");

    char dummy[MAX_BUF];
    char current_symbol[MAX_BUF];
    int symbol_address;
    // | CSECT_NAME | SYMBOL_NAME | ADDRESS | LENGTH |

    while (fscanf(ESTAB, "%10s%10s%10x%10s\n", dummy, current_symbol, &symbol_address, dummy) > 0)
    {
        printf("CS (%s), SS (%s)\n", current_symbol, symbol_to_search);
        if (strcmp(current_symbol, symbol_to_search) == 0)
            return symbol_address;
    }

    fclose(ESTAB);
    return ERROR_VALUE;
}

void modify_address(int address_to_modify, int half_bytes, char operator, char * symbol)
{
    int symbol_value = search_symbol(symbol);
    int modified_value = 0x0;

    int i;
    for (i = 0; i < (half_bytes / 2 + half_bytes % 2); i++)
    {
        modified_value <<= 8;
        modified_value += memory_array[address_to_modify + i];
    }

    modified_value += symbol_value;

    for (; i >= 0; i--)
    {
        int last_byte = modified_value & 0xFF;
        memory_array[address_to_modify + i] = last_byte;
        modified_value >>= 8;
    }

    return;
}

void print_memory(int PROGADDR, int total_length)
{
    FILE *memory_state = fopen("memory_state.txt", "a");
    short int obj_instruction;
    int current_mem_address = PROGADDR;

    for (int i = 0; i < total_length; i++)
    {
        if (i % 4 == 0)
            fprintf(memory_state, "\n%8x ", current_mem_address + i);

        obj_instruction = memory_array[current_mem_address + i];
        fprintf(memory_state, "%2x", obj_instruction);
    }

    fclose(memory_state);
    return;
}