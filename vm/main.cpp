#include "state.h"

using namespace std;


auto run(CodeFile& file) -> void
{
	void *commands[256] = { &&op_copy, &&op_movi, &&op_jmp };
	constexpr int commands_no = 3;
	VMState<commands_no> state(file);
#define	dispatch(A)   state.advance(A);\
	              goto *commands[state.decode()]

	for(int i = commands_no; i < 256; i++){
		commands[i] = &&op_notimplemented;
	}

	dispatch(0);
	// VM operations:
op_copy:
	// TODO: mov operation
	printf("copy\n");
	dispatch(5);
op_movi:
	// TODO: add operation
	printf("movi\n");
	dispatch(5);
op_jmp:
	state.jump(state.decode_imm32());
	dispatch(0);
	
op_notimplemented:
	fprintf(stderr, "error: opcode %d not implemented!\n",
			int(state.decode()));
	return;
	
}

auto main(int argc, char **argv) -> int
{
	if(argc < 2){
		printf("error: no bytecode file given!\n\n");
		return 1;
	}
	try{
		CodeFile file(argv[1]);
		run(file);
	}catch(runtime_error& e){
		printf("runtime error: %s\n\n", e.what());
		return 2;
	}
	return 0;
}

