#include "Max.h"
#include "bqmdlinfo.h"

#include <vector>
#include <string>
#include <map>

class _Mesh
{
public:
    _Mesh(const bqMDLChunkHeaderMesh& header)
    {
        m_chunkHeaderMesh = header;

        /*int indexSize = 2;
        if (header.m_indexType == 1)
            indexSize = 4;*/

        m_vertices = (bqMDLMeshVertex*)malloc(header.m_vertBufSz);
        if (m_vertices)
        {
            for (uint32_t i = 0; i < header.m_vertNum; ++i)
            {
                auto v = &m_vertices[i];
                memset(v, 0, sizeof(bqMDLMeshVertex));
            }

            m_indices = (uint8_t*)malloc(header.m_indBufSz);
            if (m_indices)
            {
                for (uint32_t i = 0; i < header.m_indBufSz; ++i)
                {
                    m_indices[i] = 0;
                }
            }
        }
    }

    ~_Mesh()
    {
        if (m_vertices) free(m_vertices);
        if (m_indices) free(m_indices);
    }
    bqMDLChunkHeaderMesh m_chunkHeaderMesh;

    bqMDLMeshVertex* m_vertices = 0;
    uint8_t* m_indices = 0;
};

class bqMDL
{
    uint32_t m_stringIndex = 0;
public:
    bqMDL() {}
    ~bqMDL() 
    {
        for (size_t i = 0, sz = m_meshes.size(); i < sz; ++i)
        {
            delete m_meshes[i];
        }
    }

    bqMDLFileHeader m_fileHeader;
    bqMDLChunkHeader m_chunkHeader;
    bqMDLChunkHeaderMesh m_chunkHeaderMesh;
    bqMDLTextTableHeader m_textTableHeader;

    std::vector<std::string> m_strings;
    std::map<std::string, uint32_t> m_stringMap;
    std::vector<_Mesh*> m_meshes;

    uint32_t AddString(const char* str)
    {
        uint32_t index = 0;

        auto iterator = m_stringMap.find(str);
        if (iterator == m_stringMap.end())
        {
            index = m_stringIndex;
            m_stringMap[str] = index;
            m_strings.push_back(str);
            ++m_stringIndex;
        }
        else
        {
            index = iterator->second;
        }

        return index;
    }

    uint32_t AddString(const wchar_t* str)
    {
        char cstr[0x5fff];
        WideCharToMultiByte(CP_UTF8, 0, str, -1, cstr, 0x5fff, 0, 0);
        
        return AddString(cstr);
    }

    _Mesh* CreateMesh(const bqMDLChunkHeaderMesh& header)
    {
        _Mesh* m = new _Mesh(header);
        m_meshes.push_back(m);
        return m;
    }

