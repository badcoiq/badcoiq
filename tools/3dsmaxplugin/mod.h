#pragma once
#include "simpmod.h"

class PluginMod_collisionMesh;

class CreateMouseCallBack_PluginMod : public CreateMouseCallBack
{
public:
	CreateMouseCallBack_PluginMod() {}
	virtual ~CreateMouseCallBack_PluginMod() {}

	virtual int proc(
		ViewExp* vpt,
		int msg,
		int point,
		int flags,
		IPoint2 m,
		Matrix3& mat
	) override;
};

class ClassDescMod_collisionMesh : public ClassDesc2
{
public:
	ClassDescMod_collisionMesh() {}
	virtual ~ClassDescMod_collisionMesh() {}

	virtual int				IsPublic() { return TRUE; };
	virtual void* Create(BOOL loading = FALSE);

	virtual const MCHAR* ClassName()
	{
		return L"Badcoiq Modifier";
	}

	virtual const MCHAR* NonLocalizedClassName()
	{
		return L"Badcoiq Modifier";
	}

	virtual SClass_ID		SuperClassID()
	{
		return OSM_CLASS_ID;
	}

	virtual Class_ID		ClassID()
	{
		return Class_ID(0x69e43a5c, 0x6b6c0cc8);
	}

	virtual const MCHAR* Category()
	{
		return L"Badcoiq Engine";
	}
};

class Deformer_collisionMesh : public Deformer
{
public:
	Deformer_collisionMesh() {}
	virtual ~Deformer_collisionMesh() {}
};

class PluginMod_collisionMesh : public SimpleMod
{
	Deformer_collisionMesh m_deformer;
public:
	PluginMod_collisionMesh();
	virtual ~PluginMod_collisionMesh();
	virtual Deformer& GetDeformer(TimeValue t, ModContext& mc, Matrix3& mat, Matrix3& invmat) override;
};