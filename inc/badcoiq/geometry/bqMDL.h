﻿/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#ifndef __BQ_MDL_H__
#define __BQ_MDL_H__

#include "badcoiq/geometry/bqAABB.h"
#include "badcoiq/geometry/bqMesh.h"
#include "badcoiq/geometry/bqSkeleton.h"
#include "badcoiq/geometry/bqTriangle.h"
#include "badcoiq/archive/bqArchive.h"
#include "badcoiq/common/bqFileBuffer.h"
#include "badcoiq/gs/bqGS.h"

#include "bqmdlinfo.h"

class bqMDLBVHNode
{
public:
	bqMDLBVHNode();
	~bqMDLBVHNode();

	bqMDLBVHAABB m_mdl_aabb;
	uint32_t m_triNum = 0;
	uint32_t* m_tris = 0;
};

class bqMDLCollision
{
public:
	bqMDLCollision();
	~bqMDLCollision();
	BQ_PLACEMENT_ALLOCATOR(bqMDLCollision);

	bqAabb m_aabb;
	bqReal m_radius = 0.f;

	//bqTriangle* m_triangles = 0;
	//uint32_t m_triNum = 0;

	bqVec3f* m_vBuf = 0;
	uint32_t* m_iBuf = 0;
	uint32_t m_vertNum = 0;
	uint32_t m_indNum = 0;

	bqMDLBVHNode* m_bvh = 0;
	uint32_t m_bvhNodeNum = 0;

	/// origin - позиция
	/// указывается позиция только другого объекта, относительно этого
	/// 
	/// Например 
	/// CollisionSphereSphere( obj->m_radius, obj->m_pos - mdlColObj->m_pos )
	/// 
	/// точка пересечения, нормаль и прочая информация помещается в переменные,
	/// указанных ниже следующих методов
	///
	bool CollisionSphereSphere(bqReal radius, const bqVec3& origin);
	bool CollisionSphereBox(bqReal radius, const bqVec3& origin);
	bool CollisionSphereBVH(bqReal radius, const bqVec3& origin);
	bool CollisionSphereTriangle(bqReal radius, const bqVec3& origin);
	bool CollisionTriangleTriangle(bqTriangle* t, const bqVec3& origin);
	//bool CollisionAabbTriangle(const bqAabb& a, const bqVec3& origin);

	/// Позиция луча должна учитывать позицию объекта.
	/// Метод проверяет треугольники как есть, то есть они
	/// как бы относительно центра мира.
	/// 
	/// Одно пересечение, самое близкое к bqRay::m_origin
	bool CollisionRayTriangle(const bqRay&);

	bqVec3 m_outIntersection;
	bqVec3 m_outNormal;
	bqReal m_outLen = 0.0;
	bqTriangle m_outTriangle;
};

class bqMDLHitbox
{
public:
	bqMDLHitbox();
	~bqMDLHitbox();

	bqVec3f* m_vBuf = 0;
	uint16_t* m_iBuf = 0;
	uint16_t m_vNum = 0;
	bqAabb m_aabb;
	
	uint16_t m_jointIndex = 0;
	bqJoint* m_joint = 0;
};

// 
class bqMDL
{
	struct _mtl
	{
		bqMDLChunkHeaderMaterial m_header;
		bqMaterial m_material;

		bqTexture* m_diffuseMap = 0;
	};

	struct _mesh
	{
		bqMesh* m_mesh = 0;
		bqGPUMesh* m_GPUmesh = 0;
		bqMDLChunkHeaderMesh m_chunkHeaderMesh;

		_mtl m_mtl;
	};

	bqArray<_mesh> m_meshes;

	bqArray<bqStringA> m_strings;
	uint8_t* _decompress(const bqMDLFileHeader&, 
		bqFileBuffer& , 
		uint32_t*, bqCompressorType);
	uint8_t* m_compressedData = 0;

	bqSkeleton* m_skeleton = 0;
	bqArray<bqSkeletonAnimation*> m_loadedAnimations;

	// ! их может быть множество !
	// потом переделаю
	bqMDLCollision* m_collision = 0;

	bqAabb m_aabb;
	bqReal m_radius = 0.f;

	bqArray<bqMDLHitbox*> m_hitboxes;
public:
	bqMDL();
	~bqMDL();
	BQ_PLACEMENT_ALLOCATOR(bqMDL);

	bqMDLCollision* GetCollision() { return m_collision; }

	bool Load(const char* mdlFile, const char* textureDir, bqGS*, bool free_bqMesh);
	void Unload();
	
	void FreebqMesh();
	size_t GetMeshNum() { return m_meshes.m_size; }
	bqGPUMesh* GetGPUMesh(size_t i) { return m_meshes.m_data[i].m_GPUmesh; }
	bqSkeleton* GetSkeleton() { return m_skeleton; }
	bqShaderType GetShaderType(size_t i) { return m_meshes.m_data[i].m_mtl.m_material.m_shaderType; }
	bqMaterial* GetMaterial(size_t i) { return &m_meshes.m_data[i].m_mtl.m_material; }
	
	uint32_t GetAniNum() { return (uint32_t)m_loadedAnimations.m_size; }
	bqSkeletonAnimation* GetAnimation(uint32_t i) { return m_loadedAnimations.m_data[i]; }

	bqAabb& GetAabb() { return m_aabb; }
	bqReal GetBoundingRadius() { return m_radius; }
	
	size_t GetHitboxNum() { return m_hitboxes.m_size; }
	bqMDLHitbox* GetHitbox(size_t i) { return m_hitboxes.m_data[i]; }
};


#endif

