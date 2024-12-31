# SIC/XE two pass assembler.

Fully working two-pass-assembler for the hypothetical SIC/XE computer

# Important note:

- End record in object program contains the _same address as provided in the first line of the input assembly program_. This is actually incorrect, since end record should have the address of the first _executable_ instruction. Future revisions to the code could fix that feature.
- These C programs works properly only on a windows machine. For linux or other machines, replace the <windows.h> header file and CreateDirectory() function with their equivalent header file and function.

## Notes

- To execute pass 1 execute the following command:
  `gcc -o pass-1.exe pass-1.c utils.c && ./pass-1.exe`

- Linux requires explicit linking of `math.h` library. Therefore,
  `gcc -o pass-1.exe pass-1.c utils.c -lm`

- Use the `-lm` argument to execute pass 2 as well.

`input.txt` contains the assembly program whose object code is to be generated.

Program must be written in 3 columns/fields `LABEL    MNEMONIC   OPERAND`.
Comments should have a `.` in the `LABEL` field. Both `MNEMONIC` and `OPERAND` field _must_ have atleast one character in the comment line, else assembler will cause unexpected behaviour.
Empty fields must be filled by the string `"____"`.

`OPTAB.txt` contains mnemonics and their opcodes. It also has three fields `MNEMONIC    FORMAT  OPCODE`. Any changes to the OPTAB _must_ maintain this structure. `FORMAT` refers to the 4 instruction formats in SIC/XE (of 1 byte, 2 bytes, 3 bytes and 4 bytes)

- To execute pass 2 execute the following command:
  `gcc -o pass-2.exe pass-2.c utils.c && ./pass-2.exe`

- Both pass-1 and pass-2 can be executed in one command:

`gcc -o pass-1.exe pass-1.c utils.c && ./pass-1.exe && gcc -o pass-2.exe pass-2.c utils.c && ./pass-2.exe`

There is a Linux-specific issue because the math library (libm) isn't linked by default. Use the following command to compile successfully:
`gcc -o pass-2.exe pass-2.c utils.c -lm && ./pass-2.exe`

`input.txt`, `OPTAB.txt` are written in this directory for reference.

Pass 1 produces `intermediate.txt`, `SYMTAB.txt` and `program_length.txt` as outputs.
Pass 2 produces `object_program.txt` and `program_listing.txt` as outputs.

To remove all output files run the command:
`rm -rf pass-1-outputs/*.* pass-2-outputs/*.* && rmdir pass-1-outputs pass-2-outputs`

If the output directories do not exist, `rmdir` will show an error, obviously.

To remove just the executable files run the command:
`rm -rf pass-1.exe pass-2.exe`
