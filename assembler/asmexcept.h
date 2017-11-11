#pragma once

class AssemblerError : public std::exception{
	std::string msg;
	int line_n;

public:
	explicit AssemblerError(const char* message, int N) noexcept
		: msg(message), line_n(N)
	{
	}
	explicit AssemblerError(std::string message, int N) noexcept
		: AssemblerError(message.c_str(),N)
	{
	}

	virtual ~AssemblerError() {}

	virtual auto what() const noexcept -> const char*
	{
		return msg.c_str();
	}

	virtual auto what_line() const noexcept -> int
	{
		return line_n;
	}
};

