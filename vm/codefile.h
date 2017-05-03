#pragma once

#include <iostream>
#include <stdexcept>

#include <cassert>
#include <cstdint>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CODEFILE_ERROR	"could not open bytecode file!"

class CodeFile{
	const std::string filename;
	uint8_t *file;
	size_t file_len;
public:
	CodeFile(std::string n)
		: filename(n), file(nullptr)
	{
		struct stat statbuf;

		if(stat(filename.c_str(), &statbuf) != 0){
			throw std::runtime_error(CODEFILE_ERROR);
		}
		file_len = statbuf.st_size;

		auto fd = open(filename.c_str(), O_RDONLY);
		if(fd == -1){
			throw std::runtime_error(CODEFILE_ERROR);
		}
		file = static_cast<uint8_t*>(mmap(nullptr, file_len,
			PROT_READ, MAP_PRIVATE, fd, 0));

		if(file == MAP_FAILED){
			throw std::runtime_error(CODEFILE_ERROR);
		}
		close(fd);

		/* constructor must always throw if the
		 * file could not be mapped correctly */
		assert(file != nullptr);
	}

	CodeFile(const CodeFile&) = delete;
	CodeFile(CodeFile&& o)
		: filename(std::move(o.filename)),
		file(std::move(o.file)),
		file_len(std::move(o.file_len))
	{
		o.file = nullptr;
	}

	~CodeFile()
	{
		munmap(file, file_len);
	}

	const uint8_t* code() const
	{
		return file;
	}

	size_t size() const
	{
		return file_len;
	}
};
