#include "state.h"
#include "object.h"
#include <math.h>

#define DST (state.dst())
#define DSTREG (state.registers[DST])
#define SRC1 (state.src1())
#define SRC1REG (state.registers[SRC1])
#define SRC2 (state.src2())
#define SRC2REG (state.registers[SRC2])
#define IMM16 (state.imm16())
#define IMM32 (state.imm32())

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
	//commands[254] = &&reg_debug;
	commands[255] = &&reg_debug;//str_debug
#endif

	dispatch(0);
	// VM operations:
op_copy:
	DSTREG = SRC1REG;
	dispatch(3);
op_movi:
	DSTREG = IMM16;	
	dispatch(4);
op_jmp:
	state.jump(IMM32);
	dispatch(0);
op_push:{
	state.stack.push_back(DSTREG);
	}
	dispatch(2);
op_pushi:
	state.stack.push_back(IMM32);
	dispatch(5);
op_add:{
	DSTREG = SRC1REG.getIntVal() + SRC2REG.getIntVal();
	}
	dispatch(4);
op_sub:{
	DSTREG = SRC1REG.getIntVal() - SRC2REG.getIntVal();
	}
	dispatch(4);
op_mul:{
	DSTREG = SRC1REG.getIntVal() * SRC2REG.getIntVal();
	}
	dispatch(4);
op_div:{
	DSTREG = SRC1REG.getIntVal() / SRC2REG.getIntVal();
	}
	dispatch(4);
op_addi:{
	DSTREG = SRC1REG.getIntVal() + SRC2;
	}
	dispatch(5);
op_pop:{
	if(state.stack.empty())
			return;
	DSTREG = state.stack.back();
	state.stack.pop_back();
	}
	dispatch(2);
op_call:{
	//state.advance(5); // saved ip must be next instruction
	state.call(IMM32);
	}
	dispatch(0);
op_ret:
	/*int returnaddress;	
	if(state.stack.empty())
			return;
	returnaddress = state.stack.back().getIntVal();
	state.stack.pop_back();
	*/
	int topop = SRC1REG.getIntVal();

	for(int i=0; i < topop; i++){
		if(state.stack.empty())
			return;
		state.stack.pop_back();
	}

	if(!state.ret()) return;
	dispatch(0);
op_jc:
	state.conditional_reljump(IMM32, 0);
	dispatch(0);
op_jnc:
	state.conditional_reljump(IMM32, 1);
	dispatch(0);
op_lstr:{
	if(state.stack.empty())
		return;
	const char *str = reinterpret_cast<const char*>(state.file.code()) + state.stack.back().getIntVal();
	DSTREG = std::string(str);
	state.stack.pop_back();
	}
	dispatch(2);
op_equ:{
	state.set_cond(DSTREG.getIntVal() == SRC1REG.getIntVal());
	}
	dispatch(3);
ops_neq:{
	state.set_cond(DSTREG.getIntVal() != SRC1REG.getIntVal());
	}
	dispatch(3);
op_grt:{
	state.set_cond(DSTREG.getIntVal() > SRC1REG.getIntVal());
	} 
	dispatch(3);
op_lrt:{
	state.set_cond(DSTREG.getIntVal() < SRC1REG.getIntVal());
	}
	dispatch(3);
op_geq:{
	state.set_cond(DSTREG.getIntVal() >= SRC1REG.getIntVal());
	}
	dispatch(3);
op_leq:{
	state.set_cond(DSTREG.getIntVal() <= SRC1REG.getIntVal());
	}
	dispatch(3);
op_ccat:
	DSTREG = SRC1REG.getStringVal()	+ SRC2REG.getStringVal();
	dispatch(4);
op_len:
	DSTREG = SRC1REG.length();
	dispatch(3);
op_slice:{
	const auto& s = SRC1REG.getStringVal();
	int i =  SRC2REG.getIntVal();

	if(i >= (int)s.length()){
		i = s.length() -1;
	}
	DSTREG = i >= 0 ? s.substr(0, i): s.substr(s.length()+i, s.length()-1);
	}
	dispatch(4);
op_read:{
	DSTREG = SRC1REG[SRC2REG.getIntVal()];
	}
	dispatch(4);
op_write:{
	DSTREG[SRC1REG.getIntVal()] = SRC2REG;
	}
	dispatch(4);
op_dim:
	DSTREG.dim(SRC1REG.getIntVal());
	dispatch(3);
op_zero:
	DSTREG.zero();
	dispatch(2);
op_estr:
	DSTREG.emptyString();
	dispatch(2);
op_obj:{
	auto safety_check = state.file.code();
	auto obj = DST;
	auto fun = SRC1;
	assert(obj >= 0 && fun >= 0);
	auto ret = sb_objects[object_id(obj,fun)](state);
	/* if the called function returns 0 it failed */
	assert(ret != 0);
	/* the called instruction must not change the instruction pointer */
	assert(state.file.code() == safety_check);
	}
	dispatch(3);

#ifdef DEBUG
/*reg_debug:{
	printf("reg: %d content: %d\n", DST, DSTREG.getIntVal());	
	printf("reg: %d content: %d\n", SRC1, SRC1REG.getIntVal());
	}
	dispatch(3);*/
reg_debug:{
	//printf("reg: %d content: %s\n", DST, DSTREG.getStringVal().c_str());
	std::cout << "reg: " << std::to_string(DST) << " type: " << DSTREG.getType() << " content: " << DSTREG << std::endl;
	}
	dispatch(2);
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