    void Save(const MCHAR* name)
    {
        FILE* f = 0;
        char cname[0xffff];
        WideCharToMultiByte(CP_UTF8, 0, name, -1, cname, 0xffff, 0, 0);
        fopen_s(&f, cname, "wb");
        if (f)
        {
            auto meshChunkNum = m_meshes.size();

            fwrite(&m_fileHeader.m_bmld, 1, sizeof(m_fileHeader.m_bmld), f);
            fwrite(&m_fileHeader.m_version, 1, sizeof(m_fileHeader.m_version), f);

            m_fileHeader.m_chunkNum = meshChunkNum;
            fwrite(&m_fileHeader.m_chunkNum, 1, sizeof(m_fileHeader.m_chunkNum), f);
            
            uint32_t reserved[4] = { 0,0,0,0 };
            fwrite(&reserved, 1, 4 * sizeof(uint32_t), f);

            // ================================== chunks
            for (uint32_t i = 0; i < meshChunkNum; ++i)
            {
                m_chunkHeader.m_chunkType = 1;
                fwrite(&m_chunkHeader.m_chunkType, 1, sizeof(m_chunkHeader.m_chunkType), f);

                uint32_t reserved[2] = { 0,0 };
                fwrite(&reserved, 1, 2 * sizeof(uint32_t), f);

                // ==================================bqMDLChunkHeaderMesh
                auto mesh = m_meshes[i];
                
                fwrite(&mesh->m_chunkHeaderMesh.m_name, 1, sizeof(mesh->m_chunkHeaderMesh.m_name), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_indexType, 1, sizeof(mesh->m_chunkHeaderMesh.m_indexType), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_vertexType, 1, sizeof(mesh->m_chunkHeaderMesh.m_vertexType), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_material, 1, sizeof(mesh->m_chunkHeaderMesh.m_material), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_vertNum, 1, sizeof(mesh->m_chunkHeaderMesh.m_vertNum), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_indNum, 1, sizeof(mesh->m_chunkHeaderMesh.m_indNum), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_vertBufSz, 1, sizeof(mesh->m_chunkHeaderMesh.m_vertBufSz), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_indBufSz, 1, sizeof(mesh->m_chunkHeaderMesh.m_indBufSz), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_reserved1, 1, sizeof(mesh->m_chunkHeaderMesh.m_reserved1), f);
                fwrite(&mesh->m_chunkHeaderMesh.m_reserved2, 1, sizeof(mesh->m_chunkHeaderMesh.m_reserved2), f);
             
                fwrite(mesh->m_vertices, 1, mesh->m_chunkHeaderMesh.m_vertBufSz, f);
                fwrite(mesh->m_indices, 1, mesh->m_chunkHeaderMesh.m_indBufSz, f);
            }

            // ==================================bqMDLTextTableHeader
            m_textTableHeader.m_strNum = m_strings.size();
            fwrite(&m_textTableHeader.m_strNum, 1, sizeof(m_textTableHeader.m_strNum), f);
            fwrite(&m_textTableHeader.m_reserved1, 1, sizeof(m_textTableHeader.m_reserved1), f);
            for (uint32_t i = 0; i < m_textTableHeader.m_strNum; ++i)
            {
                for (auto str : m_strings)
                {
                    fprintf(f, "%s", str.c_str());
                    uint8_t nul = 0;
                    fwrite(&nul, 1, 1, f);
                }
            }

            fclose(f);
        }
    }
};

class Plugin : public SceneExport
{
    bqMDL m_mdl;
    void _get_meshes(INode* node)
    {
       /* node->geti

        for (int i = 0; i < node->NumberOfChildren(); ++i)
        {
            _get_meshes(node->GetChildNode(i));
        }*/
    }
public:
    Plugin() {}
    virtual ~Plugin()
    {
        for (size_t i = 0, sz = m_sceneEntries.size(); i < sz; ++i)
        {
            delete m_sceneEntries[i];
        }
    }
    int	ExtCount() 
    {
        return 1; 
    }
    virtual const MCHAR* Ext(int n)
    {
        switch (n)
        {
        case 0:
            return L"bmdl";
        }
        return L"";
    }
    virtual const MCHAR* LongDesc()
    {
        return L"Badcoiq engine model";
    }
    virtual const MCHAR* ShortDesc()
    {
        return L"Badсoiq MDL";
    }
    virtual const MCHAR* AuthorName()
    {
        return L"badcoiq";
    }
    virtual const MCHAR* CopyrightMessage()
    {
        return L"badcoiq";
    }
    virtual const MCHAR* OtherMessage1()
    {
        return L"";
    }
    virtual const MCHAR* OtherMessage2()
    {
        return L"";
    }
    virtual unsigned int	Version()
    {
        return 100;
    }
    virtual void ShowAbout(HWND hWnd) {}

