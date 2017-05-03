#include "state.h"

#include <iostream>

using namespace std;


auto run(CodeFile& file) -> void
{
	void *commands[] = { &&op_end, &&op_mov, &&op_add };
	constexpr int commands_no = sizeof(commands)/sizeof(*commands);
	VMState state(file, commands_no);
#define dispatch()	goto *commands[state.advance()];

	dispatch();
	// VM operations:
op_end:
	return;
op_mov:
	// TODO: mov operation
	cout << "mov" << endl;
	dispatch();
op_add:
	// TODO: add operation
	cout << "add" << endl;
	dispatch();
	
}

auto main(int argc, char **argv) -> int
{
	if(argc < 2){
		cout << "error: no bytecode file given!" << endl;
		return 1;
	}
	try{
		CodeFile file(argv[1]);
		run(file);
	}catch(runtime_error e){
		cout << "runtime error: " << e.what() << endl << endl;
		return 2;
	}
	return 0;
}

