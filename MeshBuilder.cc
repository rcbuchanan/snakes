#include "irrInc.hh"

#include "MeshBuilder.hh"

static void
appendToCMesh (CMeshBuffer<S3DVertex> *a, const IMeshBuffer *b, const matrix4 &mat)
{
	u32 nverts = b->getVertexCount();
	array<S3DVertex> verts = array<S3DVertex>(nverts);

	matrix4 matinv;
	mat.getInverse(matinv);
	matinv = matinv.getTransposed();
	
	for (u32 i = 0; i < nverts; i++) {
		verts[i].Pos = b->getPosition(i);
		verts[i].Normal = b->getNormal(i);
		mat.transformVect(verts[i].Pos);
	}
	a->append(verts.pointer(), nverts, b->getIndices(), b->getIndexCount());
}

static matrix4
getBasisMatrix (
	const vector3df &x,
	const vector3df &y,
	const vector3df &z,
	const vector3df &o)
{
	matrix4 rv;
	f32 *p = rv.pointer();

	// column 1
	p[0] = x.X;
	p[1] = x.Y;
	p[2] = x.Z;
	// column 2
	p[4] = y.X;
	p[5] = y.Y;
	p[6] = y.Z;
	// column 3
	p[8] = z.X;
	p[9] = z.Y;
	p[10] = z.Z;
	// column 4
	p[12] = o.X;
	p[13] = o.Y;
	p[14] = o.Z;

	return rv;
}

static IMeshSceneNode *
makeGenericMeshNode (ISceneManager *smgr, IMesh *mesh)
{
	IMeshSceneNode *node = smgr->addMeshSceneNode(mesh);
	
	node->setMaterialType(EMT_SOLID);
	node->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);
	node->setMaterialFlag(EMF_COLOR_MATERIAL, false);
	node->getMaterial(0).DiffuseColor = video::SColor(255, 10, 10, 10);
	node->getMaterial(0).AmbientColor = video::SColor(255, 255, 255, 255);
	node->setVisible(false);
	return node;
}

IMeshSceneNode *
MeshBuilder::buildTileNode (ISceneManager *smgr) const
{
	SMesh *mesh = new SMesh();
	mesh->addMeshBuffer(smgr->getMesh("models/tile.obj")->getMeshBuffer(0));
	return makeGenericMeshNode(smgr, mesh);
}

IMeshSceneNode *
MeshBuilder::buildLadderNode (
	ISceneManager *smgr,
	const vector3df &src,
	const vector3df &dst) const
{
	LineSurface surf = LineSurface(src, dst, vector3df(0, 1, 0));
	return buildExtendMeshNode(smgr, "ladder", surf);
}

IMeshSceneNode *
MeshBuilder::buildSlideNode (
	ISceneManager *smgr,
	const vector3df &src,
	const vector3df &dst) const
{
	SineSurface surf = SineSurface(src + vector3df(0, 0, 0), dst, vector3df(0, 1, 0));
	return buildExtendMeshNode(smgr, "slide", surf, 0.1);
}

IMeshSceneNode *
MeshBuilder::buildExtendMeshNode (
	ISceneManager *smgr,
	const char *name,
	const UVSurface &surf,
	const f32 zscale) const
{
	stringc s = stringc("./models/") + name;
	IMeshBuffer *midbuf = smgr->getMesh(s + "_mid.obj")->getMeshBuffer(0);
	IMeshBuffer *endbuf = smgr->getMesh(s + "_end.obj")->getMeshBuffer(0);
	SMesh *mesh = buildExtendedMesh(midbuf, endbuf, surf, zscale);

	return makeGenericMeshNode(smgr, mesh);
}

SMesh *
MeshBuilder::buildExtendedMesh (
	const IMeshBuffer *midbuf,
	const IMeshBuffer *endbuf,
	const UVSurface &surf,
	const f32 zscale) const
{
	const u32 SEGS = static_cast<u32>(surf.Length / zscale);
	const f32 PCT = (zscale * SEGS) / surf.Length;
	const f32 DELTA = PCT / SEGS;
	matrix4 mat;

	SMesh *smesh = new SMesh();
	CMeshBuffer<S3DVertex> *buf = new CMeshBuffer<S3DVertex>();
	
	f32 t = 0.0f;
	for (u32 i = 0; i < SEGS; t += DELTA, i++) {
		vector3df v1 = surf.getPosition(t, 0);
		vector3df v2 = surf.getPosition(t + DELTA, 0);
		f32 h = surf.Y.dotProduct(v2 - v1);
		vector3df zz = zscale * surf.Z + h * surf.Y;
		
		mat = getBasisMatrix(surf.X, surf.Y, zz, v1);
		appendToCMesh(buf, midbuf, mat);
	}

	mat = getBasisMatrix(surf.X, surf.Y, surf.Z, surf.getPosition(PCT, 0));
	appendToCMesh(buf, endbuf, mat);

	// include a 180 rotation around Y axis
	mat = getBasisMatrix(-surf.X, surf.Y, -surf.Z, surf.getPosition(0, 0));
	appendToCMesh(buf, endbuf, mat);

	buf->recalculateBoundingBox();
	smesh->addMeshBuffer(buf);
	smesh->recalculateBoundingBox();

	return smesh;
}

vector3df
UVSurface::getNormal (f32 u, f32 v) const
{
	const f32 EPSILON = 0.001f;
	vector3df surf = getPosition(u, v) - getPosition(u + EPSILON, v);
		
	return X.crossProduct(surf).normalize();
}

S3DVertex
UVSurface::getVertex (f32 u, f32 v, SColor col) const
{
	vector2df tex = vector2df(u, v);
	return S3DVertex(getPosition(u, v), getNormal(u, v), col, tex);
}

UVSurface::UVSurface (
	const vector3df &src,
	const vector3df &dst,
	const vector3df &up)
{
	const vector3df v = dst - src;
	O = src;
	Y = vector3df(up).normalize();

	Height = Y.dotProduct(v);
	Z = (v - Height * Y).normalize();
	Length = Z.dotProduct(v);
	X = Y.crossProduct(Z);
}

vector3df
SineSurface::getPosition (f32 u, f32 v) const
{
	f32 interval = 1.0f / count;
	u32 steps = (u32) (u / interval);
		
	f32 aratio = fmod(u, interval) / interval;
	f32 aphase = sin((aratio - 0.5) * M_PI) * 0.5 + 0.5;
		
	f32 hh = Height * (steps * interval + aphase * interval);
	f32 uu = Length * u;
	return O + uu * Z + v * X + hh * Y;
}

SineSurface::SineSurface (
	const vector3df &src,
	const vector3df &dst,
	const vector3df &up,
	const u32 n) :
	UVSurface (src, dst, up)
{
	count = n;
}
	
vector3df
LineSurface::getPosition (f32 u, f32 v) const
{
	return O + (Length * u) * Z + v * X;
}

LineSurface::LineSurface (
	const vector3df &src,
	const vector3df &dst,
	const vector3df &up) :
	UVSurface (src, dst, up)
{
	Z = (dst - src).normalize();
	Y = Z.crossProduct(X);
	Length = (dst - src).getLength();
}
	
u32 count;
