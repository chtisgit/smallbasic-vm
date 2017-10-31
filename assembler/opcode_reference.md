# Opcode reference for sbas & sbvm;;;;;;
;;;;;;
When operand 1 is a register, it is usually the destination (and also a source).;;;;;;
When operand 2 is a register, it is usually not the destination (not written).;;;;;;
CF is the condition flag.;;;;;;
;;;;;;
Possible Opcodes:
opcode | dst | src | src
 ----- | ----- | ----- | ----- 
op;imm16/32;[imm16]
op;dst;[src];[src]


Mnemonic | Opcode (dec) | Opcode (hex) |dst| operand 1 | operand 2 | Description
-------- | ------------ | ------------ | ------------ | --------- | --------- | -----------
copy    |0|0|reg| reg |-| copies value from operand 1 into srcreg
movi    |1|1|reg| imm16 |-| copies signed 16-bit immediate (integer) into srcreg
jmp     |2|2| imm32 |-| - | jumps to byte address given in operand 1
push    |3|3|reg|-| - | pushes value of dst 1 onto the stack
pushi   |4|4| imm32 |-| - | pushes signed 32-bit immediate (integer) onto the stack
add     |5|5|reg| reg | reg | adds value of operand 1 to that of operand 2 and saves the result in dst
sub     |6|6|reg| reg | reg | subtracts value of operand 2 from that of operand 1 and saves the result in dst
mul     |7|7|reg| reg | reg | multiplies value of operand 1 with that of operand 2 and saves the result in dst
div     |8|8|reg| reg | reg | divides value of operand 1 by that of operand 2 and saves the result in dst
addi    |9|9|reg| reg | imm16 | adds 16-bit signed immediate (integer) value of operand 2 to that of operand 1 and saves result in dst
pop     |10| 0A |reg|-| - | takes value off the stack and places it in dst
call    |11| 0B |imm32|-| - | pushes return address then unconditionally jumps to byte address given in dst
ret     |12| 0C |imm16|-| - | takes return address from the stack, pops the stack n times (operand 1), then jumps to the address previously taken from the stack
jc      |13| 0D |imm32|-| - | jump to address from dst if CF is set, then reset CF
jnc     |14| 0E |imm32|-| - | jump to address from dst if CF is not set, then reset CF
lstr    |15| 0F |reg|-| - | pops address from the stack from which it loads a NUL-terminated string into dst
equ     |16|10|reg| reg |-| if dst == operand 1 then set CF
neq     |17|11|reg| reg |-| if dst != operand 1 then set CF
grt     |18|12|reg| reg |-| if dst > operand 1 then set CF
lrt     |19|13|reg| reg |-| if dst < operand 1 then set CF
geq     |20|14|reg| reg |-| if dst >= operand 1 then set CF
leq     |21|15|reg| reg |-| if dst <= operand 1 then set CF
ccat    |22|16|reg| reg | reg | concatenate strings in operand 1 and operand2 and place result in dst
len     |23|17|reg| reg |-| save the length of string/array in operand 1 into dst
slice   |24|18|reg| reg | reg | cuts the string given in operand 1 to a length given in operand 2 (From the beginning). If operand 2 valuen is negative, the string gets cut from the end. Resultstring is saved in dst
read    |25|19|reg| reg | reg |saves value of array operand 1 at index operand 2 in dst
write   |26| 1A |reg| reg | reg |writes value of operand 2 into array of dst at index operand 1
dim     |27| 1B |reg| reg |-| create an n element (operand 1) array in dst. If dst already holds an array, resize it
zero    |28| 1C |reg| reg |-| zero dst and operand 1
estr    |29| 1D |reg| reg |-| load empty strings into dst and operand 1
obj     |30| 1E |imm16| imm16 |-| make a call to a native object function (specific to smallbasic)
reg\_debug |254| FE |reg| reg |-| show numeric values of dst and operand 1
str\_debug |255| FF |reg| reg | - | show string in dst
