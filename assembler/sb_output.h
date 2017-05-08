#pragma once

#include "sb_opcodes.h"

#include <iostream>
#include <deque>
#include <map>

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstdint>

auto parse_operand16(const std::string& s) -> uint16_t
{
	uint16_t rv;
	if(s[0] == '$'){
		// register
		char *endptr;
		long v = strtol(s.c_str()+1, &endptr, 10);
		if(*endptr != '\0' || v < 0 || v >= 65536){
			throw std::runtime_error(s+" no such register");
		}
		rv = v;
	}else if(isdigit(s[0])){
		// immediate 16 bit value 
		char *endptr;
		long v = strtol(s.c_str(), &endptr, 10);
		if(*endptr != '\0'){
			throw std::runtime_error("invalid operand \""+s+"\"");
		}else if(v < -32768 || v > 32767){
			throw std::runtime_error(s+" is out of range for 16 bit immediate");
		}
		if(v >= 0){
			rv = v;
		}else{
			rv = 65536+v;
		}
	}else{
		throw std::runtime_error("invalid operand \""+s+"\"");
	}
	return rv;
}

// resolving labels is currently only implemented for op1
// and it resolves into op_r
// this only supports labels in mnemonics with 1 operand
template<typename Func>
auto parse_operand32(const std::string& s, Func get_label_addr) -> int32_t
{
	int32_t rv;
	if(isdigit(s[0])){
		// immediate 32 bit value 
		char *endptr;
		long v = strtol(s.c_str(), &endptr, 10);
		if(*endptr != '\0' || v < INT32_MIN || v > INT32_MAX){
			throw std::runtime_error(s+" is out of range for 32 bit immediate");
		}
		rv = static_cast<uint16_t>(v);
	}else if(isalpha(s[0]) || s[0] == '_'){
		try{
			rv = get_label_addr(s);
		}catch(std::out_of_range& err){
			throw std::out_of_range("label not yet known \""+s+"\"");
		}
	}else{
		throw std::runtime_error("invalid operand \""+s+"\"");
	}
	return rv;

}

class OutputTuple{
	const Opcode& opcode;
	// op1, op2 .. the operands may be resolved lazily
	std::string op1, op2;
	// len .. bytes that this output will need
	unsigned len;
	union{
		// op1_r, op2_r or op_r .. resolved operand(s)
		uint16_t op1_r, op2_r;
		int32_t op_r;
	};
	bool flag_resolved = false;

public:
	OutputTuple(const Opcode& op, std::string o1, std::string o2 = std::string())
		: opcode(op), op1(std::move(o1)), op2(std::move(o2))
	{
		// could be different for opcodes with less than 2 operands
		len = 5;
	}

	OutputTuple(const OutputTuple& ot)
		: opcode(ot.opcode), op1(ot.op1), op2(ot.op2), len(ot.len), flag_resolved(ot.flag_resolved)
	{
		if(opcode.operands == 2){
			op1_r = ot.op1_r;
			op2_r = ot.op2_r;
		}else{
			op_r = ot.op_r;
		}
	}
#if 0
	OutputTuple(OutputTuple&& ot)
		: opcode(ot.opcode), op1(ot.op1), op2(ot.op2), len(ot.len), flag_resolved(ot.flag_resolved)
	{
		if(opcode.operands == 2){
			op1_r = ot.op1_r;
			op2_r = ot.op2_r;
		}else{
			op_r = ot.op_r;
		}
	}
#endif
	auto operator=(OutputTuple ot) -> OutputTuple&
	{
		std::swap(*this, ot);
		return *this;
	}

	auto size() const -> size_t
	{
		return len;
	}

	template<typename Func>
	auto resolve(Func get_label_addr) -> bool
	{
		flag_resolved = true;
		if(opcode.operands != 0){
			try{
				if(opcode.operands == 1){
					op_r = parse_operand32(op1, get_label_addr);
				}else{
					
					op1_r = parse_operand16(op1);
					op2_r = parse_operand16(op1);
				}
			}catch(std::out_of_range& ia){
				// this is not a fatal error!
				std::cerr << ia.what() << std::endl; // debug
				flag_resolved = false;
			}catch(std::runtime_error& err){
				// this is
				throw;
			}
		}
		return flag_resolved;
	}

	auto needed_label() const -> const std::string&
	{
		return op1;
	}

	auto resolved() const -> bool
	{
		return flag_resolved;
	}

	auto operands() const -> unsigned
	{
		return opcode.operands;
	}

	auto get_opcode() const -> uint8_t
	{
		return opcode.code;
	}
	auto get_operand1() const -> uint16_t
	{
		return op1_r;
	}
	auto get_operand2() const -> uint16_t
	{
		return op2_r;
	}
	auto get_operand32() const -> int32_t
	{
		return op_r;
	}
};

template<typename S>
inline auto put_little_endian(S& o, uint16_t x) -> S&
{
	o.put(x & 0xFF);
	o.put((x >> 8) & 0xFF);
	return o;
}
template<typename S>
inline auto put_little_endian(S& o, int32_t x) -> S&
{
	return put_little_endian(put_little_endian(o, x & 0xFFFF), (x >> 16) & 0xFFFF);
}

inline auto operator<<(std::ostream& o, OutputTuple& ot) -> std::ostream&
{
	o.put(ot.get_opcode());
	switch(ot.operands()){
	case 0:
		// write garbage
	case 1:
		put_little_endian(o, ot.get_operand32());
		break;
	case 2:
		put_little_endian(o, ot.get_operand1());
		put_little_endian(o, ot.get_operand2());
		break;
	default:
		assert(0);
		break;
	}
	return o;
}

class Output{
	std::ostream& out;
	std::deque<OutputTuple> buffer;
	std::map<std::string, int32_t> labels;

	int32_t addr = 0;
public:
	explicit Output(std::ostream& o) : out(o)
	{
	}

	auto write() -> bool
	{
		auto end = buffer.end();
		auto it = buffer.begin();
		auto resolver = [this](const std::string& s) -> int32_t{
				// this lambda must throw out_of_range if the label
				// can't be resolved
				return this->labels.at(s);
			};
		for(; it != end; it++){
			auto& ot = *it;
			if(!ot.resolve(resolver)){
				break;	
			}
			out << ot;
		}
		buffer.erase(buffer.begin(), it);
		return buffer.empty();
	}

	auto force_write() -> void
	{
		if(!write()){
			const std::string& label = buffer.back().needed_label();
			throw std::runtime_error("label could not be resolved \""+label+"\"");
		}
	}

	auto add_opcode(const Opcode& op, const std::string& o1, const std::string& o2) -> void
	{
		OutputTuple ot(op, o1, o2);
		

		// operands parameter would enable using different
		// opcode sizes here
		addr += 5;
	}
};

