#!/usr/bin/python
from random import randint

mn = ["copy", "movi", "jmp", "push", "pushi", "add", "sub", "mul", "div", "addi", "pop"]

max_op = 10
two_op = [0, 1, 5, 6, 7, 8, 9]
one_op = [2, 3, 4, 10]
no_op = []

first_imm32 = [2, 4]
first_reg = [0, 1, 3, 5, 6, 7, 8, 9, 10]
second_imm16 = [1, 9]
second_reg = [0, 5, 6, 7, 8]

labels_defd = []
labels_used = []
label_no = 0

def new_label():
	global label_no
	l = "L"+str(label_no)
	label_no += 1
	return l

for i in range(0, 400000):
	if randint(0,9) == 1:
		if len(labels_used) > 0:
			j = randint(0, len(labels_used)-1)
			print labels_used[j]+":"
			del labels_used[j]
		else:
			print new_label()+":"
	
	op = randint(0, max_op)

	op1 = ""
	op2 = ""

	if op in two_op:
		assert op in first_reg
		if op in first_reg:
			op1 = "$"+str(randint(0, 65535))

		if op in second_imm16:
			op2 = ""+str(randint(-30000,30000))
		elif op in first_reg:
			op2 = "$"+str(randint(0, 65535))

	elif op in one_op:
		if op in first_reg:
			op1 = "$"+str(randint(0, 65535))
		elif op in first_imm32:
			if len(labels_defd) > 0 and randint(0,10) < 5:
				# use existing label if any
				op1 = labels_defd[randint(0,len(labels_defd)-1)]
			else:
				op1 = new_label()
				labels_used.append(op1)
		
	else:
		assert 0

	print mn[op]+" "+op1+" "+op2

for l in labels_used:
	print l+":"


	
#	{"copy", {0, 2, OT_REGISTER, OT_REGISTER}},
#	{"movi", {1, 2, OT_REGISTER, OT_IMM16}},
#	{"jmp", {2, 1, OT_IMM32, OT_NO}},
#	{"push", {3, 1, OT_REGISTER, OT_NO}},
#	{"pushi", {4, 1, OT_IMM32, OT_NO}},
#	{"add", {5, 2, OT_REGISTER, OT_REGISTER}},
#	{"sub", {6, 2, OT_REGISTER, OT_REGISTER}},
#	{"mul", {7, 2, OT_REGISTER, OT_REGISTER}},
#	{"div", {8, 2, OT_REGISTER, OT_REGISTER}},
#	{"addi", {9, 2, OT_REGISTER, OT_IMM16}},
#	{"pop", {10, 1, OT_REGISTER, OT_NO}}


