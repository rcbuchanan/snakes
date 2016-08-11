#include "irrInc.hh"

#include "Console.hh"

#include <lua5.3/lua.hpp>

#include <iostream>


static vector3df
readvector (lua_State *state, u32 i)
{
	//std::cout << lua_typename(state, lua_type(state, -1)) << "\n";
	//std::cout << lua_typename(state, lua_type(state, -2)) << "\n\n";

	lua_geti(state, -1, i);
	lua_getfield(state, -1, "x");
	lua_getfield(state, -2, "y");
	lua_getfield(state, -3, "z");
	vector3df rv(lua_tonumber(state, -3), lua_tonumber(state, -2), lua_tonumber(state, -1));
	lua_pop(state, 4);
	return rv;	    
}

static void
showerror (lua_State *state) {
	//stringw s(stxt->getText());
	//s = (s + "\n") + lua_tostring(state, -1);
	//stxt->setText(s.c_str());
	//lua_pop(state, 1);
	std::cout << lua_tostring(state, -1) << "\n";
	lua_pop(state, 1);

}

void
Console::makegui (IGUIEnvironment *guienv)
{
	guienv->getSkin()->setFont(guienv->getFont("./fonts/fontcourier.bmp"));
	guienv->getSkin()->setColor(EGDC_BUTTON_TEXT, SColor(255, 63, 255, 63));
		
	stxt = guienv->addStaticText(L"STATIC TEXT", rect<s32>(), true);
	stxt->setBackgroundColor(SColor(127, 0, 0, 0));
	stxt->setVisible(false);
		
	ebox = guienv->addEditBox(L"", rect<s32>(), true);
	guienv->getSkin()->setColor(EGDC_FOCUSED_EDITABLE, SColor(127, 0, 0, 0));
	ebox->setVisible(false);
}

void
Console::startlua ()
{
	state = luaL_newstate();
	luaL_openlibs(state);
	u32 rv = luaL_dofile(state, "./scripts/world.lua");
	if (rv) showerror(state);

}

void
Console::enter ()
{
	stringw str = stxt->getText();
	u32 rv = luaL_dostring(state, stringc(ebox->getText()).c_str());
	if (rv) showerror(state);
	ebox->setText(L"");
}

array<vector3df>
Console::getTileList ()
{
	array<vector3df> rv;

	lua_getglobal(state, "tiles");
	if (lua_istable(state, -1)) {
	 	const u32 N = luaL_len(state, -1);
		std::cout << "FOUND " << N << " tiles\n";
		for (u32 i = 0; i < N; i++)
	 		rv.push_back(readvector(state, i + 1));
	}
	lua_pop(state, 1);

	return rv;
}

array<vector3df>
Console::getSlideList ()
{
	array<vector3df> rv;

	lua_getglobal(state, "slides");
	if (lua_istable(state, -1)) {
	 	const u32 N = luaL_len(state, -1);
		for (u32 i = 0; i < N; i++)
	 		rv.push_back(readvector(state, i + 1));
		
	 	//for (u32 i = 0; i < N; i++)
		//std::cout << rv[i].X << " " << rv[i].Y << " " << rv[i].Z << "\n";
	}
	lua_pop(state, 1);

	return rv;
}

array<vector3df>
Console::getLadderList ()
{
	array<vector3df> rv;

	lua_getglobal(state, "ladders");
	if (lua_istable(state, -1)) {
	 	const u32 N = luaL_len(state, -1);
		for (u32 i = 0; i < N; i++)
	 		rv.push_back(readvector(state, i + 1));
		
	 	//for (u32 i = 0; i < N; i++)
		//std::cout << rv[i].X << " " << rv[i].Y << " " << rv[i].Z << "\n";
	}
	lua_pop(state, 1);

	return rv;
}

void
Console::show (u32 W, u32 H)
{
	const s32 th = 100;

	isActive = true;

	stxt->setRelativePosition(rect<s32>(0, 0, W, H - th));
	stxt->updateAbsolutePosition();
	stxt->setVisible(true);
			
	ebox->setRelativePosition(rect<s32>(0, H - th, W, H));
	ebox->updateAbsolutePosition();
	ebox->setVisible(true);
}

void
Console::hide ()
{
	isActive = false;
	ebox->setVisible(false);
	stxt->setVisible(false);
}
