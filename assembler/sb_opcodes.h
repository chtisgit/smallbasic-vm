#pragma once

#include <array>
#include <map>

#include <cassert>
#include <cstdint>

enum OperandType{
	OT_NO,          // no operand
	OT_REGISTER,
	OT_IMM8,
	OT_IMM16,
	OT_IMM32
};

struct Opcode{
	enum{ MAX_OPERANDS=3 };
	const uint8_t code;
	const unsigned operands;
	const unsigned size_in_bytes;
	const std::array<OperandType,MAX_OPERANDS> optype;

	Opcode(uint8_t c, unsigned o, unsigned s, std::array<OperandType,MAX_OPERANDS> optype_)
		: code(c), operands(o), size_in_bytes(s), optype(optype_)
	{
		// opcodes can have a minimum of zero and
		// a maximum of MAX_OPERANDS operands
		assert(o >= 0 && o <= MAX_OPERANDS);

		// simple checks
		assert(o != 0 || (optype[0] == OT_NO && optype[1] == OT_NO && optype[2] == OT_NO));
		assert(o != 1 || (optype[0] != OT_NO && optype[1] == OT_NO && optype[2] == OT_NO));
		assert(o != 2 || (optype[0] != OT_NO && optype[1] != OT_NO && optype[2] == OT_NO));
		assert(o != 3 || (optype[0] != OT_NO && optype[1] != OT_NO && optype[2] != OT_NO));

		// IMM32 values only in first operand 
		assert(optype[1] != OT_IMM32 && optype[2] != OT_IMM32);
	}

	Opcode(const Opcode&) = default;
};

std::map<std::string, Opcode> mnemonic_table = {
	{"copy", {0, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"movi", {1, 2, 4, {OT_REGISTER, OT_IMM16, OT_NO}}},
	{"jmp", {2, 1, 5, {OT_IMM32, OT_NO, OT_NO}}},
	{"push", {3, 1, 2, {OT_REGISTER, OT_NO, OT_NO}}},
	{"pushi", {4, 1, 5, {OT_IMM32, OT_NO, OT_NO}}},
	{"add", {5, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"sub", {6, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"mul", {7, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"div", {8, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"addi", {9, 2, 5, {OT_REGISTER, OT_IMM16, OT_NO}}},
	{"pop", {10, 1, 2, {OT_REGISTER, OT_NO, OT_NO}}},
	{"call", {11, 1, 5, {OT_IMM32, OT_NO, OT_NO}}},
	{"ret", {12, 1, 3, {OT_IMM16, OT_NO, OT_NO}}},
	{"jc", {13, 1, 5, {OT_IMM32, OT_NO, OT_NO}}},
	{"jnc", {14, 1, 5, {OT_IMM32, OT_NO, OT_NO}}},
	{"lstr", {15, 1, 2, {OT_REGISTER, OT_NO, OT_NO}}},
	{"equ", {16, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"neq", {17, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"grt", {18, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"lrt", {19, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"geq", {20, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"leq", {21, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"ccat", {22, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"len", {23, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"slice", {24, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"read", {25, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"write", {26, 3, 4, {OT_REGISTER, OT_REGISTER, OT_REGISTER}}},
	{"dim", {27, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"zero", {28, 1, 2, {OT_REGISTER, OT_NO, OT_NO}}},
	{"estr", {29, 1, 2, {OT_REGISTER, OT_NO, OT_NO}}},
	{"obj", {30, 2, 3, {OT_IMM8, OT_IMM8, OT_NO}}},
	{"reg_debug", {254, 2, 3, {OT_REGISTER, OT_REGISTER, OT_NO}}},
	{"str_debug", {255, 1, 2, {OT_REGISTER, OT_NO, OT_NO}}}
};


