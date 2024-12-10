#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ERROR_VALUE -1;

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
