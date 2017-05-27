#include "state.h"

using namespace std;


auto run(CodeFile& file) -> void
{
	void *commands[256] = {
		&&op_copy, &&op_movi, &&op_jmp, &&op_push,
		&&op_pushi, &&op_add, &&op_sub, &&op_mul,
		&&op_div, &&op_addi, &&op_pop, &&op_call,
		&&op_ret, &&op_jc, &&op_jnc, &&op_lsrt
	};
	int commands_no = 0;
	for(auto c: commands){
		if(c != NULL)
			commands_no++;
		else
			break;
	}
	VMState state(file);
#define	dispatch(A)   state.advance(A);\
	              goto *commands[state.decode()]
#define next_instr    dispatch(5)

	for(int i = commands_no; i < 256; i++){
		commands[i] = &&op_notimplemented;
	}

	dispatch(0);
	// VM operations:
op_copy:{
	// TODO: mov operation
	//printf("copy\n");
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	state.registers[reg1] = state.registers[reg2];	
	}
	next_instr;
op_movi:{
	// TODO: add operation
	//printf("movi\n");
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	state.registers[reg1] = reg2;
	}
	next_instr;
op_jmp:{
	state.jump(state.decode_imm32());
	}
	dispatch(0);
op_push:{
	int reg1 = state.decode1();
	state.stack.push_back(state.registers[reg1]);
	}
	next_instr;
op_pushi:{
	state.stack.push_back(state.decode_imm32());
	}
	next_instr;
op_add:{
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	state.registers[reg1] = state.registers[reg1].getIntVal() + state.registers[reg2].getIntVal();
	}
	next_instr;
op_sub:{
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	state.registers[reg1] = state.registers[reg1].getIntVal() - state.registers[reg2].getIntVal();
	}
	next_instr;
op_mul:{
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	state.registers[reg1] = state.registers[reg1].getIntVal() * state.registers[reg2].getIntVal();
	}
	next_instr;
op_div:{
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	state.registers[reg1] = state.registers[reg1].getIntVal() / state.registers[reg2].getIntVal();
	}
	next_instr;
op_addi:{
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	state.registers[reg1] = state.registers[reg1].getIntVal() + reg2;
	printf("reg: %d content: %d\n", reg1, state.registers[reg1].getIntVal());
	}
	next_instr;
op_pop:{
	if(state.stack.empty())
			return;
	state.registers[state.decode1()]= state.stack.back();
	state.stack.pop_back();
	}
	next_instr;
op_call:{
	int reg1 = state.decode_imm32();
	state.advance(5); // saved ip must be next instruction
	state.call(reg1);
	}
	dispatch(0);
op_ret:{
	if(!state.ret()) return;
	}	
	dispatch(0);
op_jc:{
	state.conditional_reljump(state.decode_imm32(), 5);
	}
	dispatch(0);
op_jnc:{
	state.conditional_reljump(5, state.decode_imm32());
	}
	dispatch(0);
op_lsrt:{
	if(state.stack.empty())
			return;
	state.registers[state.decode1()]= std::string(reinterpret_cast<const char *>(state.file.code()) + state.stack.back().getIntVal());
	state.stack.pop_back();
	printf("string in reg %d : %s\n", state.decode1(), (state.registers[state.decode1()]).getStringVal().c_str());
}
next_instr;
op_notimplemented:{
	fprintf(stderr, "error: opcode %d not implemented!\n",
			int(state.decode()));
	return;
	}	
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

