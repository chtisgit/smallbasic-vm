#include <vector>

#include <string>
#include <tuple>
#include <iostream>
#include <string.h>

class Value{
private:
enum Type{INT, FLOAT, STRING, NONE};
std::tuple<int, float, std::string> value;
int int_val;
float float_val;
std::string string_val;
public:
	Type type;

	Value(){
		type = NONE;
	}

	Value(int x){
		type = INT;
		std::get<INT>(value) = x;		
	}
	
	Value(float x){
		type = FLOAT;
		std::get<FLOAT>(value) = x;		
	}

	Value(std::string x){
		type = STRING;
		std::get<STRING>(value) = x;		
	}

	~Value(){
	}

	void getValue(void *ptr){
		switch(type){
		case INT:
			*((int *)ptr) = getIntVal();
			break;
		case FLOAT:
			*((float *)ptr) = getFloatVal();
			break;
		case STRING:
			*((std::string*)ptr) = getStringVal();
			break;
		default:
			ptr = NULL;
		}
	}
	
	auto getValue(void) -> std::tuple<int, float, std::string>{
		return std::make_tuple(int_val, float_val, string_val);	
	}
	
	int getIntVal(void){
		return std::get<INT>(value);
	}
	
	float getFloatVal(void){
		return std::get<FLOAT>(value);
	}

	std::string getStringVal(void){
		return std::get<STRING>(value);
	}

};

