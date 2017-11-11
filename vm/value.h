#pragma once

#include <vector>
#include <functional>
#include <string>
#include <tuple>
#include <iostream>
#include <string.h>
#include <assert.h>

// the floating point type that the VM should use to
// represent SmallBasic Floats
using SBFloat = double;

class Value{
private:
	enum Type{INT, FLOAT, STRING};
	Type type;
	SBFloat float_val;
	std::string string_val;
	int int_val;
	std::map<int, Value> array;
	bool a = false;

	void makeScalar(){
		a = false;
		array.clear();
	}

	friend std::ostream& operator<<(std::ostream &strm, const Value &val);
public:

	// FIXME: this probably needs some fix
	// for comparisons between FLOATs and INTs at least
	auto operator==(const Value& rhs) const -> bool
	{
		return type == rhs.type &&
			((type == INT && int_val == rhs.int_val) ||
			 (type == FLOAT && float_val == rhs.float_val) ||
			 (type == STRING && string_val == rhs.string_val));
	}

	auto isArray() const -> bool
	{
		return a;
	}

	auto containsIndex(int i) const -> bool
	{
		return isArray() && array.count(i) == 1;
	}

	auto getValueIndex(const Value& val) const -> int
	{
		int index = 0;
		for(const auto& key_value : array){
			if(key_value.second == val)
				return index;
			index++;
		}
		return -1;
	}

	auto getIndexArray() const -> Value
	{
		Value newArr;
		int index = 1;
		for(const auto& key_value : array){
			newArr[index++] = key_value.first;
		}
		newArr.a = true;
		return newArr;
	}

	auto getItemCount() const -> int
	{
		return array.size();
	}

	auto removeItem(int index) -> void
	{
		array.erase(index);
	}

	auto operator[](const int i) -> Value&
	{
		a = true;
		return array[i];
	}

	Value(){
		type = FLOAT;
		float_val = 0;
	}

	Value(int x){
		type = INT;
		int_val = x;
	}

	Value(SBFloat x){
		type = FLOAT;
		float_val = x;
		string_val = std::to_string(float_val);
	}

	Value(const std::string& x){
		type = STRING;
		string_val = x;		
	}
	Value(std::string&& x){
		type = STRING;
		string_val = std::move(x);
	}

	~Value(){
	}

	void emptyString(){
		type = STRING;
		string_val.clear();
		makeScalar();
	}

	void zero(){
		type = FLOAT;
		float_val = 0;
		int_val = 0; 
		string_val = "0";
		makeScalar();
	}

	int length(void){
		if(a){
			return array.rbegin()->first; //"biggest key" = array size
		}
		switch(type){
		case STRING:
			return string_val.length();
		default:
			return 0;
		}
	}

	void dim(int length){
		a = true;
		array[length] = 0;
	}

	void getValue(void *ptr){
		switch(type){
		case FLOAT:
			*((SBFloat *)ptr) = getFloatVal();
			break;
		case INT:	
			*((int *)ptr) = getIntVal();
			break;
		case STRING:
			*((std::string*)ptr) = getStringVal();
			break;
		default:
			ptr = NULL;
		}
	}
	
	auto getValue(void) -> std::tuple<int, SBFloat, const std::string&>{
		return std::make_tuple(int(float_val), float_val, std::ref(string_val));
	}
	
	int getIntVal(void){
		switch(type){
		case FLOAT:
			return int(float_val);
		case INT:
			return int_val;
		case STRING:
			return std::stoi(string_val, nullptr, 10);
		default:
			assert(0);
		}	
	}
	
	SBFloat getFloatVal(void){
		switch(type){
		case FLOAT:
			return float_val;
		case INT:
			return (SBFloat)int_val;
		case STRING:
			std::string::size_type sz;
			return std::stod(string_val, &sz);
		default:
			assert(0);
		}
	}

	const std::string getStringVal(void) const {
		switch(type){
		case FLOAT:
			return std::to_string(float_val);
		case INT:
			return std::to_string((SBFloat)int_val);
		case STRING:
			return string_val;
		default:
			assert(0);
		}
	}

	const std::string getType(void) const{
		if(a)
			return "ARRAY";
		switch(type){
		case FLOAT:
			return "FLOAT";
		case INT:
			return "INT";
		case STRING:
			return "STRING";
		default:
			assert(0);
		}
	}
	
	const int isArray(void) const {
		return a;
	}

};
 std::ostream&operator<<(std::ostream &strm, const Value &val){
	std::string out;
	if(val.isArray()) {
		for(const auto &kp : val.array){
			out += "[" + kp.second.getStringVal()  + "], ";
		}
	}else{
		out = val.getStringVal();
	}	
	return strm << out;	
}

