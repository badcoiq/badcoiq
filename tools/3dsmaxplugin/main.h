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

	void AabbAdd(const Point3& p)
	{
		if (p.x < m_chunkHeaderMesh.m_aabbMin[0]) m_chunkHeaderMesh.m_aabbMin[0] = p.x;
		if (p.y < m_chunkHeaderMesh.m_aabbMin[1]) m_chunkHeaderMesh.m_aabbMin[1] = p.y;
		if (p.z < m_chunkHeaderMesh.m_aabbMin[2]) m_chunkHeaderMesh.m_aabbMin[2] = p.z;

		if (p.x > m_chunkHeaderMesh.m_aabbMax[0]) m_chunkHeaderMesh.m_aabbMax[0] = p.x;
		if (p.y > m_chunkHeaderMesh.m_aabbMax[1]) m_chunkHeaderMesh.m_aabbMax[1] = p.y;
		if (p.z > m_chunkHeaderMesh.m_aabbMax[2]) m_chunkHeaderMesh.m_aabbMax[2] = p.z;
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

