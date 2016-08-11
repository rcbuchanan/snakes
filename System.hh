#ifndef SYSTEM_HH_
#define SYSTEM_HH_

#include "irrInc.hh"

class SnakeScene;

class System {
public:
	virtual void update (f32 ms) = 0;
	virtual void init () = 0;
};

class InputSystem : public System {
public:
	virtual void update (f32 ms);
	virtual void init ();
	InputSystem (SnakeScene *scene) :
		keyMaster (scene->keyMaster),
		player (scene->player),
		activetile (scene->activetile),
		cursor (scene->cursor),
		console (scene->console),
		trypush (scene->trypush) {}
private:
	KeyMaster &keyMaster;
	Player *&player;
	Entity *&activetile;
	ICursorControl *&cursor;
	Console *&console;
	bool &trypush;
};

class ConsoleSystem : public System {
public:
	virtual void update (f32 ms);
	virtual void init ();
	ConsoleSystem (SnakeScene *scene) :
		console (scene->console),
		guienv (scene->guienv),
		keyMaster (scene->keyMaster),
		deskres (scene->deskres),
		tiles (scene->tiles),
		meshBuilder (scene->meshBuilder),
		smgr (scene->smgr) {}
private:
	Console *&console;
	IGUIEnvironment *&guienv;
	KeyMaster &keyMaster;
	dimension2d<u32> &deskres;
	map<vector3df, Entity *> &tiles;
	MeshBuilder &meshBuilder;
	ISceneManager *&smgr;
};

class PhysicsSystem : public System {
public:
	virtual void update (f32 ms);
	virtual void init ();
	PhysicsSystem (SnakeScene *scene) : player (scene->player) {}
private:
	Player *&player;
};

class GraphSystem : public System {
public:
	virtual void update (f32 ms);
	virtual void init ();
	GraphSystem (SnakeScene *scene) :
		player (scene->player),
		activetile (scene->activetile),
		tiles (scene->tiles),
		meshBuilder (scene->meshBuilder),
		trypush (scene->trypush),
		smgr (scene->smgr) {}

private:
	Player *&player;
	Entity *&activetile;
	map<vector3df, Entity *> &tiles;
	MeshBuilder &meshBuilder;
	bool &trypush;
	ISceneManager *&smgr;
};

#endif
