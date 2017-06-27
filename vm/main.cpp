#include "state.h"
#include "object.h"
#include <math.h>

using namespace std;


auto run(CodeFile& file) -> void
{
	void *commands[256] = {
		&&op_copy, &&op_movi, &&op_jmp, &&op_push,
		&&op_pushi, &&op_add, &&op_sub, &&op_mul,
		&&op_div, &&op_addi, &&op_pop, &&op_call,
		&&op_ret, &&op_jc, &&op_jnc, &&op_lstr, &&op_equ,
		&&ops_neq, &&op_grt, &&op_lrt, &&op_geq, &&op_leq, 
		&&op_ccat, &&op_len, &&op_slice, &&op_read, &&op_write,
		&&op_dim, &&op_zero, &&op_estr, &&op_obj

	};

	VMState state(file);
#define	dispatch(A)   state.advance(A);\
	              goto *commands[state.decode()]
#define next_instr    dispatch(5)

	for(int i = 0; i < 256; i++){
		if(commands[i] == nullptr){
			commands[i] = &&op_notimplemented;
		}
	}
#ifdef DEBUG
	commands[254] = &&reg_debug;
	commands[255] = &&str_debug;
#endif

	dispatch(0);
	// VM operations:
op_copy:
	state.registers[state.decode1()] = state.registers[state.decode2()];
	next_instr;
op_movi:
	state.registers[state.decode1()] = state.decode2();
	next_instr;
op_jmp:
	state.jump(state.decode_imm32());
	dispatch(0);
op_push:{
	int reg1 = state.decode1();
	state.stack.push_back(state.registers[reg1]);
	}
	next_instr;
op_pushi:
	state.stack.push_back(state.decode_imm32());
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
op_ret:
	if(!state.ret()) return;
	dispatch(0);
op_jc:
	state.conditional_reljump(state.decode_imm32(), 5);
	dispatch(0);
op_jnc:
	state.conditional_reljump(5, state.decode_imm32());
	dispatch(0);
op_lstr:{
	if(state.stack.empty())
		return;
	const char *str = reinterpret_cast<const char*>(state.file.code()) + state.stack.back().getIntVal();
	state.registers[state.decode1()]= std::string(str);
	state.stack.pop_back();
	}
	next_instr;
op_equ:{
	int reg1_content = state.registers[state.decode1()].getIntVal();
	int reg2_content = state.registers[state.decode2()].getIntVal();
	state.set_cond(reg1_content == reg2_content);
	}
	next_instr;
ops_neq:{
	int reg1_content = state.registers[state.decode1()].getIntVal();
	int reg2_content = state.registers[state.decode2()].getIntVal();
	state.set_cond(reg1_content != reg2_content);
	}
	next_instr;
op_grt:{
	int reg1_content = state.registers[state.decode1()].getIntVal();
	int reg2_content = state.registers[state.decode2()].getIntVal();
	state.set_cond(reg1_content > reg2_content);
	} 
	next_instr;
op_lrt:{
	int reg1_content = state.registers[state.decode1()].getIntVal();
	int reg2_content = state.registers[state.decode2()].getIntVal();
	state.set_cond(reg1_content < reg2_content);
	}
	next_instr;
op_geq:{
	int reg1_content = state.registers[state.decode1()].getIntVal();
	int reg2_content = state.registers[state.decode2()].getIntVal();
	state.set_cond(reg1_content >= reg2_content);
	}
	next_instr;
op_leq:{
	int reg1_content = state.registers[state.decode1()].getIntVal();
	int reg2_content = state.registers[state.decode2()].getIntVal();
	state.set_cond(reg1_content <= reg2_content);
	}
	next_instr;
op_ccat:
	state.registers[state.decode1()] = state.registers[state.decode1()].getStringVal() 
										+ state.registers[state.decode2()].getStringVal();
	next_instr;
op_len:
	state.registers[state.decode1()] = state.registers[state.decode2()].length();
	next_instr;
op_slice:{
	std::string s = state.registers[state.decode1()].getStringVal();
	int i =  state.registers[state.decode2()].getIntVal();

	if(i >= (int)s.length()){
		i = s.length() -1;
	}
	state.registers[state.decode1()] = i >= 0 ? 
										s.substr(0, i):
										s.substr(0, s.length() + i-1);				
	}
	next_instr;
op_read:{
	int i = state.stack.back().getIntVal();
	state.registers[state.decode1()] = state.registers[state.decode2()][i];
 state.stack.pop_back();
	}
	next_instr;
op_write:{
	int i = state.stack.back().getIntVal();
	state.registers[state.decode1()][i] = state.registers[state.decode2()];
	state.stack.pop_back();
	}
	next_instr;
op_dim:
	state.registers[state.decode1()][state.registers[state.decode2()].getIntVal()] = 0.0;
	next_instr;
op_zero:
	state.registers[state.decode1()] = 0;
	state.registers[state.decode2()] = 0;
	next_instr;
op_estr:
	state.registers[state.decode1()].getStringVal().clear();
	state.registers[state.decode2()].getStringVal().clear();
	next_instr;
op_obj:{
	auto safety_check = state.file.code();
	auto obj = state.decode1();
	auto fun = state.decode2();
	assert(obj >= 0 && fun >= 0);
	auto ret = sb_objects[object_id(obj,fun)](state);
	/* if the called function returns 0 it failed */
	assert(ret != 0);
	/* the called instruction must not change the instruction pointer */
	assert(state.file.code() == safety_check);
	}
	next_instr;

#ifdef DEBUG
reg_debug:{
	int reg1 = state.decode1();
	int reg2 = state.decode2();
	printf("reg: %d content: %d\n", reg1, state.registers[reg1].getIntVal());	
	printf("reg: %d content: %d\n", reg2, state.registers[reg2].getIntVal());
	}
	next_instr;
str_debug:{
	int reg = state.decode1();	
	printf("reg: %d content: %s\n", reg, state.registers[reg].getStringVal().c_str());
	}
	next_instr;
#endif // DEBUG

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

