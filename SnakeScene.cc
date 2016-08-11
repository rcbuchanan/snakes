#include "irrInc.hh"

#include "Console.hh"
#include "Entity.hh"
#include "MeshBuilder.hh"
#include "SnakeScene.hh"
#include "System.hh"

#include <ostream>
#include <math.h>

template <typename T>
std::ostream &
operator<< (std::ostream &os, const vector3d<T> &v)
{
	os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
	return os;
}

dimension2d<u32>
getNativeResolution ()
{
	IrrlichtDevice *d = createDevice(EDT_NULL);
	dimension2d<u32> r = d->getVideoModeList()->getDesktopResolution();
	d->drop();
	return r;
}

void
snapVector (vector3df &v)
{
	v.X = roundf(v.X);
	v.Y = roundf(v.Y);
	v.Z = roundf(v.Z);
}

u32
SnakeScene::run ()
{
	init();
		
	startclock();
	while (device->run()) {
		const u32 td = tickclock();
		update(td);
		draw();
	}

	return 0;
}

void
SnakeScene::init ()
{
	u32 n = systems.size();
	
	for (u32 i = 0; i < n; i++)
		systems[i]->init();
}
	
void
SnakeScene::update (f32 ms)
{
	u32 n = systems.size();
	
	for (u32 i = 0; i < n; i++)
		systems[i]->update(ms);
}
	
void
SnakeScene::draw ()
{
	deskres = driver->getCurrentRenderTargetSize();
	cursor->setPosition(0.5f, 0.5f);
	smgr->getActiveCamera()->setAspectRatio(deskres.Width / (f32) deskres.Height);
	
	driver->beginScene(true, true, SColor(255, 135, 206, 250));
	smgr->drawAll();
	guienv->drawAll();
	driver->endScene();
}

void
SnakeScene::startclock ()
{
	lastupdate = device->getTimer()->getTime();
}

f32
SnakeScene::tickclock ()
{
	const u32 old = lastupdate;
	lastupdate = device->getTimer()->getTime();
	return (old - lastupdate) / 1.0e9f;
}

SnakeScene::SnakeScene () :
	systems (),
	lastupdate (0),
	meshBuilder (),
	keyMaster (),
	trypush (false)
{

	device = createDevice(EDT_OPENGL, dimension2d<u32>(640, 480), 16, false);
	device->setWindowCaption(L"Snakes");
	device->setEventReceiver(&keyMaster);
		
	driver = this->device->getVideoDriver();
	smgr = this->device->getSceneManager();
	guienv = this->device->getGUIEnvironment();
	filesys = this->device->getFileSystem();
		
	cursor = this->device->getCursorControl();
	cursor->setVisible(false);

	addEntities();
	addSystems();
		
	junk();
}

SnakeScene::~SnakeScene ()
{
	device->drop();
}

void
SnakeScene::addEntities ()
{
	player = new Player();
	activetile = new Entity();
	console = new Console();
}

void
SnakeScene::addSystems ()
{
	systems.push_back(new InputSystem(this));
	systems.push_back(new PhysicsSystem(this));
	systems.push_back(new GraphSystem(this));
	systems.push_back(new ConsoleSystem(this));
}
	
u32
SnakeScene::junk ()
{
	vector3df p = vector3df(0, 80, 0);

	// cube to show light source
	ISceneNode* cube = smgr->addCubeSceneNode(0.125f, 0, -1, p);
	cube->setMaterialType(EMT_SOLID);
	cube->getMaterial(0).EmissiveColor.set(255, 255, 255, 255);
	cube->getMaterial(0).ColorMaterial = ECM_NONE;

	smgr->addLightSceneNode(0, p, SColorf(1.0f, 1.0f, 1.0f), 800.0);
	smgr->setAmbientLight(SColor(255, 70, 30, 30));

	vector3df src = vector3df(0.0f, 0.05f, 0.3f);
	vector3df dst = vector3df(0.0f, 8.05f, 8.7f);
	// IMeshSceneNode *n = meshBuilder.buildSlideNode(smgr, src, dst);
	// n->setVisible(true);
	

	src = vector3df(4.0f, 0.05f, 0.0f);
	dst = vector3df(5.0f, 8.05f, 0.0f);
	IMeshSceneNode *n = meshBuilder.buildLadderNode(smgr, src, dst);
	n->setVisible(true);

	return 0;
}

bool
KeyMaster::OnEvent (const SEvent &event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT)
		keyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

	if (event.EventType == EET_GUI_EVENT)
		eboxEnter = event.GUIEvent.EventType == EGET_EDITBOX_ENTER;
	else
		eboxEnter = false;

		
	return false;
}

KeyMaster::KeyMaster ()
{
	for (s32 i = 0; i < KEY_KEY_CODES_COUNT; i++)
		keyIsDown[i] = false;
}
	
KeyMaster::~KeyMaster ()
{
}

int
main ()
{
	SnakeScene scene;
	return scene.run();
	return 0;
}
