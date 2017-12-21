#pragma once
#include <map>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <unistd.h>
#include <dlfcn.h>

#define OBJECT_FILENAME_PREFIX		"object"
#define OBJ_REL_PATH			"../objects/"

class VMState;

struct ObjectCollection{
	using ID = uint_least32_t;
	using ObjectFunction = int(*)(VMState&);

	static constexpr ID GetID(int obj, int fun){
		static_assert(std::is_unsigned<ID>::value, "");
		return (ID(fun) << 16) | ID(obj);
	}

	static constexpr int Major(ID id){
		return id & 0xFFFF;
	}

	static constexpr int Minor(ID id){
		return id >> 16;
	}

	~ObjectCollection()
	{
		UnloadAll();
	}

	auto Load(int obj) -> void
	{
		auto path = ObjPath()+OBJECT_FILENAME_PREFIX+std::to_string(obj)+".so";
		void* lib = dlopen(path.c_str(),RTLD_LAZY);
		if(lib == nullptr){
			printf("fatal error: cannot find shared library for object %d\n",obj);
			abort();
		}
		void **functions;
		char **names;
		char sym[40];
		sprintf(sym,"Object%dFunctions",obj);
		functions = static_cast<void**>(dlsym(lib, sym));
		sprintf(sym,"Object%dFunctionNames",obj);
		names = static_cast<char**>(dlsym(lib, sym));
		if(functions == nullptr || names == nullptr){
			printf("fatal error: cannot load shared library for object %d\n",obj);
			abort();
		}

		int fun;
		for(fun = 0; fun <= 0xFFFF && functions[fun] != nullptr; fun++){
			funcs[GetID(obj,fun)] = reinterpret_cast<ObjectFunction>(functions[fun]);
		}
		if(fun == 0xFFFF){
			printf("fatal error: cannot load shared library for object %d\n",obj);
			abort();
		}

		openlibs[obj] = lib;
	}

	auto UnloadAll() -> void
	{
		for(auto& p : openlibs){
			dlclose(p.second);
		}
	}

	auto Call(int obj, int fun, VMState& state) -> int
	{
		try{
			return funcs.at(GetID(obj,fun))(state);
		}catch(std::out_of_range& e){
			Load(obj);
			return funcs[GetID(obj,fun)](state);
		}
	}

	static auto ObjPath() -> const std::string&
	{
		static std::string ownpath;

#ifdef OBJ_LIBS_PATH
		if(ownpath.size() == 0){
			ownpath = OBJ_LIBS_PATH;
		}
#else
		if(ownpath.size() == 0){
#ifndef __unix__
#warning "ObjPath: no implementation for non-unix systems"
#else // __unix__
			char buf[50];
			char path[PATH_MAX+1];
			sprintf(buf,"/proc/%d/exe",getpid());
			auto i = readlink(buf, path, PATH_MAX);
			path[i] = '\0';
			ownpath = path;
			ownpath.erase(ownpath.rfind('/')+1);
			ownpath += OBJ_REL_PATH;
#endif
		}
#endif
		return ownpath;
	}
private:
	std::map<ID,ObjectFunction> funcs;

	// map of object id to shared library handle
	std::map<int,void*> openlibs;

};

static_assert(ObjectCollection::Major(ObjectCollection::GetID(0xC0FF,0xFEFE)) == 0xC0FFu, "");
static_assert(ObjectCollection::Minor(ObjectCollection::GetID(0xC0FF,0xFEFE)) == 0xFEFEu, "");
