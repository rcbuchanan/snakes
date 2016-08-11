#include "irrInc.hh"

#include <lua5.3/lua.hpp>

#include "Console.hh"
#include "Entity.hh"
#include "MeshBuilder.hh"
#include "SnakeScene.hh"
#include "System.hh"


void
InputSystem::init ()
{
}

void
PhysicsSystem::init ()
{
}

void
GraphSystem::init ()
{	
	player->camera = smgr->addCameraSceneNode(
		0,
		player->position,
		player->position + player->fwd);
	player->camera->setNearValue(0.1);
	player->camera->setFarValue(20000);
	player->node = player->camera;

	activetile->node = meshBuilder.buildTileNode(smgr);
}

void
InputSystem::update (f32 ms)
{
	vector3df fwd = player->fwd;
	vector3df right = player->fwd.crossProduct(player->up);
	vector3df up = right.crossProduct(fwd);

	if (console->isActive)
		return;
		
	player->velocity.set(0, 0, 0);
	if (keyMaster.keyIsDown[KEY_COMMA]) player->velocity += fwd;
	if (keyMaster.keyIsDown[KEY_KEY_O]) player->velocity -= fwd;
	if (keyMaster.keyIsDown[KEY_PERIOD]) player->velocity += up;
	if (keyMaster.keyIsDown[KEY_OEM_7]) player->velocity -= up; /* single quote*/
	if (keyMaster.keyIsDown[KEY_KEY_A]) player->velocity += right;
	if (keyMaster.keyIsDown[KEY_KEY_E]) player->velocity -= right;
	player->velocity.normalize();
	player->velocity *= 10;

	const position2d<f32> mid = position2d<f32>(0.5f, 0.5f);
	player->mouseLook(mid - cursor->getRelativePosition());

	trypush = keyMaster.keyIsDown[KEY_SPACE];

	activetile->position = player->position + 2 * player->fwd - player->up;
	activetile->position.X = roundf(activetile->position.X);
	activetile->position.Y = roundf(activetile->position.Y);
	activetile->position.Z = roundf(activetile->position.Z);
}

void
PhysicsSystem::update (f32 ms)
{	
	player->basicPhysics(ms);
}

void
GraphSystem::update (f32 ms)
{
	bool tiledup = tiles.find(activetile->position) != NULL;
	//activetile->node->setVisible(!tiledup);
	if (trypush && !tiledup) {
		tiles[activetile->position] = activetile;
		activetile = new Entity();
		activetile->node = meshBuilder.buildTileNode(smgr);
	}

	player->updateGraph(ms);
	activetile->updateGraph(ms);

	map<vector3df, Entity *>::Iterator it;
	for (it = tiles.getIterator(); !it.atEnd(); it++)
		(*it).getValue()->updateGraph(ms);
}

void
ConsoleSystem::init ()
{
	console->makegui(guienv);
	console->startlua();
}

void
ConsoleSystem::update (f32 ms)
{	
	if (console->isActive)
		guienv->setFocus(console->ebox);
	else
		guienv->setFocus(console->stxt);

	if (console->isActive) {
		if (keyMaster.keyIsDown[KEY_ESCAPE])
			console->hide();
		else if (keyMaster.eboxEnter)
			console->enter();
	} else if (keyMaster.keyIsDown[KEY_TAB]) {
		console->show(deskres.Width, deskres.Height);
	}

	const array<vector3df> tl = console->getTileList();
	u32 N = tl.size();
	luaL_dostring(console->state, "tiles = nil");

	for (u32 i = 0; i < N; i++) {
		bool tiledup = tiles.find(tl[i]) != NULL;
		if (!tiledup) {
			Entity *e = new Entity();

			e->position = tl[i];
			e->node = meshBuilder.buildTileNode(smgr);
			e->node->setVisible(true);
			tiles[tl[i]] = e;
		}
	}

	const array<vector3df> slides = console->getSlideList();
	N = slides.size();
	luaL_dostring(console->state, "slides = nil");
	for (u32 i = 0; i < N; i += 2) {
		ISceneNode *n = meshBuilder.buildSlideNode(smgr, slides[i], slides[i + 1]);
		n->setVisible(true);
	}

	const array<vector3df> ladders = console->getLadderList();
	N = ladders.size();
	luaL_dostring(console->state, "ladders = nil");
	for (u32 i = 0; i < N; i += 2) {
		printf("%d\n", i);
		ISceneNode *n = meshBuilder.buildLadderNode(smgr, ladders[i], ladders[i + 1]);
		n->setVisible(true);
	}

}

