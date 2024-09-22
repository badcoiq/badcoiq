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

#define xInfinity std::numeric_limits<float>::infinity()
#define xEpsilon std::numeric_limits<float>::epsilon()

void GUI_GetNumbers(const char* str, const char* format, ...);

#ifndef IPOS_CONTROL_CLASS_ID
#define IPOS_CONTROL_CLASS_ID		Class_ID(0x118f7e02,0xffee238a)
#endif

template<class T>
const T& xMax(const T& a, const T& b)
{
	return (a < b) ? b : a;
}
template<class T>
const T& xMin(const T& a, const T& b)
{
	return (b < a) ? b : a;
}

struct vec3
{
	vec3()
	{
	}
	vec3(float v)
		:
		x(v),y(v),z(v)
	{
	}
	vec3(float _x, float _y, float _z)
		:
		x(_x), y(_y), z(_z)
	{
	}

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
	float dot()const { return (x * x) + (y * y) + (z * z); }
	float dot(const vec3& o)const { return (x * o.x) + (y * o.y) + (z * o.z); }

	vec3 cross(const vec3& a)const {
		vec3 out;
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
		return out;
	}

	void normalize()
	{
		float len = std::sqrt(dot());
		if (len > 0)
			len = 1.0 / len;
		x *= len;
		y *= len;
		z *= len;
	}

	vec3 operator+(const vec3& v)const
	{
		vec3 r;
		r.x = x + v.x;
		r.y = y + v.y;
		r.z = z + v.z;
		return r;
	}
	vec3 operator-(const vec3& v)const
	{
		vec3 r;
		r.x = x - v.x;
		r.y = y - v.y;
		r.z = z - v.z;
		return r;
	}
	vec3 operator*(const vec3& v)const
	{
		vec3 r;
		r.x = x * v.x;
		r.y = y * v.y;
		r.z = z * v.z;
		return r;
	}
	vec3 operator/(const vec3& v)const
	{
		vec3 r;
		r.x = x / v.x;
		r.y = y / v.y;
		r.z = z / v.z;
		return r;
	}
};

class xRay
{
public:
	xRay() {}
	~xRay() {}

	vec3 m_origin;
	vec3 m_end;
	vec3 m_direction;
	vec3 m_invDir;
	void Update()
	{
		m_direction.x = m_end.x - m_origin.x;
		m_direction.y = m_end.y - m_origin.y;
		m_direction.z = m_end.z - m_origin.z;
		m_direction.normalize();

		m_invDir.x = 1.f / m_direction.x;
		m_invDir.y = 1.f / m_direction.y;
		m_invDir.z = 1.f / m_direction.z;
	}
	
	void Set(const vec3& origin, const vec3& end)
	{
		m_origin = origin;
		m_end = end;
		Update();
	}

};

class xTriangle
{
public:
	xTriangle() {}
	xTriangle(const vec3 _v1, const vec3& _v2, const vec3& _v3)
	{
		Set(_v1, _v2, _v3);
	}
	
	vec3 v1;
	vec3 v2;
	vec3 v3;
	vec3 e1;
	vec3 e2;

	void Set(const vec3 _v1, const vec3& _v2, const vec3& _v3)
	{
		v1 = _v1;
		v2 = _v2;
		v3 = _v3;
		Update();
	}

	void Update()
	{
		e1 = vec3(v2.x - v1.x,
			v2.y - v1.y,
			v2.z - v1.z);
		e2 = vec3(v3.x - v1.x,
			v3.y - v1.y,
			v3.z - v1.z);
		//	e1.cross(e2, faceNormal);
	}

	void Center(vec3& out)
	{
		out = (v1 + v2 + v3) * 0.3333333;
	}

