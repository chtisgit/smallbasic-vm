#pragma once

#include "state.h"

#include <vector>

using ObjectFunction = int(*)(VMState&);
using ObjectID = uint_least32_t;

#include "obj0_textwindow.h"

constexpr ObjectID object_id(int obj, int fun){
	static_assert(std::is_unsigned<ObjectID>::value, "");
	return (ObjectID(fun) << 16) | ObjectID(obj);
}

std::map<uint_least32_t,ObjectFunction> sb_objects = {

	// Object 0 - TextWindow
	{object_id(0,0),ObjTextWindow::ForegroundColorSet},
	{object_id(0,1),ObjTextWindow::ForegroundColorGet},
	{object_id(0,2),ObjTextWindow::BackgroundColorSet},
	{object_id(0,3),ObjTextWindow::BackgroundColorGet},
	{object_id(0,4),ObjTextWindow::CursorLeftSet},
	{object_id(0,5),ObjTextWindow::CursorLeftGet},
	{object_id(0,6),ObjTextWindow::CursorTopSet},
	{object_id(0,7),ObjTextWindow::CursorTopGet},
	{object_id(0,8),ObjTextWindow::LeftSet},
	{object_id(0,9),ObjTextWindow::LeftGet},
	{object_id(0,10),ObjTextWindow::TopSet},
	{object_id(0,11),ObjTextWindow::TopGet},
	{object_id(0,12),ObjTextWindow::TitleSet},
	{object_id(0,13),ObjTextWindow::TitleGet},
	{object_id(0,14),ObjTextWindow::Show},
	{object_id(0,15),ObjTextWindow::Hide},
	{object_id(0,16),ObjTextWindow::Clear},
	{object_id(0,17),ObjTextWindow::Pause},
	{object_id(0,18),ObjTextWindow::PauseIfVisible},
	{object_id(0,19),ObjTextWindow::PauseWithoutMessage},
	{object_id(0,20),ObjTextWindow::Read},
	{object_id(0,21),ObjTextWindow::ReadKey},
	{object_id(0,22),ObjTextWindow::ReadNumber},
	{object_id(0,23),ObjTextWindow::WriteLine},
	{object_id(0,24),ObjTextWindow::Write}

};

