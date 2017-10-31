#pragma once

#include "state.h"

// TODO
// in future this file should provide an interface for loading
// arbitrary objects (as shared libraries) and install them
// in the VM on-the-fly when the ObjectID is first accessed
//
// at the moment the implementation of the objects is hard-coded

#include <vector>

using ObjectFunction = int(*)(VMState&);
using ObjectID = uint_least32_t;

#include "obj0_textwindow.h"
#include "obj1_array.h"
#include "obj2_text.h"

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
	{object_id(0,24),ObjTextWindow::Write},

	// Object 1 - Array
	{object_id(1,0),ObjArray::ContainsIndex},
	{object_id(1,1),ObjArray::ContainsValue},
	{object_id(1,2),ObjArray::GetAllIndices},
	{object_id(1,3),ObjArray::GetItemCount},
	{object_id(1,4),ObjArray::IsArray},
	{object_id(1,5),ObjArray::SetValue},
	{object_id(1,6),ObjArray::GetValue},
	{object_id(1,7),ObjArray::RemoveValue},

	// Object 2 - Text
	{object_id(2,0),ObjText::Append},
	{object_id(2,1),ObjText::GetLength},
	{object_id(2,2),ObjText::IsSubText},
	{object_id(2,3),ObjText::EndsWith},
	{object_id(2,4),ObjText::StartsWith},
	{object_id(2,5),ObjText::GetSubText},
	{object_id(2,6),ObjText::GetSubTextToEnd},
	{object_id(2,7),ObjText::GetIndexOf},
	{object_id(2,8),ObjText::ConvertToLowerCase},
	{object_id(2,9),ObjText::ConvertToUpperCase},
	{object_id(2,10),ObjText::GetCharacter},
	{object_id(2,11),ObjText::GetCharacterCode}
	

};

