#pragma once

#include "Max.h"
#include <modstack.h>
#include "TextureMapIndexConstants.h"
#include "stdmat.h"
#include "3dsmaxport.h"
#include "IFrameTagManager.h"

#include <iskin.h>
#include <iskin.h>

#include "CS/BIPEXP.H"

#include <vector>
#include <string>
#include <map>
#include <filesystem>

#include "fastlz.h"
#include "bqmdlinfo.h"

#include "resource.h"

void GUI_GetNumbers(const char* str, const char* format, ...);

#ifndef IPOS_CONTROL_CLASS_ID
#define IPOS_CONTROL_CLASS_ID		Class_ID(0x118f7e02,0xffee238a)
#endif

struct vec3
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	float distance(const vec3& other)
	{
		float xx = other.x - x;
		float yy = other.y - y;
		float zz = other.z - z;

		return sqrt((xx * xx) + (yy * yy) + (zz * zz));
	}
};

struct aabb
{
	aabb()
	{
		m_min.x = FLT_MAX;
		m_min.y = FLT_MAX;
		m_min.z = FLT_MAX;
		
		m_max.x = FLT_MIN;
		m_max.y = FLT_MIN;
		m_max.z = FLT_MIN;
	}

	void add(const vec3& v)
	{
		if (v.x < m_min.x)m_min.x = v.x;
		if (v.y < m_min.y)m_min.y = v.y;
		if (v.z < m_min.z)m_min.z = v.z;

		if (v.x > m_max.x)m_max.x = v.x;
		if (v.y > m_max.y)m_max.y = v.y;
		if (v.z > m_max.z)m_max.z = v.z;
	}

	void add(const aabb& v)
	{
		add(v.m_min);
		add(v.m_max);
	}

	float radius()
	{
		return m_min.distance(m_max) * 0.5f;
	}

	vec3 center() const
	{
		vec3 v;
		v.x = m_min.x + m_max.x;
		v.y = m_min.y + m_max.y;
		v.z = m_min.z + m_max.z;
		v.x *= 0.5f;
		v.y *= 0.5f;
		v.z *= 0.5f;
		return v;
	}

	vec3 m_min;
	vec3 m_max;
};

#define TRI_AABB_MAXTRIS 10
struct tri_aabb
{
	tri_aabb()
	{
		for (int i = 0; i < TRI_AABB_MAXTRIS; ++i)
		{
			m_tris[i] = 0;
		}
	}

	aabb m_aabb;
	vec3 m_center;
	float m_distance = 0.f;
	int m_triNum = 0;

	uint32_t m_aabb_a = 0;
	uint32_t m_aabb_b = 0;

	uint32_t m_tris[TRI_AABB_MAXTRIS];

	enum
	{
		flag_added = 0x1,
		flag_distanceCalculated = 0x2,

		// это главный аабб, в него будут добавляться другие аабб
		flag_main = 0x4,
		
		flag_node = 0x8,
		flag_leafInTree = 0x10,
		flag_hasParent = 0x20,
	};
	uint32_t m_flags = 0;
	void clear_flags() { m_flags = 0; }
	void remove_flag(uint32_t flag) { m_flags &= ~flag; }
};

class FileBuffer
{
	uint8_t* m_data = 0;
	uint32_t m_allocated = 0;
	uint32_t m_add = 1024 * 1024;
	uint32_t m_size = 0;

	void _reallocate(uint32_t newSz)
	{
		uint32_t newAllocated = newSz + m_add;

		uint8_t* newData = (uint8_t*)malloc(newAllocated);

		if (m_data && newData)
		{
			memcpy(newData, m_data, m_allocated);
			free(m_data);
		}

		if (newData)
		{
			m_data = newData;
			m_allocated = newAllocated;
		}
	}

public:
	FileBuffer() 
	{
		Reserve(4);
	}

	~FileBuffer() 
	{
		Clear();
	}

	void Clear()
	{
		if (m_data)
		{
			free(m_data);
			m_data = 0;
			m_allocated = 0;
		}
		m_size = 0;
	}

	void Reserve(uint32_t sz)
	{
		if (sz > m_allocated)
			_reallocate(sz);
	}

	uint32_t Size() { return m_size; }
	uint8_t* Data() { return m_data; }

	void Add(void* data, uint32_t sz)
	{
		if (data && sz)
		{
			uint32_t newSz = m_size + sz;
			if (newSz >= m_allocated)
				_reallocate(newSz);

			memcpy(&m_data[m_size], data, sz);

			m_size = newSz;
		}
	}

};

