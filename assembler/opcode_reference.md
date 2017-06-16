# Opcode reference for sbas & sbvm

When operand 1 is a register, it is usually the destination (and also a source).
When operand 2 is a register, it is usually not the destination (not written).
CF is the condition flag.

Mnemonic | Opcode (dec) | Opcode (hex) | operand 1 | operand 2 | Description
-------- | ------------ | ------------ | --------- | --------- | -----------
copy    | 0 | 00 | reg | reg | copies value from operand 2 into operand 1
movi    | 1 | 01 | reg | imm16 | copies signed 16-bit immediate (integer) into operand 1
jmp     | 2 | 02 | imm32 | - | jumps to byte address given in operand 1
push    | 3 | 03 | reg | - | pushes value of operand 1 onto the stack
pushi   | 4 | 04 | imm32 | - | pushes signed 32-bit immediate (integer) onto the stack
add     | 5 | 05 | reg | reg | adds value of operand 1 to that of operand 2 and saves the result in operand 1
sub     | 6 | 06 | reg | reg | subtracts value of operand 2 from that of operand 1 and saves the result in operand 1
mul     | 7 | 07 | reg | reg | multiplies value of operand 1 with that of operand 2 and saves the result in operand 1
div     | 8 | 08 | reg | reg | divides value of operand 1 by that of operand 2 and saves the result in operand 1
addi    | 9 | 09 | reg | imm16 | adds 16-bit signed immediate (integer) value of operand 2 to that of operand 1 and saves the result in operand 1
pop     | 10 | 0A | reg | - | takes value off the stack and places it in operand 1
call    | 11 | 0B | imm32 | - | pushes return address then unconditionally jumps to byte address given in operand 1
ret     | 12 | 0C | imm16 | - | takes return address from the stack, pops the stack n times (operand 1), then jumps to the return address
jc      | 13 | 0D | imm32 | - | jump if CF is set, then reset CF
jnc     | 14 | 0E | imm32 | - | jump if CF is not set, then reset CF
lstr    | 15 | 0F | reg | - | pops address from the stack from which it loads a NUL-terminated string into operand 1
equ     | 16 | 10 | reg | reg | if operand 1 == operand 2 then set CF
neq     | 17 | 11 | reg | reg | if operand 1 != operand 2 then set CF
grt     | 18 | 12 | reg | reg | if operand 1 > operand 2 then set CF
lrt     | 19 | 13 | reg | reg | if operand 1 < operand 2 then set CF
geq     | 20 | 14 | reg | reg | if operand 1 >= operand 2 then set CF
leq     | 21 | 15 | reg | reg | if operand 1 <= operand 2 then set CF
ccat    | 22 | 16 | reg | reg | concatenate strings in operand 1 and operand2 and place result in operand 1
len     | 23 | 17 | reg | reg | save the length of string/array in operand 2 into operand 1
slice   | 24 | 18 | reg | reg | cuts the string given in operand 1 to a length given in operand 2 (From the beginning). If operand 2 is negative, it is cut backwards (from the end). The result is placed in operand 1.
read    | 25 | 19 | reg | reg | takes an index off the stack, reads the contents of the string/array in operand 2 at that index and stores them into operand 1
write   | 26 | 1A | reg | reg | takes an index off the stack, writes the value given in operand 2 into the string/array given in operand 1 at the given index.
dim     | 27 | 1B | reg | reg | create an n element (operand 2) array in operand 1. If operand 1 already holds an array, resize it.
zero    | 28 | 1C | reg | reg | zero the two operands
estr    | 29 | 1D | reg | reg | load empty strings into both operands
obj     | 128 | 80 | imm16 | imm16 | make a call to a native object function (specific to smallbasic)
reg\_debug | 254 | FE | reg | reg | show numeric values of both operands
str\_debug | 255 | FF | reg | - | show string in operand 1

