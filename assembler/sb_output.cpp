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

// resolving labels is currently only implemented for op1
// and it resolves into op32_r
// this only supports labels in mnemonics with 1 operand
auto parse_operand32(const std::string& s, LabelDeducer get_label_addr) -> int32_t
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
	}else if(isalpha(s[0]) || s[0] == '_' || s[0] == '.'){
		rv = get_label_addr(s);
	}else{
		throw std::runtime_error("invalid operand \""+s+"\"");
	}
	return rv;

}

auto OutputTuple::warning(const AssemblyFile& asmfile, const char *t, OperandType expected, OperandType actual) -> void
{
	if(expected != actual){
		if(expected == OT_REGISTER)
			std::cerr << "warning in file " << asmfile.path << ": "
				<< t << " operand should be "
				"register (immediate given) in line " <<
				asmfile.line << std::endl;
		else
			std::cerr << "warning in file " << asmfile.path << ": " 
				<< t << " operand should be "
				"immediate (register given) in line " << 
				asmfile.line << std::endl;
	}
}


OutputTuple::OutputTuple(const AssemblyFile& asmfile, const Opcode *const opcode, std::vector<std::string> op_, const std::string& most_recent_label)
	: opcode(opcode), op(std::move(op_)), line(asmfile.line), path(asmfile.path)
{
	//std::cerr << "opcode=" << int(op->code) << " op1=" << op1 << " op2=" << op2 << std::endl;
	// could be different for opcodes with less than 2 operands
	len = opcode->size_in_bytes;

	// extend local label short-hand notation
	for(unsigned i = 0; i < opcode->operands; i++){
		if(opcode->optype[i] == OT_IMM32 && op[i].size() > 1 && op[i][0] == '.'){
			op[i] = most_recent_label+op[i];
		}
	}
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
auto OutputTuple::resolve(LabelDeducer get_label_addr) -> bool
{
	if(towrite.empty()){
		try{
			std::vector<uint8_t> temp;
			if(opcode->operands > 0){
				int op8_i = 0;
				for(unsigned i = 0; i < opcode->operands; i++){
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
			throw AssemblerError(rue.what(),*path,line);
		}
	}
	return resolved();
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
			if(s[0] == '.')
				return this->labels.at(this->most_recent_label+s).addr;
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
		throw AssemblerError("label could not be resolved \""+label+"\"",*buffer.front().path,buffer.front().line);
	}
}

auto Output::add_opcode(const AssemblyFile& asmfile, const Opcode& op, std::vector<std::string> tok) -> void
{
	tok.erase(tok.cbegin());
	buffer.emplace_back(asmfile, &op, tok, most_recent_label);
	assert(op.operands <= 3);
	
	addr += op.size_in_bytes;
}

auto Output::add_label(const AssemblyFile& asmfile, std::string name) -> void
{
	if(name[0] == '.'){
		name = most_recent_label+name;
	}else{
		most_recent_label = name;
	}

	if(labels.find(name) != labels.end()){
		throw AssemblerError("label '"+name+"' defined (at least) twice (first "
			"appeared in line "+std::to_string(labels[name].line)+")", asmfile);
	}
	labels[name] = Label{asmfile.line, addr};

	if(!buffer.empty() && buffer.front().needed_label() == name){
		// if this label is used in the first command in the
		// buffer write everything we can
		write();
	}
}

auto Output::add_char(const AssemblyFile& asmfile, const char *s) -> void
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
		buffer.emplace_back(asmfile.line,std::move(vec));
	}
}

auto Output::add_int(const AssemblyFile& asmfile, const std::vector<std::string>& tok) -> void
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
		buffer.emplace_back(asmfile.line,std::move(vec));
	}
}

auto Output::add_float(const AssemblyFile& asmfile, const std::vector<std::string>& tok) -> void
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
		buffer.emplace_back(asmfile.line,std::move(vec));
	}
}
