#pragma once

#include <cstdio>

namespace ObjTextWindow{

// Properties
int ForegroundColorSet(VMState& state){ return 0; }
int ForegroundColorGet(VMState& state){ return 0; }
int BackgroundColorSet(VMState& state){ return 0; }
int BackgroundColorGet(VMState& state){ return 0; }
int CursorLeftSet(VMState& state){ return 0; }
int CursorLeftGet(VMState& state){ return 0; }
int CursorTopSet(VMState& state){ return 0; }
int CursorTopGet(VMState& state){ return 0; }
int LeftSet(VMState& state){ return 0; }
int LeftGet(VMState& state){ return 0; }
int TopSet(VMState& state){ return 0; }
int TopGet(VMState& state){ return 0; }
int TitleSet(VMState& state){ return 0; }
int TitleGet(VMState& state){ return 0; }

// Functions

int Show(VMState& state){ return 0; }
int Hide(VMState& state){ return 0; }
int Clear(VMState& state)
{
	// FIXME: working as expected?
	printf("\f");
	return 1;
}
int Pause(VMState& state){ return 0; }
int PauseIfVisible(VMState& state){ return 0; }
int PauseWithoutMessage(VMState& state){ return 0; }
int Read(VMState& state)
{
	char buf[200], *endptr;
	bool term = false;
	std::string str;
	do{
		auto* s = fgets(buf, 200, stdin);
		if(s == nullptr) break;
		auto* nl = strchr(buf, '\n');
		if(nl != nullptr){
			*nl = '\0';
			term = true;
		}
#ifndef __unix__
		nl = strchr(buf, '\r');
		if(nl != nullptr){
			*nl = '\0';
			term = true;
		}
#endif
		str += buf;
	}while(!term);
	state.stack.push_back(str);
	return 1;
}
int ReadKey(VMState& state)
{
	state.stack.push_back(getchar());
	return 1;
}
int ReadNumber(VMState& state)
{
	char buf[200], *endptr;
	fgets(buf, 200, stdin);
	state.stack.push_back(strtod(buf, &endptr));
	// TODO: maybe check if endptr == '\0'
	return 1;
}
int WriteLine(VMState& state)
{
	if(state.stack.empty()) return 0;
	auto str = state.stack.back();
	state.stack.pop_back();
	printf("%s\n", str.getStringVal().c_str());
	return 1;
}
int Write(VMState& state)
{
	if(state.stack.empty()) return 0;
	auto str = state.stack.back();
	state.stack.pop_back();
	printf("%s", str.getStringVal().c_str());
	return 1;
}


};

