#pragma once

#include "sb_opcodes.h"
#include "asmexcept.h"

#include <algorithm>
#include <iostream>
#include <deque>
#include <map>
#include <vector>
#include <functional>

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstdint>
#include <cstring>

using LabelDeducer = std::function<int32_t(const std::string&)>;

auto parse_register(const std::string& s) -> uint8_t;
auto parse_operand8(const std::string& s) -> uint8_t;
auto parse_operand16(const std::string& s) -> uint16_t;
auto parse_operand32(const std::string& s, LabelDeducer get_label_addr) -> int32_t;

struct Label{
	int line;
	int32_t addr;
};

class OutputTuple{
	const Opcode *const opcode;
	// op1, op2 .. the operands may be resolved lazily
	std::vector<std::string> op;
	// len .. bytes that this output will need
	unsigned len;
	// op8_r[] or op32_r .. resolved operand(s)
	uint16_t op8_r[3];
	int32_t op32_r;
	std::vector<uint8_t> towrite;

	static auto warning(int line, const char *t, OperandType expected, OperandType actual) -> void;

public:
	const int line;
	OutputTuple(int line, const Opcode *const op, std::vector<std::string> op_);
	OutputTuple(int line, std::vector<uint8_t>&& vec);

	auto operator=(OutputTuple ot) -> OutputTuple&;
	auto size() const -> size_t;
	auto write(std::ostream& o) const -> std::ostream&;
	auto needed_label() const -> const std::string&;
	auto resolved() const -> bool;
	auto resolve(LabelDeducer get_label_addr) -> bool;

};

class Output{
	std::ostream& out;
	std::deque<OutputTuple> buffer;
	std::map<std::string, Label> labels;

	int32_t addr = 0;
public:
	explicit Output(std::ostream& o); 

	auto write() -> bool;
	auto force_write() -> void;
	auto add_opcode(int line, const Opcode& op, std::vector<std::string> tok) -> void;
	auto add_label(int line, const std::string& s) -> void;
	auto add_char(int line, const char *s) -> void;
	auto add_int(int line, const std::vector<std::string>& tok) -> void;
	auto add_float(int line, const std::vector<std::string>& tok) -> void;
};

