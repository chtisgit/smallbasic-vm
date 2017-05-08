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
};

std::map<std::string, Opcode> mnemonic_table = {
	{"copy", {0, 2, OT_REGISTER, OT_REGISTER}},
	{"movi", {1, 2, OT_REGISTER, OT_IMM16}},
	{"jmp", {2, 1, OT_IMM32, OT_NO}},
	{"push", {3, 1, OT_REGISTER, OT_NO}},
	{"pushi", {4, 1, OT_IMM32, OT_NO}}
};


