#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    FILE *object_programs = fopen("object_programs.txt", "r");
    FILE *load_address_file = fopen("load_address.txt", "r");

    int PROGADDR;
    fscanf(load_address_file, "%x", &PROGADDR);

    printf("Progaddr: %x\n", PROGADDR);
}