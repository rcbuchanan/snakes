#ifndef CONSOLE_HH_
#define CONSOLE_HH_

#include "irrInc.hh"

struct lua_State;

class Console {
public:
	void makegui (IGUIEnvironment *guienv);
	void startlua ();
	void show (u32 W, u32 H);
	void hide ();
	void enter ();
	array<vector3df> getTileList ();
	array<vector3df> getSlideList ();
	array<vector3df> getLadderList ();


	stringw txt;

	lua_State *state;

	IGUIStaticText *stxt;
	IGUIEditBox *ebox;
	bool isActive;
};

#endif
