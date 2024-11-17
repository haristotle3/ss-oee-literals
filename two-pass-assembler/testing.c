#include <stdio.h>
#include <math.h>

int get_object_code_length(unsigned long long int assembled_object_code)
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

    return (int)ceil(log2(assembled_object_code) / 4.0 / 2.0);
}
int main()
{
    printf("0x17202d = \t%d\n", get_object_code_length(0x17202d));
    printf("0xf2016 = \t%d\n", get_object_code_length(0xf2016));
    printf("0x10003 = \t%d\n", get_object_code_length(0x10003));
    printf("0x4b10105b = \t%d\n", get_object_code_length(0x4b10105b));

    printf("%0*llx\n", 2 * get_object_code_length(0x17202d), 0x17202d);
    printf("%0*llx\n", 2 * get_object_code_length(0xf2016), 0xf2016);
    printf("%0*llx\n", 2 * get_object_code_length(0x10003), 0x10003);
    printf("%0*llx\n", 2 * get_object_code_length(0x4b10105b), 0x4b10105b);
}
