#ifndef MESHBUILDER_HH_
#define MESHBUILDER_HH_

#include "irrInc.hh"

class UVSurface {
public:
	virtual vector3df getPosition (f32 u, f32 v) const = 0;
	
	vector3df getNormal (f32 u, f32 v) const;
	S3DVertex getVertex (f32 u, f32 v, SColor col = SColor()) const;

	UVSurface (
		const vector3df &src,
		const vector3df &dst,
		const vector3df &up = vector3df(0, 1, 0));
	
	vector3df O;
	vector3df X; // local X -- points "right"
	vector3df Y; // local Y -- points "up"
	vector3df Z; // local Z -- points "into"
	f32 Height;
	f32 Length;
	f32 Delta;
	u32 Segments;
};

class SineSurface : public UVSurface {
public:
	virtual vector3df getPosition (f32 u, f32 v) const;

	SineSurface (
		const vector3df &src,
		const vector3df &dst,
		const vector3df &up = vector3df(0, 1, 0),
		const u32 n = 1);
	
	u32 count;
};

class LineSurface : public UVSurface {
public:
	virtual vector3df getPosition (f32 u, f32 v) const;

	LineSurface (
		const vector3df &src,
		const vector3df &dst,
		const vector3df &up = vector3df(0, 1, 0));
	
	u32 count;
};

class MeshBuilder {
public:
	IMeshSceneNode *buildTileNode (ISceneManager *smgr) const;

	IMeshSceneNode *buildLadderNode (
		ISceneManager *smgr,
		const vector3df &src,
		const vector3df &dst) const;
	
	IMeshSceneNode *buildSlideNode (
		ISceneManager *smgr,
		const vector3df &src,
		const vector3df &dst) const;

private:
	IMeshSceneNode *buildExtendMeshNode (
		ISceneManager *smgr,
		const char *name,
		const UVSurface &surf,
		const f32 zscale = 1.0f) const;
	
	SMesh *buildExtendedMesh (
		const IMeshBuffer *midbuf,
		const IMeshBuffer *endbuf,
		const UVSurface &surf,
		const f32 zscale) const;

};

#endif
