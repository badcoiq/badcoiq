#pragma once

class PluginExporter;

class ClassDescExporter : public ClassDesc
{
public:
	ClassDescExporter() {}
	virtual ~ClassDescExporter() {}

	virtual int				IsPublic() { return TRUE; };
	virtual void* Create(BOOL loading = FALSE);

	virtual const MCHAR* ClassName()
	{
		return L"Badcoiq plugin class";
	}

	virtual const MCHAR* NonLocalizedClassName()
	{
		return L"Badcoiq plugin class";
	}

	virtual SClass_ID		SuperClassID()
	{
		return SCENE_EXPORT_CLASS_ID;
	}

	virtual Class_ID		ClassID()
	{
		return Class_ID(0xb3d0, 0);
	}

	virtual const MCHAR* Category()
	{
		return L"Scene Export";
	}
};


class PluginExporter : public SceneExport
{
	uint32_t m_stringIndex = 0;
	std::vector<std::string> m_strings;
	std::map<std::string, uint32_t> m_stringMap;
	std::vector<_Mesh*> m_meshes;
	FileBuffer m_fileBuffer;

public:
	PluginExporter();
	virtual ~PluginExporter();
	int	ExtCount();
	virtual const MCHAR* Ext(int n);
	virtual const MCHAR* LongDesc();
	virtual const MCHAR* ShortDesc();
	virtual const MCHAR* AuthorName();
	virtual const MCHAR* CopyrightMessage();
	virtual const MCHAR* OtherMessage1();
	virtual const MCHAR* OtherMessage2();
	virtual unsigned int	Version();
	virtual void ShowAbout(HWND hWnd);

	std::vector<_Material> m_materials;
	_Material* GetMaterial(uint32_t id);
	uint32_t AddString(const char* str);
	uint32_t AddString(const wchar_t* str);
	std::string& GetString(uint32_t i);
	_Mesh* CreateMesh();

	// будет установлен true когда добавится хоть 1
	// animation data в структуру SkeletonBone.
	// добавление зависит от того на сколько установлена time line.
	//  (количество фреймов на полоске внизу программы)
	// в 3Ds Max. Возможно там можно установить 1 фрейм или даже 0.
	// 1й обычно это bind поза.
	// хотя для файла который хранит чисто анимации
	// bind поза не нужна. Получается анимация есть даже если
	// она состоит из 1го фрейма.
	// В любом случае m_hasBoneAnimationData, для порядка.
	bool m_hasBoneAnimationData = false;

	Point3 m_rotation = Point3(PI * 0.5f, 0.f, 0.f);

	void Save(const MCHAR* name);

	struct SkeletonBoneTransformation
	{
		float m_position[3];
		float m_scale[3];
		float m_rotation[4];

		uint8_t* Data() { return (uint8_t*)this; }

		void GetPosition(float* in)
		{
			in[0] = m_position[0];
			in[1] = m_position[1];
			in[2] = m_position[2];
		}
		void GetScale(float* in)
		{
			in[0] = m_scale[0];
			in[1] = m_scale[1];
			in[2] = m_scale[2];
		}
		void GetRotation(float* in)
		{
			in[0] = m_rotation[0];
			in[1] = m_rotation[1];
			in[2] = m_rotation[2];
			in[3] = m_rotation[3];
		}
		void SetPosition(float* in)
		{
			m_position[0] = in[0];
			m_position[1] = in[1];
			m_position[2] = in[2];
		}
		void SetScale(float* in)
		{
			m_scale[0] = in[0];
			m_scale[1] = in[1];
			m_scale[2] = in[2];
		}
		void SetRotation(float* in)
		{
			m_rotation[0] = in[0];
			m_rotation[1] = in[1];
			m_rotation[2] = in[2];
			m_rotation[3] = in[3];
		}
	};

	struct SkeletonBone
	{
		// индексы относительно всей иерархии.
		// в skin модификатор может быть добавлены
		// не все кости. Индексы костей в вершине
		// ВЕРОЯТНО указаны относительно добавленных 
		// костей, а не относительно всей иерархии.
		// В bqmdl будут индексы относительно всей иерархии.
		int32_t m_index = 0;
		int32_t m_parentIndex = -1;

		uint32_t m_nameIndex = 0;
		INode* m_node = 0;

