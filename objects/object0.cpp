#include <cstdio>
#include <termios.h>

#include <state.h>

namespace ObjTextWindow{

// Properties
int ForegroundColorSet(VMState& state){ return 0; }
int ForegroundColorGet(VMState& state){ return 0; }
int BackgroundColorSet(VMState& state){ return 0; }
int BackgroundColorGet(VMState& state){ return 0; }
int CursorLeftSet(VMState& state){ return 0; }
int CursorLeftGet(VMState& state){ return 0; }
int CursorTopSet(VMState& state){ return 0; }
int CursorTopGet(VMState& state){ return 0; }
int LeftSet(VMState& state){ return 0; }
int LeftGet(VMState& state){ return 0; }
int TopSet(VMState& state){ return 0; }
int TopGet(VMState& state){ return 0; }
int TitleSet(VMState& state){ return 0; }
int TitleGet(VMState& state){ return 0; }

// Functions

int Show(VMState& state){ return 0; }
int Hide(VMState& state){ return 0; }
int Clear(VMState& state)
{
	// FIXME: working as expected?
	printf("\f");
	return 1;
}
static void pause(bool msg){
	static const char PAUSE_MSG[] = "Press any key to continue...";
	struct termios info;
	tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
	auto vtime = info.c_cc[VTIME];
	auto vmin = info.c_cc[VMIN];
	info.c_lflag &= ~ICANON;      /* disable canonical mode */
	info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
	info.c_cc[VTIME] = 0;         /* no timeout */
	tcsetattr(0, TCSANOW, &info); /* set immediately */

	if(msg){
		printf("%s\n", PAUSE_MSG);
	}
	getchar();

	info.c_lflag |= ICANON;       /* enable canonical mode */
	info.c_cc[VMIN] = vmin;
	info.c_cc[VTIME] = vtime;
	tcsetattr(0, TCSANOW, &info);
	
}
int Pause(VMState& state)
{
	pause(true);
	return 1;
}
int PauseIfVisible(VMState& state)
{
	pause(true);
	return 1;
}
int PauseWithoutMessage(VMState& state)
{
	pause(false);
	return 1;
}
int Read(VMState& state)
{
	char buf[200];
	bool term = false;
	std::string str;
	do{
		auto* s = fgets(buf, 200, stdin);
		if(s == nullptr) break;
		auto* nl = strchr(buf, '\n');
		if(nl != nullptr){
			*nl = '\0';
			term = true;
		}
#ifndef __unix__
		nl = strchr(buf, '\r');
		if(nl != nullptr){
			*nl = '\0';
			term = true;
		}
#endif
		str += buf;
	}while(!term);
	state.stack.push_back(str);
	return 1;
}
int ReadKey(VMState& state)
{
	state.stack.push_back(getchar());
	return 1;
}
int ReadNumber(VMState& state)
{
	char buf[200], *endptr;
	fgets(buf, 200, stdin);
	state.stack.push_back(strtod(buf, &endptr));
	// TODO: maybe check if endptr == '\0'
	return 1;
}
int WriteLine(VMState& state)
{
	if(state.stack.empty()) return 0;
	auto str = state.stack.back();
	state.stack.pop_back();
	printf("%s\n", str.getStringVal().c_str());
	return 1;
}
int Write(VMState& state)
{
	if(state.stack.empty()) return 0;
	auto str = state.stack.back();
	state.stack.pop_back();
	printf("%s", str.getStringVal().c_str());
	fflush(stdout);
	return 1;
}


};

extern "C"{

	/* exported symbols */

	const void *Object0Functions[] = {
		reinterpret_cast<void*>(ObjTextWindow::ForegroundColorSet),
		reinterpret_cast<void*>(ObjTextWindow::ForegroundColorGet),
		reinterpret_cast<void*>(ObjTextWindow::BackgroundColorSet),
		reinterpret_cast<void*>(ObjTextWindow::BackgroundColorGet),
		reinterpret_cast<void*>(ObjTextWindow::CursorLeftSet),
		reinterpret_cast<void*>(ObjTextWindow::CursorLeftGet),
		reinterpret_cast<void*>(ObjTextWindow::CursorTopSet),
		reinterpret_cast<void*>(ObjTextWindow::CursorTopGet),
		reinterpret_cast<void*>(ObjTextWindow::LeftSet),
		reinterpret_cast<void*>(ObjTextWindow::LeftGet),
		reinterpret_cast<void*>(ObjTextWindow::TopSet),
		reinterpret_cast<void*>(ObjTextWindow::TopGet),
		reinterpret_cast<void*>(ObjTextWindow::TitleSet),
		reinterpret_cast<void*>(ObjTextWindow::TitleGet),
		reinterpret_cast<void*>(ObjTextWindow::Show),
		reinterpret_cast<void*>(ObjTextWindow::Hide),
		reinterpret_cast<void*>(ObjTextWindow::Clear),
		reinterpret_cast<void*>(ObjTextWindow::Pause),
		reinterpret_cast<void*>(ObjTextWindow::PauseIfVisible),
		reinterpret_cast<void*>(ObjTextWindow::PauseWithoutMessage),
		reinterpret_cast<void*>(ObjTextWindow::Read),
		reinterpret_cast<void*>(ObjTextWindow::ReadKey),
		reinterpret_cast<void*>(ObjTextWindow::ReadNumber),
		reinterpret_cast<void*>(ObjTextWindow::WriteLine),
		reinterpret_cast<void*>(ObjTextWindow::Write),
		nullptr
	};

	const char *Object0FunctionNames[] = {
		nullptr
	};

}


