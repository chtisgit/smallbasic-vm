#include "sb_output.h"


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
	if(!isdigit(s[0]))
		throw std::runtime_error(s+" no such register");
	// register
	char *endptr;
	long v = strtol(s.c_str(), &endptr, 10);
	if(*endptr != '\0' || v < 0 || v > 255){
		throw std::runtime_error(s+" no such register");
	}
	return uint8_t(v);
}

auto parse_operand16(const std::string& s) -> uint16_t
{
	uint16_t rv;
	/*if(!isdigit(s[0]) && s[0] != '-'){
		throw std::runtime_error("invalid operand \""+s+"\"");
	}*/
	// immediate 16 bit value 
	char *endptr;
	long v = strtol(s.c_str(), &endptr, 0);
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


auto OutputTuple::warning(int line, const char *t, OperandType expected, OperandType actual) -> void
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


OutputTuple::OutputTuple(int line, const Opcode *const op, std::vector<std::string> op_)
	: opcode(op), op(std::move(op_)), line(line)
{
	//std::cerr << "opcode=" << int(op->code) << " op1=" << op1 << " op2=" << op2 << std::endl;
	// could be different for opcodes with less than 2 operands
	len = op->size_in_bytes;
}
OutputTuple::OutputTuple(int line, std::vector<uint8_t>&& vec)
	: opcode(nullptr), towrite(vec), line(line)
{
}
auto OutputTuple::operator=(OutputTuple ot) -> OutputTuple&
{
	std::swap(*this, ot);
	return *this;
}
auto OutputTuple::size() const -> size_t
{
	return len;
}
auto OutputTuple::write(std::ostream& o) const -> std::ostream&
{
	std::for_each(towrite.cbegin(), towrite.cend(), [&o](const uint8_t& u){
		o << u;
	});
	return o;
}
auto OutputTuple::needed_label() const -> const std::string&
{
	return op[0];
}
auto OutputTuple::resolved() const -> bool
{
	return !towrite.empty();
}



Output::Output(std::ostream& o)
	: out(o)
{
}


auto Output::write() -> bool
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

auto Output::force_write() -> void
{
	if(!write()){
		const std::string& label = buffer.front().needed_label();
		throw AssemblerError("label could not be resolved \""+label+"\"",buffer.front().line);
	}
}

auto Output::add_opcode(int line, const Opcode& op, std::vector<std::string> tok) -> void
{
	tok.erase(tok.cbegin());
	buffer.emplace_back(line, &op, tok);
	assert(op.operands <= 3);
	
	addr += op.size_in_bytes;
}

auto Output::add_label(int line, const std::string& s) -> void
{
	if(labels.find(s) != labels.end()){
		throw AssemblerError("label '"+s+"' defined (at least) twice (first "
			"appeared in line "+std::to_string(labels[s].line)+")", line);
	}
	labels[s] = Label{line, addr};

	if(!buffer.empty() && buffer.front().needed_label() == s){
		// if this label is used in the first command in the
		// buffer write everything we can
		write();
	}
}

auto Output::add_char(int line, const char *s) -> void
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
		buffer.emplace_back(line,std::move(vec));
	}
}

auto Output::add_int(int line, const std::vector<std::string>& tok) -> void
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
		buffer.emplace_back(line,std::move(vec));
	}
}

auto Output::add_float(int line, const std::vector<std::string>& tok) -> void
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
		buffer.emplace_back(line,std::move(vec));
	}
}
