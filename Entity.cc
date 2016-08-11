#include "irrInc.hh"

#include "Entity.hh"

void
Entity::basicPhysics (f32 delta)
{
	position += velocity * delta / 1000.0f;
}

void
Entity::updateGraph (f32 delta)
{
	node->setPosition(position);
}

void
Player::updateGraph (f32 delta)
{
	camera->setTarget(camera->getAbsolutePosition() + fwd);
	camera->setUpVector(up);
	Entity::updateGraph(delta);
}

void
Player::mouseLook (position2d<f32> cursorMove)
{
	// derived from irrlicht's FPS camera code
	vector3df relativeRotation = fwd.getHorizontalAngle();
		
	if (cursorMove.getLength() != 0.0f) {
		relativeRotation.Y -= cursorMove.X * rotateSpeed;
		relativeRotation.X -= cursorMove.Y * rotateSpeed;

		// X < MaxVerticalAngle or X > 360-MaxVerticalAngle
		if (relativeRotation.X > maxVerticalAngle * 2 &&
		    relativeRotation.X < 360.0f - maxVerticalAngle) {
			relativeRotation.X = 360.0f - maxVerticalAngle;
		} else if (relativeRotation.X > maxVerticalAngle &&
		    relativeRotation.X < 360.0f - maxVerticalAngle) {
			relativeRotation.X = maxVerticalAngle;
		}

		matrix4 mat;
		mat.setRotationDegrees(relativeRotation);
		fwd = vector3df(0.0f, 0.0f, 1.0f);
		mat.transformVect(fwd);
	}
}

Player::Player ()
{
	rotateSpeed = 100.0f;
	maxVerticalAngle = 88.0f;
	up = vector3df(0.0f, 1.0f, 0.0f);
	fwd = vector3df(0.0f, 0.0f, 1.0f);
}

Player::~Player ()
{
}
