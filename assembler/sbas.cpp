#include "sb_opcodes.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <cassert>
#include <cstring>

using namespace std;

inline auto skip(char *s, char x) -> char*
{
	assert(x != '\0');
	while(*s == x) s++;
	return s;
}

auto main() -> int
{
	constexpr int LINE_LEN = 200;
	char line[LINE_LEN];
	int line_n = 0;

	for(;;){

		line_n++;
		cin.getline(line, LINE_LEN);
		if(cin.eof()){
			break;
		}
		if(cin.fail()){
			cerr << "error: line " << line_n << " too long!" << endl;
			do{
				cin.clear();
				cin.getline(line, LINE_LEN);
			}while(cin.fail());
			continue;
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
		cout << "mnemonic: '" << tok[0] << "' opcode="
		     << int(mnemonic_table.at(tok[0])) << endl;

	}
	return 0;
}
