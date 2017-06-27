#pragma once

#include <map>

#include <cassert>
#include <cstdint>

enum OperandType{
	OT_NO,          // no operand
	OT_REGISTER,
	OT_IMM16,
	OT_IMM32
};

struct Opcode{
	const uint8_t code;
	const unsigned operands;
	const OperandType optype1, optype2;

	Opcode(uint8_t c, unsigned o, OperandType o1, OperandType o2)
		: code(c), operands(o), optype1(o1), optype2(o2)
	{
		// opcodes can have a minimum of zero and
		// a maximum of 2 operands
		assert(o >= 0 && o <= 2);

		// simple checks
		assert(o != 0 || (o1 == OT_NO && o2 == OT_NO));
		assert(o != 1 || (o1 != OT_NO && o2 == OT_NO));
		assert(o != 2 || (o1 != OT_NO && o2 != OT_NO));

		// IMM32 values only in o1
		assert(o2 != OT_IMM32);
		assert(o1 != OT_IMM32 || o2 == OT_NO);
	}

	Opcode(const Opcode&) = default;
};

std::map<std::string, Opcode> mnemonic_table = {
	{"copy", {0, 2, OT_REGISTER, OT_REGISTER}},
	{"movi", {1, 2, OT_REGISTER, OT_IMM16}},
	{"jmp", {2, 1, OT_IMM32, OT_NO}},
	{"push", {3, 1, OT_REGISTER, OT_NO}},
	{"pushi", {4, 1, OT_IMM32, OT_NO}},
	{"add", {5, 2, OT_REGISTER, OT_REGISTER}},
	{"sub", {6, 2, OT_REGISTER, OT_REGISTER}},
	{"mul", {7, 2, OT_REGISTER, OT_REGISTER}},
	{"div", {8, 2, OT_REGISTER, OT_REGISTER}},
	{"addi", {9, 2, OT_REGISTER, OT_IMM16}},
	{"pop", {10, 1, OT_REGISTER, OT_NO}},
	{"call", {11, 1, OT_IMM32, OT_NO}},
	{"ret", {12, 1, OT_IMM16, OT_NO}},
	{"jc", {13, 1, OT_IMM32, OT_NO}},
	{"jnc", {14, 1, OT_IMM32, OT_NO}},
	{"lstr", {15, 1, OT_REGISTER, OT_NO}},
	{"equ", {16, 2, OT_REGISTER, OT_REGISTER}},
	{"neq", {17, 2, OT_REGISTER, OT_REGISTER}},
	{"grt", {18, 2, OT_REGISTER, OT_REGISTER}},
	{"lrt", {19, 2, OT_REGISTER, OT_REGISTER}},
	{"geq", {20, 2, OT_REGISTER, OT_REGISTER}},
	{"leq", {21, 2, OT_REGISTER, OT_REGISTER}},
	{"ccat", {22, 2, OT_REGISTER, OT_REGISTER}},
	{"len", {23, 2, OT_REGISTER, OT_REGISTER}},
	{"slice", {24, 2, OT_REGISTER, OT_REGISTER}},
	{"read", {25, 2, OT_REGISTER, OT_REGISTER}},
	{"write", {26, 2, OT_REGISTER, OT_REGISTER}},
	{"dim", {27, 2, OT_REGISTER, OT_REGISTER}},
	{"zero", {28, 2, OT_REGISTER, OT_REGISTER}},
	{"estr", {29, 2, OT_REGISTER, OT_REGISTER}},
	{"obj", {30, 2, OT_IMM16, OT_IMM16}},
	{"reg_debug", {254, 2, OT_REGISTER, OT_REGISTER}},
	{"str_debug", {255, 1, OT_REGISTER, OT_NO}}
	};


