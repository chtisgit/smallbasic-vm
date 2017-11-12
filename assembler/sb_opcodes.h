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

	Opcode(uint8_t c, unsigned o, unsigned s, std::array<OperandType,MAX_OPERANDS> optype_);
	Opcode(const Opcode&) = default;
};

extern std::map<std::string, Opcode> mnemonic_table;

