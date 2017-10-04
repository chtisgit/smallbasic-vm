#pragma once

#include "sb_opcodes.h"

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

auto parse_register(const std::string& s) -> uint8_t
{
	if(s[0] != '$'){
		throw std::runtime_error("invalid operand \""+s+"\"");
	}
	// register
	char *endptr;
	long v = strtol(s.c_str()+1, &endptr, 10);
	if(*endptr != '\0' || v < 0 || v > 255){
		throw std::runtime_error("invalid operand \""+s+"\"");
	}
	return uint8_t(v);
}

auto parse_operand8(const std::string& s) -> uint8_t
{
	if(!isdigit(s[0]) && s[0] != '-'){
		throw std::runtime_error(s+" no such register");
	}
	// register
	char *endptr;
	long v = strtol(s.c_str()+1, &endptr, 10);
	if(*endptr != '\0' || v < 0 || v > 255){
		throw std::runtime_error(s+" no such register");
	}
	return uint8_t(v);
}


auto parse_operand16(const std::string& s) -> uint16_t
{
	uint16_t rv;
	if(!isdigit(s[0]) && s[0] != '-'){
		throw std::runtime_error("invalid operand \""+s+"\"");
	}
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
	return rv;
}

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
	int line;
	std::vector<uint8_t> towrite;

	static auto warning(int line, const char *t, OperandType expected, OperandType actual) -> void
	{
		if(expected != actual){
			if(expected == OT_REGISTER)
				std::cerr << "warning: " << t << " operand should be "
					"register (immediate given) in line " <<
					line << std::endl;
			else
				std::cerr << "warning: " << t << " operand should be "
					"immediate (register given) in line " << 
					line << std::endl;
		}
	}

public:
	OutputTuple(int line, const Opcode *const op, std::vector<std::string> op_)
		: opcode(op), op(std::move(op_)), line(line)
	{
		//std::cerr << "opcode=" << int(op->code) << " op1=" << op1 << " op2=" << op2 << std::endl;
		// could be different for opcodes with less than 2 operands
		len = op->size_in_bytes;
	}

	OutputTuple(std::vector<uint8_t>&& vec)
		: opcode(nullptr), towrite(vec)
	{
	}

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
				std::cerr << "towrite opcode=" << int(opcode->code) << std::endl;
				towrite.insert(towrite.end(), temp.begin(), temp.end());
			}catch(std::out_of_range& oor){
				// this is okay, label is not yet known so don't
				// flag as resolved
			}
		}
		return resolved();
	}

	auto write(std::ostream& o) const -> std::ostream&
	{
		std::for_each(towrite.cbegin(), towrite.cend(), [&o](const uint8_t& u){
			o << u;
		});
		return o;
	}

	auto needed_label() const -> const std::string&
	{
		return op[0];
	}

	auto resolved() const -> bool
	{
		return !towrite.empty();
	}
};

class Output{
	std::ostream& out;
	std::deque<OutputTuple> buffer;
	std::map<std::string, Label> labels;

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
				return this->labels.at(s).addr;
			};
		for(; it != end; it++){
			auto& ot = *it;
			if(!ot.resolve(resolver)){
				break;	
			}
			ot.write(out);
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

	auto add_opcode(int line, const Opcode& op, std::vector<std::string> tok) -> void
	{
		tok.erase(tok.cbegin());
		buffer.emplace_back(line, &op, tok);
		assert(op.operands <= 3);
		
		addr += op.size_in_bytes;
	}

	auto add_label(int line, const std::string& s) -> void
	{
		if(labels.find(s) != labels.end()){
			throw std::runtime_error("label '"+s+"' defined (at least) twice (lines "+
				std::to_string(labels[s].line)+" and "+std::to_string(line)+")");
		}
		labels[s] = Label{line, addr};

		if(!buffer.empty() && buffer.front().needed_label() == s){
			// if this label is used in the first command in the
			// buffer write everything we can
			write();
		}
	}

	auto add_char(int line, const char *s) -> void
	{
		std::vector<uint8_t> vec;
		const char *start;

		if((start = strstr(s, "char ")) == NULL){
			std::cerr << "warning: char command could not be parsed" << std::endl;
			return;
		}
		start += 5;

		while(*start != '\0'){
			if(isdigit(*start)){
				int num;
				char *endptr;
				num = strtol(start, &endptr, 10);
				vec.push_back(static_cast<uint8_t>(num));
				start = endptr;
			}else if(*start == '"'){
				start++;
				while(*start != '\0' && *start != '"'){
					vec.push_back(*start++);
				}
				if(*start == '\0'){
					std::cerr << "warning: char sequence not terminated!" << std::endl;
					break;
				}
				start++;
			}else{
				start++;
			}
		}

		if(!vec.empty()){
			buffer.emplace_back(std::move(vec));
		}
	}

	auto add_int(int line, const std::vector<std::string>& tok) -> void
	{
		std::vector<uint8_t> vec;
		std::for_each(tok.cbegin()+1, tok.cend(), [&vec,this](const std::string& x){
			char *end_ptr;
			int32_t value = strtol(x.c_str(), &end_ptr, 10);
			vec.push_back(value & 0xFF);
			vec.push_back((value >> 8) & 0xFF);
			vec.push_back((value >> 16) & 0xFF);
			vec.push_back((value >> 24) & 0xFF);
			this->addr += 4;
		});
		if(!vec.empty()){
			buffer.emplace_back(std::move(vec));
		}
	}

	auto add_float(int line, const std::vector<std::string>& tok) -> void
	{
		std::vector<uint8_t> vec;
		std::for_each(tok.cbegin()+1, tok.cend(), [&vec,this](const std::string& x){
			char* end_ptr;
			float value = strtof(x.c_str(), &end_ptr);
			uint32_t blub = *reinterpret_cast<int32_t*>(&value); // hack
			vec.push_back(blub & 0xFF);
			vec.push_back((blub >> 8) & 0xFF);
			vec.push_back((blub >> 16) & 0xFF);
			vec.push_back((blub >> 24) & 0xFF);
			this->addr += 4;
		});
		if(!vec.empty()){
			buffer.emplace_back(std::move(vec));
		}
	}
};

