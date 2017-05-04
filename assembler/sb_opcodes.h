#pragma once

#include <map>
#include <cstdint>

std::map<std::string, uint8_t> mnemonic_table = {
	{"copy", 0},
	{"movi", 1},
	{"jmp", 2},
	{"push", 3},
	{"pushi", 4}
};


