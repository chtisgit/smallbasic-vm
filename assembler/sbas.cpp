#include "sb_opcodes.h"

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

auto assemble(istream& in, ostream& out) -> int
{
	constexpr int LINE_LEN = 200;
	char line[LINE_LEN];
	int line_n = 0;

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

		if(tok.size() == 0){
			/* empty line */
			continue;
		}
		auto& op = mnemonic_table.at(tok[0]);

		assert(tok.size() >= 1);
		if(op.operands != tok.size() - 1){
			cerr << "error: mnemonic " << tok[0]
			     << " takes " << op.operands
			     << " operand(s)" << endl;
			return 1;
		}

		out << op.code;

		// Debugging
		cerr << "mnemonic: '" << tok[0] << "' opcode="
		     << int(mnemonic_table.at(tok[0]).code) << endl;
		
	}
	return 0;
}

auto help(const char *app) -> void
{
	cerr << "Usage: " << app << "[input file] [output file]" << endl
	     << endl << "default input file: standard input" << endl
	     << "default output file: a.out" << endl;
}

auto main(int argc, char **argv) -> int
{
	int rv;
	switch(argc){
	case 1:
		{
		ofstream out("a.out");
		rv = assemble(cin, out);
		}
		break;
	case 2:
		{
		ifstream in(argv[1]);
		ofstream out("a.out");
		rv = assemble(in, out);
		}
		break;
	case 3:
		{
		ifstream in(argv[1]);
		ofstream out(argv[2]);
		rv = assemble(in, out);
		}
		break;
	default:
		help(argv[0]);
		return 3;
	}
	return rv;
}
