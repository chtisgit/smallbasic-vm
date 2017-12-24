#include "sbas.h"
#include "sb_opcodes.h"
#include "sb_output.h"
#include "asmexcept.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <cassert>
#include <cstring>

using namespace std;

auto help(const char *app) -> void
{
	cerr << "Usage: " << app << "[input file] [output file]" << endl
	     << endl << "default input file: standard input" << endl
	     << "default output file: a.out" << endl;
}

auto assemble(const char *inputfile, ostream& out) -> int
{

	constexpr int LINE_LEN = 200;
	char line[LINE_LEN];
	Output output{out};

	// AssemblyFile must have a noexcept move constructor!
	std::vector<AssemblyFile> includes;

	// main file
	std::istream* first_input = &cin;
	if(inputfile == nullptr){
		inputfile = "<stdin>";
	}else{
		first_input = new ifstream(inputfile);
	}
	includes.emplace_back(inputfile, 0, first_input);

	while(!includes.empty()){
		auto& current_file = includes.back();

		current_file.line++;
		if(current_file.stream->fail()){
			cerr << "failed to read from file " << *current_file.path << endl;
			break;
		}
		current_file.stream->getline(line, LINE_LEN);
		if(current_file.stream->eof()){
			includes.pop_back();
			continue;
		}
		if(current_file.stream->fail()){
			cerr << "error in file " << current_file.path << ": line "
				<< current_file.line << " too long!" << endl;
			return 1;
		}
		istringstream st{ string(line) };
		vector<string> tok;

		copy(istream_iterator<string>(st),
		     istream_iterator<string>(),
		     back_inserter(tok));

		// add all labels to output and remove them from tok
		auto first_non_label = find_if(tok.begin(), tok.end(),
			[](const string& s) -> bool{
				return !s.empty() && s.back() != ':';
			});
		for(auto it = tok.begin(); it != first_non_label; it++){
			auto& label = *it;
			label.erase(label.end()-1, label.end());
			output.add_label(current_file, std::move(label));
		}
		tok.erase(tok.begin(), first_non_label);
		
		// remove comments from tok
		auto comment = find_if(tok.begin(), tok.end(),
			[](const string& s) -> bool{
				return !s.empty() && s.front() == '#';
			});
		if(comment != tok.end()){
			tok.erase(comment, tok.end());
		}

		if(tok.size() == 0){
			/* empty line */
			continue;
		}

		if(tok[0] == "include"){
			const char *filename = strstr(line, "include")+8;
			auto *i = static_cast<istream*>(new ifstream(filename));
			if(i == nullptr){
				throw AssemblerError(string("could not open included file \"")+filename+"\"",current_file);
			}
			includes.emplace_back(filename, 0, i);
			continue;
		}

		if(tok[0] == "char"){
			output.add_char(current_file, line);
		}else if(tok[0] == "int"){
			output.add_int(current_file, tok);
		}else if(tok[0] == "float"){
			output.add_float(current_file, tok);
		}else{
			const auto& op = [&tok,&current_file]() -> Opcode& {
				try{
					return mnemonic_table.at(tok[0]);
				}catch(std::out_of_range& e){
					throw AssemblerError("unknown mnemonic '"+tok[0]+"'",current_file);
				}
			}();

			// Debugging
			//cerr << "mnemonic: '" << tok[0] << "' opcode="
			//     << int(mnemonic_table.at(tok[0]).code) << endl;

			assert(tok.size() >= 1);
			if(op.operands != tok.size() - 1){
				throw AssemblerError("mnemonic "+tok[0]+" takes "+
					std::to_string(op.operands)+" operand(s)", current_file);
			}
			output.add_opcode(current_file, op, tok);
		}
	}
	output.force_write();
	return 0;
}


auto main(int argc, char **argv) -> int
{
	int rv;
	try{
		switch(argc){
		case 1:
			{
			ofstream out("a.out", std::ios_base::out | std::ios_base::binary);
			rv = assemble(nullptr, out);
			}
			break;
		case 2:
			{
			ofstream out("a.out", std::ios_base::out | std::ios_base::binary);
			rv = assemble(argv[1], out);
			}
			break;
		case 3:
			{
			ofstream out(argv[2], std::ios_base::out | std::ios_base::binary);
			rv = assemble(argv[1], out);
			}
			break;
		default:
			help(argv[0]);
			return 3;
		}
	}catch(runtime_error& err){
		cerr << "error: " << err.what() << endl << endl;
		rv = 4;
	}catch(AssemblerError& err){
		cerr << "error in file " << err.what_file() << ", line "
			<< err.what_line() << ": " << err.what() << endl << endl;
		rv = 4;
	}
	return rv;
}