    /*void _do_write(FILE* f, INode* node)
    {
        auto name = node->GetName();
        if (name)
        {
            char cname[0xffff];
            WideCharToMultiByte(CP_UTF8, 0, name, -1, cname, 0xffff, 0, 0);

            fwrite(cname, 1, strlen(cname), f);
        }
        else
        {
            fwrite("[unk]", 1, 5, f);
        }
        fwrite("\n", 1, 1, f);

        for (int i = 0; i < node->NumberOfChildren(); ++i)
        {
            _do_write(f, node->GetChildNode(i));
        }
    }*/

    class SceneEntry 
    {
    public:
       // TSTR m_name;
        INode* m_node = 0;
        Object* m_obj = 0;
        
        SceneEntry(INode* n, Object* o)
            :
            m_node(n),
            m_obj(o)
        {
        }
    };

    std::vector<SceneEntry*> m_sceneEntries;
    class MyITreeEnumProc : public ITreeEnumProc
    {
        Plugin* m_plugin = 0;
    public:
        MyITreeEnumProc(Plugin* p) : m_plugin(p) {}
        virtual ~MyITreeEnumProc() {}
        virtual int callback(INode* node)
        {
           // if (exportSelected && node->Selected() == FALSE)
           //     return TREE_CONTINUE;

            Object* obj = node->EvalWorldState(m_plugin->m_timeValue).obj;
            if (obj->CanConvertToType(triObjectClassID)) 
            {                
                Append(node, obj->ConvertToType(m_plugin->m_timeValue, triObjectClassID));
             //   mtlList->AddMtl(node->GetMtl());
                return TREE_CONTINUE;
            }

            return TREE_CONTINUE;
        }

        void Append(INode* node, Object* obj)
        {
            m_plugin->m_sceneEntries.push_back(new SceneEntry(node, obj));

        }
    };

    TimeValue m_timeValue = 0;

