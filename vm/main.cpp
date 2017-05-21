#include "state.h"

using namespace std;


auto run(CodeFile& file) -> void
{
	void *commands[256] = {
		&&op_copy, &&op_movi, &&op_jmp, &&op_push,
		&&op_pushi, &&op_add, &&op_sub, &&op_mul,
		&&op_div, &&op_addi, &&op_pop, &&op_call,
		&&op_ret, &&op_jc, &&op_jnc
	};
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
op_push:
	dispatch(5);
op_pushi:
	dispatch(5);
op_add:
	dispatch(5);
op_sub:
	dispatch(5);
op_mul:
	dispatch(5);
op_div:
	dispatch(5);
op_addi:
	dispatch(5);
op_pop:
	dispatch(5);
op_call:
	state.advance(5); // saved ip must be next instruction
	state.call(state.decode_imm32());
	dispatch(0);
op_ret:
	if(!state.ret()) return;
	dispatch(0);
op_jc:
	state.conditional_reljump(state.decode_imm32(), 5);
	dispatch(0);
op_jnc:
	state.conditional_reljump(5, state.decode_imm32());
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

