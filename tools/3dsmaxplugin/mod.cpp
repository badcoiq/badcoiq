#include "main.h"

int CreateMouseCallBack_PluginMod::proc(
	ViewExp* vpt,
	int msg,
	int point,
	int flags,
	IPoint2 m,
	Matrix3& mat
)
{
	return CREATE_STOP;
}

void* ClassDescMod_collisionMesh::Create(BOOL loading)
{
	return new PluginMod_collisionMesh;
}


PluginMod_collisionMesh::PluginMod_collisionMesh()
{
}

PluginMod_collisionMesh::~PluginMod_collisionMesh()
{
}

Deformer& PluginMod_collisionMesh::GetDeformer(TimeValue t, 
	ModContext& mc, 
	Matrix3& mat, Matrix3& invmat)
{
	return m_deformer;
}

