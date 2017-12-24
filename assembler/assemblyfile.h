#pragma once

#include <memory>

struct AssemblyFile{
	std::shared_ptr<std::string> path;
	int line;
	std::shared_ptr<std::istream> stream;

	AssemblyFile(const char* p, int l, std::shared_ptr<std::istream> s)
		: path(std::make_shared<std::string>(p)), line(l), stream(std::move(s))
	{
	}
	AssemblyFile(const char* p, int l, std::istream* s)
		: AssemblyFile(p,l,std::shared_ptr<std::istream>(s))
	{
	}

	AssemblyFile(const AssemblyFile&) = delete;
	AssemblyFile(AssemblyFile&&) noexcept = default;
};