	bool RayTest_MT(const xRay& ray, bool withBackFace, float& T, float& U, float& V, float& W)
	{
		vec3  pvec = ray.m_direction.cross(e2);
		float det = e1.dot(pvec);

		if (withBackFace)
		{
			if (std::fabs(det) < xEpsilon)
				return false;
		}
		else
		{
			if (det < xEpsilon && det > -xEpsilon)
				return false;
		}

		vec3 tvec(
			ray.m_origin.x - v1.x,
			ray.m_origin.y - v1.y,
			ray.m_origin.z - v1.z);

		float inv_det = 1.f / det;
		U = tvec.dot(pvec) * inv_det;

		if (U < 0.f || U > 1.f)
			return false;

		vec3  qvec = tvec.cross(e1);

		V = ray.m_direction.dot(qvec) * inv_det;

		if (V < 0.f || U + V > 1.f)
			return false;

		T = e2.dot(qvec) * inv_det;

		if (T < xEpsilon) return false;

		W = 1.f - U - V;
		return true;
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

	aabb(const aabb& v)
	{
		add(v.m_min);
		add(v.m_max);
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

	bool isPointInside(const vec3& p)
	{
		bool r = false;
		if ((p.x >= m_min.x) && (p.x <= m_max.x))
		{
			if ((p.y >= m_min.y) && (p.y <= m_max.y))
			{
				if ((p.z >= m_min.z) && (p.z <= m_max.z))
				{
					r = true;
				}
			}
		}
		return r;
	}

	bool RayTest(const xRay& r) const
	{

		float t1 = (m_min.x - r.m_origin.x) * r.m_invDir.x;
		float t2 = (m_max.x - r.m_origin.x) * r.m_invDir.x;
		float t3 = (m_min.y - r.m_origin.y) * r.m_invDir.y;
		float t4 = (m_max.y - r.m_origin.y) * r.m_invDir.y;
		float t5 = (m_min.z - r.m_origin.z) * r.m_invDir.z;
		float t6 = (m_max.z - r.m_origin.z) * r.m_invDir.z;

		float tmin = xMax(xMax(xMin(t1, t2), xMin(t3, t4)), xMin(t5, t6));
		float tmax = xMin(xMin(xMax(t1, t2), xMax(t3, t4)), xMax(t5, t6));

		if (tmax < 0 || tmin > tmax) return false;

		return true;
	}
	bool RayTestTriangle(const vec3& tv1, const vec3& tv2, const vec3& tv3) const
	{
		auto& p1 = m_min;
		auto& p2 = m_max;

		vec3 v1 = p1;
		vec3 v2 = p2;

		vec3 v3(p1.x, p1.y, p2.z);
		vec3 v4(p2.x, p1.y, p1.z);
		vec3 v5(p1.x, p2.y, p1.z);
		vec3 v6(p1.x, p2.y, p2.z);
		vec3 v7(p2.x, p1.y, p2.z);
		vec3 v8(p2.x, p2.y, p1.z);

		xRay r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12;
		r1.Set(v1, v4);
		r2.Set(v5, v8);
		r3.Set(v1, v5);
		r4.Set(v4, v8);
		r5.Set(v3, v7);
		r6.Set(v6, v2);
		r7.Set(v3, v6);
		r8.Set(v7, v2);
		r9.Set(v2, v8);
		r10.Set(v4, v7);
		r11.Set(v5, v6);
		r12.Set(v1, v3);

		xTriangle triangle(v1,v2,v3);
		float t, u, v, w;
		t, u = v = w = 0.f;

		bool r = triangle.RayTest_MT(r1, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r2, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r3, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r4, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r5, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r6, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r7, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r8, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r9, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r10, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r11, true, t, u, v, w);
		if (!r) r = triangle.RayTest_MT(r12, true, t, u, v, w);
		return r;
	}
	bool isHasTriangle(const vec3& v1, const vec3& v2, const vec3& v3)
	{
		bool r = isPointInside(v1);
		if (!r) r = isPointInside(v2);
		if (!r) r = isPointInside(v3);
		//if (!r) r = RayTestTriangle(v1,v2,v3);

		return r;
	}

	vec3 m_min;
	vec3 m_max;
};

#define TRI_AABB_MAXTRIS 20
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
	vec3 m_firstTriangleCenter;

	float m_distance = 0.f;
	int32_t m_triNum = 0;

	uint32_t m_indexInAabbs = 0;

	uint32_t m_aabb_a = 0xFFFFFFFF;
	uint32_t m_aabb_b = 0xFFFFFFFF;

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

	int m_level = 0;
};
struct tri_aabb2
{
	tri_aabb2()
	{
	}
	~tri_aabb2()
	{
		if (m_tris)
			delete[]m_tris;
	}

	aabb m_aabb;

	int32_t m_triNum = 0;
	uint32_t * m_tris = 0;

	uint32_t m_aabb_a = 0xFFFFFFFF;
	uint32_t m_aabb_b = 0xFFFFFFFF;

	//uint32_t m_tris[TRI_AABB_MAXTRIS];

	enum
	{
		flag_added = 0x1,
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

		m_chunkHeaderMesh.m_aabb.m_aabbMax[0] = m_aabb.m_max.x;
		m_chunkHeaderMesh.m_aabb.m_aabbMax[1] = m_aabb.m_max.y;
		m_chunkHeaderMesh.m_aabb.m_aabbMax[2] = m_aabb.m_max.z;
		m_chunkHeaderMesh.m_aabb.m_aabbMin[0] = m_aabb.m_min.x;
		m_chunkHeaderMesh.m_aabb.m_aabbMin[1] = m_aabb.m_min.y;
		m_chunkHeaderMesh.m_aabb.m_aabbMin[2] = m_aabb.m_min.z;
		m_chunkHeaderMesh.m_aabb.m_radius = m_aabb.radius();
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

