#pragma once
#include "sbas.h"

class AssemblerError : public std::exception{
	std::string msg;
	std::string path;
	int line;

public:
	explicit AssemblerError(const char* message, const std::string& path, int N) noexcept
		: msg(message), path(path), line(N)
	{
	}
	explicit AssemblerError(const std::string& message, const std::string& path, int N) noexcept
		: AssemblerError(message.c_str(),path,N)
	{
	}
	explicit AssemblerError(const char* message, const AssemblyFile& asmfile) noexcept
		: AssemblerError(message,*asmfile.path,asmfile.line)
	{
	}
	explicit AssemblerError(const std::string& message, const AssemblyFile& asmfile) noexcept
		: AssemblerError(message.c_str(),asmfile)
	{
	}

	virtual ~AssemblerError() {}

	virtual auto what() const noexcept -> const char*
	{
		return msg.c_str();
	}

	virtual auto what_line() const noexcept -> int
	{
		return line;
	}

	virtual auto what_file() const noexcept -> const char*
	{
		return path.c_str();
	}
};

