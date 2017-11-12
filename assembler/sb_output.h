#pragma once

#include "sb_opcodes.h"
#include "asmexcept.h"

#include <algorithm>
#include <iostream>
#include <deque>
#include <map>
#include <vector>

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstdint>
#include <cstring>

auto parse_register(const std::string& s) -> uint8_t;
auto parse_operand8(const std::string& s) -> uint8_t;
auto parse_operand16(const std::string& s) -> uint16_t;

// resolving labels is currently only implemented for op1
// and it resolves into op32_r
// this only supports labels in mnemonics with 1 operand
template<typename Func>
auto parse_operand32(const std::string& s, Func get_label_addr) -> int32_t
{
	int32_t rv;
	if(isdigit(s[0]) || s[0] == '-'){
		// immediate 32 bit value 
		char *endptr;
		long v = strtol(s.c_str(), &endptr, 10);
		if(*endptr != '\0' || v < INT32_MIN || v > INT32_MAX){
			throw std::runtime_error(s+" is out of range for 32 bit immediate");
		}
		rv = static_cast<uint16_t>(v);
	}else if(isalpha(s[0]) || s[0] == '_'){
		rv = get_label_addr(s);
	}else{
		throw std::runtime_error("invalid operand \""+s+"\"");
	}
	return rv;

}

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

	template<typename Func>
	auto resolve(Func get_label_addr) -> bool
	{
		if(towrite.empty()){
			try{
				std::vector<uint8_t> temp;
				if(opcode->operands > 0){
					int op8_i = 0;
					for(int i = 0; i < opcode->operands; i++){
						switch(opcode->optype[i]){
						case OT_IMM32:
							op32_r = parse_operand32(op[i], get_label_addr);
							temp.push_back(op32_r & 0xFF);
							temp.push_back((op32_r >> 8) & 0xFF);
							temp.push_back((op32_r >> 16) & 0xFF);
							temp.push_back((op32_r >> 24) & 0xFF);
							break;
						case OT_IMM16:
							op32_r = parse_operand16(op[i]);
							temp.push_back(op32_r & 0xFF);
							temp.push_back((op32_r >> 8) & 0xFF);
							break;
						case OT_IMM8:
							op8_r[op8_i] = parse_operand8(op[i]);
							temp.push_back(op8_r[op8_i]);
							op8_i++;
							break;
						case OT_REGISTER:
							op8_r[op8_i] = parse_register(op[i]);
							temp.push_back(op8_r[op8_i]);
							op8_i++;
							break;
						default:
							break;
						}
					}
				}
				// if the above parse functions didn't throw
				// flag as resolved by populating towrite

				towrite.push_back(opcode->code);
				//std::cerr << "towrite opcode=" << int(opcode->code) << std::endl;
				towrite.insert(towrite.end(), temp.begin(), temp.end());
			}catch(std::out_of_range& oor){
				// this is okay, label is not yet known so don't
				// flag as resolved
			}catch(std::runtime_error& rue){
				// rethrow runtime_error's from this code as AssemblerError's
				throw AssemblerError(rue.what(),line);
			}
		}
		return resolved();
	}

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