    virtual int DoExport(const MCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts = FALSE, DWORD options = 0)
    {
        m_timeValue = i->GetTime();

        // Сначала надо получить всё необходимое.
        // Потом записывать в файл.

        MyITreeEnumProc nodeEnumerator(this);
        ei->theScene->EnumTree(&nodeEnumerator);
        for (size_t i = 0, sz = m_sceneEntries.size(); i < sz; ++i)
        {
            auto entry = m_sceneEntries[i];
            
            //entry->m_obj->PolygonCount
            
            int faceNum = 0;
            int vertNum = 0;
            GetPolygonCount(m_timeValue, entry->m_obj, faceNum, vertNum);
            if (faceNum)
            {
                auto nameIndex = m_mdl.AddString(entry->m_node->GetName());

                TriObject* triObj = dynamic_cast<TriObject*>(entry->m_obj);
                if (triObj)
                {
              /*  WideCharToMultiByte(CP_UTF8, 0, entry->m_node->GetName(), -1, cname, 0xffff, 0, 0);
                fwrite(cname, 1, strlen(cname), f);
                fwrite("\n", 1, 1, f);

                fprintf(f, "_faceNum = %i\n", faceNum);
                fprintf(f, "_vertNum = %i\n", vertNum);*/

                    uint32_t newVertNum = faceNum * 3;
                    uint8_t indexType = (newVertNum < 0xffff) ? 0 : 1;

                    bqMDLChunkHeaderMesh headerMesh;
                    headerMesh.m_name = nameIndex;
                    headerMesh.m_indexType = indexType;
                    headerMesh.m_vertNum = newVertNum;
                    headerMesh.m_indNum = newVertNum;
                    headerMesh.m_vertexType = 0;

                    int vsz = sizeof(bqMDLMeshVertex);

                    if(headerMesh.m_vertexType==1)
                        vsz = sizeof(bqMDLMeshVertexSkinned);

                    headerMesh.m_vertBufSz =
                        headerMesh.m_vertNum *
                        vsz;

                    if (headerMesh.m_indexType == 0)
                        headerMesh.m_indBufSz = headerMesh.m_indNum * 2;
                    else
                        headerMesh.m_indBufSz = headerMesh.m_indNum * 4;

                    auto newMesh = m_mdl.CreateMesh(headerMesh);

                    uint16_t* in16 = (uint16_t*)newMesh->m_indices;
                    uint32_t* in32 = (uint32_t*)newMesh->m_indices;

                    // количество индексов будет равно количеству вершин
                    // на каждую вершину свой индекс
                    // значит индексы будут просто идти попорядку 0 1 2 3 4 5...
                    for (uint32_t i = 0; i < headerMesh.m_indNum; ++i)
                    {
                        if (headerMesh.m_indexType == 0)
                            *in16 = i;
                        else
                            *in32 = i;

                        ++in16;
                        ++in32;
                    }

                    auto meshVerts = newMesh->m_vertices;

                    // теперь надо взять позиции
                    auto verts = triObj->mesh.getVertPtr(0);
                    auto faces = triObj->mesh.getFacePtr(0);
                    for (int i = 0; i < faceNum; ++i)
                    {
                        for (int o = 0; o < 3; ++o)
                        {
                            meshVerts->m_position[0] = verts[faces[i].v[o]].x;
                            meshVerts->m_position[1] = verts[faces[i].v[o]].y;
                            meshVerts->m_position[2] = verts[faces[i].v[o]].z;
                            ++meshVerts;
                        }
                    }
                    //auto vertNum = triObj->mesh.getNumVerts();

                }
            }
        }
        
        m_mdl.Save(name);

     //   auto root = ei->theScene;
        
        //if (root)
        //{
        //    /*FILE* f = 0;

        //    char cname[0xffff];
        //    WideCharToMultiByte(CP_UTF8, 0, name, -1, cname, 0xffff, 0, 0);
        //    fopen_s(&f, cname, "wb");
        //    if (f)
        //    {
        //        for (int i = 0; i < root->NumberOfChildren(); ++i)
        //        {
        //            _get_meshes(root->GetChildNode(i));
        //        }

        //        fclose(f);
        //    }*/
        //    for (int i = 0; i < root->NumberOfChildren(); ++i)
        //    {
        //        _get_meshes(root->GetChildNode(i));
        //    }
        //}

        return 1;
    }
    virtual BOOL			SupportsOptions(int ext, DWORD options) { UNUSED_PARAM(ext); UNUSED_PARAM(options); return FALSE; }
};

class ClassDescImpl : public ClassDesc
{
public:
    ClassDescImpl() {}
    virtual ~ClassDescImpl() {}

    virtual int				IsPublic() { return TRUE; };
    virtual void* Create(BOOL loading = FALSE)
    {
        return new Plugin;
    }

    virtual const MCHAR* ClassName()
    {
        return L"Test class";
    }

    virtual const MCHAR* NonLocalizedClassName()
    {
        return L"Test class";
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
static ClassDescImpl _description;

HINSTANCE hInstance = 0;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID /*lpvReserved*/)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        // Hang on to this DLL's instance handle.
        hInstance = hinstDLL;
        DisableThreadLibraryCalls(hInstance);
    }
    return(TRUE);
}

extern "C"
{
    __declspec(dllexport) const TCHAR* LibDescription()
    {
        // Retrieve astring from the resource string table
        static TCHAR buf[256];
        if (hInstance)
            return L"Test";
        return NULL;
    }

    __declspec(dllexport) int LibNumberClasses()
    {
        return 1;
    }

    __declspec(dllexport) ClassDesc* LibClassDesc(int i)
    {
        switch (i)
        {
        case 0:
            return &_description;
        }

        return 0;
    }

    __declspec(dllexport) ULONG LibVersion()
    {
        return Get3DSMAXVersion();
    }

    __declspec(dllexport) int LibInitialize()
    {
        // TODO: Perform initialization here.
        return TRUE;
    }

    __declspec(dllexport) int LibShutdown()
    {
        // TODO: Perform uninitialization here.
        return TRUE;
    }
}
