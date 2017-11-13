#pragma once

#include "sb_opcodes.h"
#include "asmexcept.h"

#include <algorithm>
#include <iostream>
#include <deque>
#include <map>
#include <memory>
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

	static auto warning(const AssemblyFile&, const char *t, OperandType expected, OperandType actual) -> void;

public:
	const int line;
	std::shared_ptr<const std::string> path;

	OutputTuple(const AssemblyFile&, const Opcode *const op, std::vector<std::string> op_, const std::string&);
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
	std::string most_recent_label; 
	// this is the most recent top-level label (a label
	// that doesn't start with a dot)

	int32_t addr = 0;
public:
	explicit Output(std::ostream& o); 

	auto write() -> bool;
	auto force_write() -> void;
	auto add_opcode(const AssemblyFile&, const Opcode& op, std::vector<std::string> tok) -> void;
	auto add_label(const AssemblyFile&, std::string s) -> void;
	auto add_char(const AssemblyFile&, const char *s) -> void;
	auto add_int(const AssemblyFile&, const std::vector<std::string>& tok) -> void;
	auto add_float(const AssemblyFile&, const std::vector<std::string>& tok) -> void;
};

