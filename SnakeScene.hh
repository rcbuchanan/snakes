#ifndef SNAKESCENE_HH_
#define SNAKESCENE_HH_

#include "irrInc.hh"

class Console;
class Player;
class Entity;
class MeshBuilder;
class System;

class KeyMaster : public IEventReceiver {
public:
 	virtual bool OnEvent (const SEvent &event);
	KeyMaster ();
	~KeyMaster ();
	
	bool keyIsDown[KEY_KEY_CODES_COUNT];
	bool eboxEnter;
};

// Scene state
class SnakeScene {
public:
	u32 run ();

	void updateConsole (f32 ms);

	void init ();
	
	void update (f32 ms);
	
	void draw ();

	void startclock ();

	f32 tickclock ();

	SnakeScene ();

	~SnakeScene ();

	void addEntities ();
	void addSystems ();
	
	u32 junk ();

	array<System *> systems;

	u32 lastupdate;
	dimension2d<u32> deskres;
	
	IrrlichtDevice *device;
	IVideoDriver *driver;
	ISceneManager *smgr;
	IGUIEnvironment *guienv;
	IFileSystem *filesys;
	ICursorControl *cursor;
	
	MeshBuilder meshBuilder;

	KeyMaster keyMaster;

	Player *player;
	Entity *activetile;
	bool trypush;
	map<vector3df, Entity *> tiles;

	Console *console;

};

#endif