struct _Material
{
	bqMDLChunkHeaderMaterial m_header;
};


class SubMesh
{
public:
	SubMesh()
	{
	}
	~SubMesh()
	{
		if (m_vertices) free(m_vertices);
		if (m_indices) free(m_indices);
	}
	bool Allocate(bqMDLChunkHeaderMesh header)
	{
		bool r = false;
		if (!m_vertices)
		{
			m_vertices = (uint8_t*)calloc(1, header.m_vertBufSz);

			if (m_vertices)
			{
				m_indices = (uint8_t*)calloc(1, header.m_indBufSz);
				r = true;
			}
			m_chunkHeaderMesh = header;
		}
		return r;
	}
	uint8_t* m_vertices = 0;
	uint8_t* m_indices = 0;
	uint32_t m_stride = 0;
	bqMDLChunkHeaderMesh m_chunkHeaderMesh;

	aabb m_aabb;
	void AabbAdd(const Point3& p)
	{
		/*if (p.x < m_chunkHeaderMesh.m_aabbMin[0]) m_chunkHeaderMesh.m_aabbMin[0] = p.x;
		if (p.y < m_chunkHeaderMesh.m_aabbMin[1]) m_chunkHeaderMesh.m_aabbMin[1] = p.y;
		if (p.z < m_chunkHeaderMesh.m_aabbMin[2]) m_chunkHeaderMesh.m_aabbMin[2] = p.z;

		if (p.x > m_chunkHeaderMesh.m_aabbMax[0]) m_chunkHeaderMesh.m_aabbMax[0] = p.x;
		if (p.y > m_chunkHeaderMesh.m_aabbMax[1]) m_chunkHeaderMesh.m_aabbMax[1] = p.y;
		if (p.z > m_chunkHeaderMesh.m_aabbMax[2]) m_chunkHeaderMesh.m_aabbMax[2] = p.z;*/
		vec3 v;
		v.x = p.x;
		v.y = p.y;
		v.z = p.z;
		m_aabb.add(v);

		m_chunkHeaderMesh.m_aabbMax[0] = m_aabb.m_max.x;
		m_chunkHeaderMesh.m_aabbMax[1] = m_aabb.m_max.y;
		m_chunkHeaderMesh.m_aabbMax[2] = m_aabb.m_max.z;
		m_chunkHeaderMesh.m_aabbMin[0] = m_aabb.m_min.x;
		m_chunkHeaderMesh.m_aabbMin[1] = m_aabb.m_min.y;
		m_chunkHeaderMesh.m_aabbMin[2] = m_aabb.m_min.z;
		m_chunkHeaderMesh.m_radius = m_aabb.radius();
	}


	//_Material* m_material = 0;
	uint32_t m_materialIndex = 0;

	//Mesh* m_maxMesh = 0;
	uint32_t m_faceNum = 0;
};

class _Mesh
{
public:
	_Mesh()
	{
	}

	~_Mesh()
	{
		for (size_t i = 0, sz = m_subMesh.size(); i < sz; ++i)
		{
			delete m_subMesh[i];
		}
	}
	std::vector<SubMesh*> m_subMesh;


	SubMesh* GetSubMesh(_Material* material/*, Mesh* maxMesh*/)
	{
		SubMesh* sd = 0;
		if (material)
		{
			for (size_t i = 0, sz = m_subMesh.size(); i < sz; ++i)
			{
				if (m_subMesh[i]->m_materialIndex == material->m_header.m_nameIndex)
				{
					sd = m_subMesh[i];
					break;
				}
			}

			if (!sd)
			{
				SubMesh* newSD = new SubMesh;
				newSD->m_materialIndex = material->m_header.m_nameIndex;
				//newSD->m_maxMesh = maxMesh;
				newSD->m_faceNum = 0;
				m_subMesh.push_back(newSD);

				sd = newSD;
			}
		}

		return sd;
	}

	bool m_multiMat = false;
	
	// материалы которые закреплены за m_node.
	// индексы имён.
	std::vector<uint32_t> m_materials;
	uint32_t GetMaterial(uint32_t id)
	{
		uint32_t m = 0;
		if (m_materials.size())
		{
			if (id < m_materials.size())
			{
				return m_materials[id];
			}
		}
		return m;
	}

	INode* m_node = 0;
	TriObject* m_triObj = 0;
	ISkin* m_skin = 0;
	ISkinContextData* m_skinData = 0;
};

#include "exporter.h"
#include "mod.h"

