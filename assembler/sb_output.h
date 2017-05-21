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

auto parse_operand16(const std::string& s, enum OperandType *optype = NULL) -> uint16_t
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
		if(optype != NULL) *optype = OT_REGISTER;
	}else if(isdigit(s[0]) || s[0] == '-'){
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
		if(optype != NULL) *optype = OT_IMM16;
	}else{
		throw std::runtime_error("invalid operand \""+s+"\"");
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
	const Opcode& opcode;
	// op1, op2 .. the operands may be resolved lazily
	std::string op1, op2;
	// len .. bytes that this output will need
	unsigned len;
	union{
		// op16_r[0], op16_r[1] or op32_r .. resolved operand(s)
		uint16_t op16_r[2];
		int32_t op32_r;
	};
	bool flag_resolved = false;
	int line;

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
	OutputTuple(int line, const Opcode& op, std::string o1 = std::string(), std::string o2 = std::string())
		: opcode(op), op1(std::move(o1)), op2(std::move(o2)), line(line)
	{
		// could be different for opcodes with less than 2 operands
		len = 5;
	}

	OutputTuple(const OutputTuple& ot)
		: opcode(ot.opcode), op1(ot.op1), op2(ot.op2), len(ot.len),
		  flag_resolved(ot.flag_resolved), line(ot.line)
	{
		if(opcode.operands == 2){
			op16_r[0] = ot.op16_r[0];
			op16_r[1] = ot.op16_r[1];
		}else{
			op32_r = ot.op32_r;
		}
	}
#if 0
	OutputTuple(OutputTuple&& ot)
		: opcode(ot.opcode), op1(ot.op1), op2(ot.op2), len(ot.len), flag_resolved(ot.flag_resolved)
	{
		if(opcode.operands == 2){
			op16_r[0] = ot.op16_r[0];
			op16_r[1] = ot.op16_r[1];
		}else{
			op32_r = ot.op32_r;
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
		if(opcode.operands != 0){
			try{
				if(opcode.operands == 1 && opcode.optype1 == OT_IMM32){
					op32_r = parse_operand32(op1, get_label_addr);
				}else{
					enum OperandType ot;
					if(op1.size()){
						op16_r[0] = parse_operand16(op1, &ot);
						if(ot != opcode.optype1){
							warning(line,"first",opcode.optype1,ot);
						}
					}
					if(op2.size()){
						op16_r[1] = parse_operand16(op2, &ot);
						if(ot != opcode.optype1){
							warning(line,"second",opcode.optype2,ot);
						}
					}
				}
				// if the above parse functions didn't throw
				// flag as resolved
				flag_resolved = true;
			}catch(std::out_of_range& oor){
				// this is okay, label is not yet known so don't
				// flag as resolved
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
		return op16_r[0];
	}
	auto get_operand2() const -> uint16_t
	{
		return op16_r[1];
	}
	auto get_operand32() const -> int32_t
	{
		return op32_r;
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
	return put_little_endian(put_little_endian(o, static_cast<uint16_t>(x & 0xFFFF)),
			static_cast<uint16_t>((x >> 16) & 0xFFFF));
}
template<typename S>
inline auto put_little_endian(S& o, float x) -> S&
{
	/* hack... */
	union{
		float y;
		uint8_t buf[4];
	} u;
	u.y = x;
	for(uint8_t x : u.buf){
		o.put(x);
	}
	return o;
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

	auto add_opcode(int line, const Opcode& op, const std::vector<std::string>& tok) -> void
	{
		switch(op.operands){
		case 0:
			buffer.emplace_back(line, op);
			break;
		case 1:
			buffer.emplace_back(line, op, tok[1]);
			break;
		case 2:
			buffer.emplace_back(line, op, tok[1], tok[2]);
			break;
		default:
			std::cout << "operands = " << op.operands << std::endl;
			assert(0);
			break;
		}
		
		// operands parameter would enable using different
		// opcode sizes here
		addr += 5;
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

	auto add_char(int line, const std::vector<std::string>& tok) -> void
	{
		for(auto it = tok.cbegin()+1; it != tok.cend(); it++){
			const auto& x = *it;
			if(x[0] == '"'){
				size_t size = x.size()-1;
				if(x.back() != '"'){
					std::cerr << "warning: string literal not closed (line " << 
					          line << ")" << std::endl;
				}
				for(size_t i = 1; i < size; i++){
					out.put(x[i]);
				}
			}else{
				char *end_ptr;
				out.put(static_cast<uint8_t>(strtol(x.c_str(), &end_ptr, 10)));
			}
		}
	}

	auto add_int(int line, const std::vector<std::string>& tok) -> void
	{
		for_each(tok.cbegin()+1, tok.cend(), [this](const std::string& x){
			char *end_ptr;
			put_little_endian(this->out, static_cast<int32_t>(strtol(x.c_str(), &end_ptr, 10)));
			this->addr += 4;
		});
	}

	auto add_float(int line, const std::vector<std::string>& tok) -> void
	{
		for_each(tok.cbegin()+1, tok.cend(), [this](const std::string& x){
			char* end_ptr;
			put_little_endian(this->out, strtof(x.c_str(), &end_ptr));
			this->addr += 4;
		});
	}
};

