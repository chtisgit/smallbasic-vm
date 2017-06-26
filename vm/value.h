#include <vector>
#include <functional>
#include <string>
#include <tuple>
#include <iostream>
#include <string.h>
#include <assert.h>

class sb_sf{
private:
	enum sb_Type{sb_FLOAT, sb_STRING};
	sb_Type sb_type;
	double sb_float;
	std::string sb_string;		
public:
	sb_sf(double x){
		sb_type = sb_FLOAT;
		sb_float = x;
	}
	
	sb_sf(std::string x){
		sb_type = sb_STRING;
		sb_string = x;
	}
};

class Value{
private:
	enum Type{FLOAT, STRING, NONE};
	Type type;
	double float_val;
	std::string string_val;
	std::map<int, Value> array;
	bool a = false;
public:

	auto operator[](const int i) -> Value&
	{
		a = true;
		return array[i];
	}

	Value(){
		type = NONE;
	}

	Value(double x){
		type = FLOAT;
		float_val = x;		
	}

	Value(std::string x){
		type = STRING;
		string_val = x;		
	}

	~Value(){
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

	void getValue(void *ptr){
		switch(type){
		case FLOAT:
			*((double *)ptr) = getFloatVal();
			break;
		case STRING:
			*((std::string*)ptr) = getStringVal();
			break;
		default:
			ptr = NULL;
		}
	}
	
	auto getValue(void) -> std::tuple<int, double, const std::string&>{
		return std::make_tuple(int(float_val), float_val, std::ref(string_val));
	}
	
	int getIntVal(void){
		switch(type){
		case FLOAT:
			return int(float_val);
		case STRING:
			return std::stoi(string_val, nullptr, 10);
		case NONE:
			return 0;
		default:
			assert(0);
		}	
	}
	
	double getFloatVal(void){
		switch(type){
		case FLOAT:
			return float_val;
		case STRING:
			std::string::size_type sz;
			return std::stod(string_val, &sz);
		case NONE:
			return 0.0;
		default:
			assert(0);
		}
	}

	std::string getStringVal(void){
		switch(type){
		case FLOAT:
			return std::to_string(float_val);
		case STRING:
			return string_val;
		case NONE:
			return "";
		default:
			assert(0);
		}
	}

};

