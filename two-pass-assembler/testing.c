#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void update_text_record_length(FILE *temp_text_record, int text_record_length)
{
    // Text record length is in bytes, two hex digits make a byte.
    // But each character stored in a file is takes up a byte.

    // Therefore we have to multiply text_record_length by 2 to get to
    // the column where length is stored.

    // File points to the place after the '\n' character and therefore
    // Minus one gets to '\n', and minus 2 gets to the last character of the text record.
    // Think of it as indexing starting from 2, instead of 0,

    // Then we have to index to the Nth character if indexed from 0,
    // But here index is from 2, so we have to index to (N+2) th character

    // Example, if the record length (in characters) 60, then we have to index to 62nd
    // Character from last (since 2 bytes is for text-record-length columns)

    // Therefore we have to index to -(60 + 2 + 2)th index.
    fseek(temp_text_record, -2 * (text_record_length)-2 - 2, SEEK_CUR);
    fprintf(temp_text_record, "%02x", text_record_length);

    // Go back to the end of the file.
    fseek(temp_text_record, 0, SEEK_END);

    return;
}

int main()
{
    FILE *temptr = fopen("Temp_text_record.txt", "w+");
    fprintf(temptr, "T0000000017202d69202d4b1010360320262900003320074b10105b3f2fec0320100f20160100030f200d4b10105b3e2003454f46b410b400b44075101000e32017332ffadb2011a004\n");
    update_text_record_length(temptr, 138 / 2);
}
