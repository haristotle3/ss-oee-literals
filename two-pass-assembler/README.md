# SIC/XE two pass assembler.

Fully working two-pass-assembler for the hypothetical SIC/XE computer

# Important note:

- `RSUB` instruction is format 1 instruction in this assembler. (changed from format 3).

## Notes

To execute pass 1 execute the following command:
`gcc -o pass-1.exe pass-1.c utils.c && ./pass-1.exe`

`input.txt` contains the assembly program whose object code is to be generated.

Program must be written in 3 columns/fields `LABEL    MNEMONIC   OPERAND`
Comments should have a `.` in the `LABEL` field. Both `MNEMONIC` and `OPERAND` field MUST have atleast one character in the comment line, else assembler will cause unexpected behaviour
Empty fields must be filled by the string `"****"`.

`OPTAB.txt` contains mnemonics and their opcodes. It also has three fields `MNEMONIC    FORMAT  OPCODE`. Any changes to the OPTAB MUST maintain this structure. `FORMAT` refers to the 4 instruction formats in SIC/XE (of 1 byte, 2 bytes, 3 bytes and 4 bytes)

To execute pass 2 execute the following command:
`gcc -o pass-2.exe pass-2.c utils.c && ./pass-2.exe`

Both pass-1 and pass-2 can be executed in one command:
`gcc -o pass-1.exe pass-1.c utils.c && ./pass-1.exe && gcc -o pass-2.exe pass-2.c utils.c && ./pass-2.exe`

`input.txt`, `OPTAB.txt` are written in this directory for reference.

Assembler produces object_program and program_listing as outputs.