		SkeletonBoneTransformation m_initialTransformation;
		void GetInitialPosition(float* in)
		{
			m_initialTransformation.GetPosition(in);
		}
		void GetInitialScale(float* in)
		{
			m_initialTransformation.GetScale(in);
		}
		void GetInitialRotation(float* in)
		{
			m_initialTransformation.GetRotation(in);
		}
		void SetInitialPosition(float* in)
		{
			m_initialTransformation.SetPosition(in);
		}
		void SetInitialScale(float* in)
		{
			m_initialTransformation.SetScale(in);
		}
		void SetInitialRotation(float* in)
		{
			m_initialTransformation.SetRotation(in);
		}


		std::vector<SkeletonBoneTransformation> m_animationData;
	};
	std::map<std::string, SkeletonBone> m_skeleton;
	int m_bonesNum = 0;


	void AddBone(INode* node, Object* obj);
	void BuildSkeleton();
	SkeletonBone GetBone(const char* name);
	SkeletonBone GetBone(INode* node);
	SkeletonBone GetBone(int index);
	std::string GetAString(const wchar_t* str);
	
	class MyITreeEnumProc : public ITreeEnumProc
	{
		PluginExporter* m_plugin = 0;
		Interface* m_ip = 0;
	public:
		MyITreeEnumProc(PluginExporter* p, Interface* ip) : m_plugin(p),
			m_ip(ip) {}
		virtual ~MyITreeEnumProc() {}
		virtual int callback(INode* node)
		{
			// if (exportSelected && node->Selected() == FALSE)
			//     return TREE_CONTINUE;


			Object* obj = node->EvalWorldState(m_plugin->m_timeValue).obj;

			auto superClassID = obj->SuperClassID();
			auto classID = obj->ClassID();

			if (classID == Class_ID(DUMMY_CLASS_ID, 0))
			{
				return TREE_CONTINUE;
			}

			/*std::string nodeName(m_plugin->GetAString(node->GetName()).c_str());
			printf("Node: %s\n", nodeName.c_str());*/
			Control* c = node->GetTMController();

			if ((obj->ClassID() == BONE_OBJ_CLASSID) ||
				(c->ClassID() == BIPDRIVEN_CONTROL_CLASS_ID) ||
				(c->ClassID() == BIPBODY_CONTROL_CLASS_ID) ||
				(c->ClassID() == FOOTPRINT_CLASS_ID))
			{
				m_plugin->AddBone(node, obj);
				return TREE_CONTINUE;
			}

			//if (obj->ClassID() == BIPED_CLASS_ID)
			//{
			//    m_plugin->AddBone(node, obj);
			//    //    Append(node, obj);
			//    return TREE_CONTINUE;
			//}

			/*if (obj->ClassID() != Class_ID(POLYOBJ_CLASS_ID, 0))
			{
				return TREE_CONTINUE;
			}*/

			if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
			{
				if (!m_plugin->m_GUI_checkOnlySkeleton)
				{
					m_plugin->AddMesh(node, obj->ConvertToType(m_plugin->m_timeValue, Class_ID(TRIOBJ_CLASS_ID, 0)));
				}
				return TREE_CONTINUE;
			}
			//printf("superClassID %#010x\n", superClassID);
			//printf("classID %#010x %#010x\n", classID.PartA(), classID.PartB());


			return TREE_CONTINUE;
		}

	};

	TimeValue m_timeValue = 0;
	int m_framesNum = 0;
	int m_tickPerFrame = 0;
	INode* m_sceneRootNode = 0;

	void MtlGetBitmap(std::string& str, Texmap* tMap);
	void AddMtl(Mtl* mtl, _Mesh* _mesh);

	// надо добавить определённую информацию чтобы потом
	// спокойно создать sub meshes.
	// информация не касающаяся subMesh
	void AddMesh(INode* node, Object* obj);
	Point3 GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);
	void BuildMeshes();

	ExpInterface* m_ei = 0;
	Interface* m_ip = 0;

	virtual int DoExport(const MCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts = FALSE, DWORD options = 0);
	virtual BOOL SupportsOptions(int ext, DWORD options);

	int m_GUI_checkExportAnimation = 1;
	int m_GUI_checkOnlySkeleton = 0;
	float m_GUI_scale = 1.f;
};