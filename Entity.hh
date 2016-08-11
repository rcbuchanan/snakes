#ifndef ENTITY_HH_
#define ENTITY_HH_

#include "irrInc.hh"

class Entity {
public:	
	void basicPhysics (f32 delta);
	virtual void updateGraph (f32 delta);
	
	vector3df velocity;
	vector3df position;
	ISceneNode *node;
};

class Player : public Entity {
public:
	virtual void updateGraph (f32 delta);
	void mouseLook (position2d<f32> cursorMove);

	Player ();
	~Player ();

	vector3df up;
	vector3df fwd;
	
	ICameraSceneNode *camera;
	f32 rotateSpeed;
	f32 maxVerticalAngle;
};

#endif
