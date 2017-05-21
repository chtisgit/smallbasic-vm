#include "sb_opcodes.h"
#include "sb_output.h"

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

auto assemble(istream& in, ostream& out) -> int
{
	constexpr int LINE_LEN = 200;
	char line[LINE_LEN];
	int line_n = 0;
	Output output{out};

	for(;;){

		line_n++;
		in.getline(line, LINE_LEN);
		if(in.eof()){
			break;
		}
		if(in.fail()){
			cerr << "error: line " << line_n << " too long!" << endl;
			return 1;
		}
		istringstream st{ string(line) };
		vector<string> tok;

		copy(istream_iterator<string>(st),
		     istream_iterator<string>(),
		     back_inserter(tok));

		auto first_non_label = find_if(tok.begin(), tok.end(),
			[](const string& s) -> bool{
				return !s.empty() && s.back() != ':';
			});
		for(auto it = tok.begin(); it != first_non_label; it++){
			auto& label = *it;
			label.erase(label.end()-1, label.end());
			output.add_label(line_n, label);
		}
		tok.erase(tok.begin(), first_non_label);

		if(tok.size() == 0){
			/* empty line */
			continue;
		}

		if(tok[0] == "char"){
			output.add_char(line_n, tok);
		}else if(tok[0] == "int"){
			output.add_int(line_n, tok);
		}else if(tok[0] == "float"){
			output.add_float(line_n, tok);
		}else{
			auto& op = mnemonic_table.at(tok[0]);

			// Debugging
			//cerr << "mnemonic: '" << tok[0] << "' opcode="
			//     << int(mnemonic_table.at(tok[0]).code) << endl;

			assert(tok.size() >= 1);
			if(op.operands != tok.size() - 1){
				cerr << "error: mnemonic " << tok[0]
				     << " takes " << op.operands
				     << " operand(s)" << endl;
				return 1;
			}
			output.add_opcode(line_n, op, tok);
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
			rv = assemble(cin, out);
			}
			break;
		case 2:
			{
			ifstream in(argv[1]);
			ofstream out("a.out", std::ios_base::out | std::ios_base::binary);
			rv = assemble(in, out);
			}
			break;
		case 3:
			{
			ifstream in(argv[1]);
			ofstream out(argv[2], std::ios_base::out | std::ios_base::binary);
			rv = assemble(in, out);
			}
			break;
		default:
			help(argv[0]);
			return 3;
		}
	}catch(runtime_error& err){
		cerr << "error: " << err.what() << endl << endl;
		rv = 4;
	}
	return rv;
}
