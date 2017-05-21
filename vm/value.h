#include <vector>

#include <string>
#include <tuple>
#include <iostream>
#include <string.h>
#include <assert.h>


class Value{
private:
	enum Type{INT, FLOAT, STRING, NONE};
	Type type;
	int int_val;
	float float_val;
	std::string string_val;
public:

	Value(){
		type = NONE;
	}

	Value(int x){
		type = INT;
		int_val = x;		
	}
	
	Value(float x){
		type = FLOAT;
		float_val = x;		
	}

	Value(std::string x){
		type = STRING;
		string_val = x;		
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
		switch(type){
		case INT:
			return std::get<INT>(getValue());
			break;
		case FLOAT:
			return (int)std::get<FLOAT>(getValue());
			break;
		case STRING:
			return std::stoi(std::get<STRING>(getValue()), nullptr, 10);
			break;
		case NONE:
			return 0;
			break;
		default:
			assert(0);
		}	
	}
	
	float getFloatVal(void){
		switch(type){
		case INT:
			return (float)std::get<INT>(getValue());
			break;
		case FLOAT:
			return std::get<FLOAT>(getValue());
			break;
		case STRING:
			std::string::size_type sz;
			return std::stof(std::get<STRING>(getValue()), &sz);
			break;
		case NONE:
			return 0.0;
			break;
		default:
			assert(0);
		}
	}

	std::string getStringVal(void){
		switch(type){
		case INT:
			return std::to_string(std::get<INT>(getValue()));
			break;
		case FLOAT:
			return std::to_string(std::get<FLOAT>(getValue()));
			break;
		case STRING:
			return std::get<STRING>(getValue());
			break;
		case NONE:
			return "";
			break;
		default:
			assert(0);
		}
	}

};

