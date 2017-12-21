#include <state.h>

namespace ObjArray{

// Functions

int ContainsIndex(VMState& state)
{	
	if(state.stack.size() < 2) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();
	auto index = state.stack.back();
	state.stack.pop_back();

	state.stack.push_back(arr.containsIndex(index.getIntVal()) ? 1 : 0);
	return 1;
}
int ContainsValue(VMState& state)
{
	if(state.stack.size() < 2) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();
	auto index = state.stack.back();
	state.stack.pop_back();

	state.stack.push_back(arr.getValueIndex(index.getIntVal()) >= 0 ? 1 : 0);
	return 1;
}
int GetAllIndices(VMState& state)
{
	if(state.stack.empty()) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();

	state.stack.push_back(arr.getIndexArray());
	return 1;
}
int GetItemCount(VMState& state)
{
	if(state.stack.empty()) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();

	state.stack.push_back(arr.getItemCount());
	return 1;
}
int IsArray(VMState& state)
{
	if(state.stack.empty()) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();

	state.stack.push_back(arr.isArray() ? 1 : 0);
	return 1;
}
int SetValue(VMState& state)
{
	if(state.stack.size() < 3) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();
	auto index = state.stack.back();
	state.stack.pop_back();
	auto value = state.stack.back();
	state.stack.pop_back();

	arr[index.getIntVal()] = value;

	return 1;
}
int GetValue(VMState& state)
{
	if(state.stack.size() < 2) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();
	auto index = state.stack.back();
	state.stack.pop_back();

	state.stack.push_back(arr[index.getIntVal()]);

	return 1;
}
int RemoveValue(VMState& state)
{
	if(state.stack.size() < 2) return 0;
	auto arr = state.stack.back();
	state.stack.pop_back();
	auto index = state.stack.back();
	state.stack.pop_back();

	arr.removeItem(index.getIntVal());

	return 1;
}

};

extern "C"{

	/* exported symbols */

	const void *Object1Functions[] = {
		reinterpret_cast<void*>(ObjArray::ContainsIndex),
		reinterpret_cast<void*>(ObjArray::ContainsValue),
		reinterpret_cast<void*>(ObjArray::GetAllIndices),
		reinterpret_cast<void*>(ObjArray::GetItemCount),
		reinterpret_cast<void*>(ObjArray::IsArray),
		reinterpret_cast<void*>(ObjArray::SetValue),
		reinterpret_cast<void*>(ObjArray::GetValue),
		reinterpret_cast<void*>(ObjArray::RemoveValue),
		nullptr
	};

	const char *Object1FunctionNames[] = {
		nullptr
	};

}


