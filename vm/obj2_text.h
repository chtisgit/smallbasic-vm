#pragma once

#include <cstdio>

#include "value.h"

namespace ObjText{

// Functions

int Append(VMState& state){
	if(state.stack.size() < 2) return 0;
	auto text1 = state.stack.back();
	state.stack.pop_back();
	auto text2 = state.stack.back();
	state.stack.pop_back();

	state.stack.emplace_back(text1.getStringVal() + text2.getStringVal());
	return 1;
}
int GetLength(VMState& state){
	if(state.stack.size() < 1) return 0;
	auto text = state.stack.back().getStringVal();
	state.stack.pop_back();

	state.stack.emplace_back(int(text.size()));
	return 1;
}
int IsSubText(VMState& state){
	if(state.stack.size() < 2) return 0;
	auto text1 = state.stack.back().getStringVal();
	state.stack.pop_back();
	auto text2 = state.stack.back().getStringVal();
	state.stack.pop_back();
	
	// is text2 in text1?
	state.stack.emplace_back(text1.find(text2) == std::string::npos ? 0 : 1);
	return 1;
}
int EndsWith(VMState& state){
	if(state.stack.size() < 2) return 0;
	auto text1 = state.stack.back().getStringVal();
	state.stack.pop_back();
	auto text2 = state.stack.back().getStringVal();
	state.stack.pop_back();
	
	// does text1 end with text2
	auto pos = text1.find(text2);
	if(pos != std::string::npos && text1.size()-pos == text2.size()){
		state.stack.emplace_back(1);
	}else{
		state.stack.emplace_back(0);
	}
	return 1;
}
int StartsWith(VMState& state){
	if(state.stack.size() < 2) return 0;
	auto text1 = state.stack.back().getStringVal();
	state.stack.pop_back();
	auto text2 = state.stack.back().getStringVal();
	state.stack.pop_back();
	
	// does text1 start with text2
	state.stack.emplace_back(text1.compare(0, text2.size(), text2) == 0 ? 1 : 0);
	return 1;
}
int GetSubText(VMState& state){
	if(state.stack.size() < 3) return 0;
	auto text = state.stack.back().getStringVal();
	state.stack.pop_back();
	auto start = state.stack.back().getIntVal();
	state.stack.pop_back();
	auto len = state.stack.back().getIntVal();
	state.stack.pop_back();
	
	state.stack.emplace_back(text.substr(start,len));
	return 1;
}
int GetSubTextToEnd(VMState& state){
	if(state.stack.size() < 2) return 0;
	auto text = state.stack.back().getStringVal();
	state.stack.pop_back();
	auto start = state.stack.back().getIntVal();
	state.stack.pop_back();
	
	state.stack.emplace_back(text.substr(start));
	return 1;
}
int GetIndexOf(VMState& state){ 
	if(state.stack.size() < 2) return 0;
	auto text1 = state.stack.back().getStringVal();
	state.stack.pop_back();
	auto text2 = state.stack.back().getStringVal();
	state.stack.pop_back();
	
	// is text2 in text1?
	state.stack.emplace_back(int(text1.find(text2)));
	return 1;
}
int ConvertToLowerCase(VMState& state){
	if(state.stack.size() < 1) return 0;
	auto text = state.stack.back().getStringVal();
	state.stack.pop_back();
	
	for(auto& a : text){
		if(a >= 'A' && a <= 'Z'){
			a = a - 'A' + 'a';
		}
	}

	state.stack.emplace_back(text);
	return 1;
}
int ConvertToUpperCase(VMState& state){ 
	if(state.stack.size() < 1) return 0;
	auto text = state.stack.back().getStringVal();
	state.stack.pop_back();
	
	for(auto& a : text){
		if(a >= 'a' && a <= 'z'){
			a = a - 'a' + 'A';
		}
	}

	state.stack.emplace_back(text);
	return 1;
}
int GetCharacter(VMState& state){
	if(state.stack.size() < 1) return 0;
	auto code = state.stack.back().getIntVal();
	state.stack.pop_back();
	
	state.stack.emplace_back(std::string(1,code));
	return 1;
}
int GetCharacterCode(VMState& state){
	if(state.stack.size() < 1) return 0;
	auto text = state.stack.back().getStringVal();
	state.stack.pop_back();
	
	state.stack.emplace_back(int(text[0]));
	return 1;
}

}
