#include "Max.h"
#include <modstack.h>

#include <iskin.h>
#include <iskin.h>

#include "CS/BIPEXP.H"

#include <vector>
#include <string>
#include <map>

#include "fastlz.h"
#include "bqmdlinfo.h"

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

class _Mesh
{
public:
    _Mesh(const bqMDLChunkHeaderMesh& header)
    {
        m_chunkHeaderMesh = header;
        
        m_stride = sizeof(bqMDLMeshVertex);
        if (header.m_vertexType == header.VertexType_TriangleSkinned)
            m_stride = sizeof(bqMDLMeshVertexSkinned);

        m_vertices = (uint8_t*)malloc(header.m_vertBufSz);
        


        if (m_vertices)
        {
            uint8_t* v = m_vertices;

            for (uint32_t i = 0; i < header.m_vertNum; ++i)
            {
                memset(v, 0, m_stride);
                v += m_stride;
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

    uint8_t* m_vertices = 0;
    uint32_t m_stride = 0;

    uint8_t* m_indices = 0;

    INode* m_node = 0;
    TriObject* m_triObj = 0;
    ISkin* m_skin = 0;
    ISkinContextData* m_skinData = 0;
};


class Plugin : public SceneExport
{
    uint32_t m_stringIndex = 0;
    std::vector<std::string> m_strings;
    std::map<std::string, uint32_t> m_stringMap;
    std::vector<_Mesh*> m_meshes;
    FileBuffer m_fileBuffer;

public:
    Plugin() 
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
    }
    virtual ~Plugin()
    {
        for (size_t i = 0, sz = m_meshes.size(); i < sz; ++i)
        {
            delete m_meshes[i];
        }
        /*for (size_t i = 0, sz = m_sceneEntries.size(); i < sz; ++i)
        {
            delete m_sceneEntries[i];
        }*/
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
            return L"mdl";
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


    uint32_t AddString(const char* str)
    {
        uint32_t index = 0;

        auto iterator = m_stringMap.find(str);
        if (iterator == m_stringMap.end())
        {
            index = m_stringIndex;
            m_stringMap[str] = index;
            m_strings.push_back(str);
            
            printf("Add String: [%s][%i]\n", str, m_bonesNum, m_stringIndex);


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

    std::string& GetString(uint32_t i)
    {
        return m_strings[i];
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
            auto stringChunkNum = m_strings.size();

            bqMDLFileHeader fileHeader;
            fileHeader.m_chunkNum = meshChunkNum
                + stringChunkNum;

            if (m_bonesNum)
            {
                ++fileHeader.m_chunkNum; // bqMDLChunkHeaderSkeleton
            }

            fwrite(&fileHeader, 1, sizeof(fileHeader), f);


            for (uint32_t i = 0; i < meshChunkNum; ++i)
            {
                auto mesh = m_meshes[i];

                bqMDLChunkHeaderMesh meshChunkHeader;
                meshChunkHeader = mesh->m_chunkHeaderMesh;

                bqMDLChunkHeader chunkHeader;
                chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_Mesh;
                chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
                    + sizeof(bqMDLChunkHeaderMesh)
                    + meshChunkHeader.m_vertBufSz
                    + meshChunkHeader.m_indBufSz;

                m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));
                m_fileBuffer.Add(&meshChunkHeader, sizeof(meshChunkHeader));
                m_fileBuffer.Add(mesh->m_vertices, meshChunkHeader.m_vertBufSz);
                m_fileBuffer.Add(mesh->m_indices, meshChunkHeader.m_indBufSz);
            }

            for (uint32_t i = 0; i < stringChunkNum; ++i)
            {
                auto& str = m_strings[i];

                bqMDLChunkHeaderString stringChunkHeader;
                stringChunkHeader.m_strSz = str.size();

                bqMDLChunkHeader chunkHeader;
                chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_String;
                chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
                    + sizeof(bqMDLChunkHeaderString)
                    + stringChunkHeader.m_strSz;

                m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));
                m_fileBuffer.Add(&stringChunkHeader, sizeof(stringChunkHeader));
                m_fileBuffer.Add(str.data(), str.size());
            }

            if (m_bonesNum)
            {
                bqMDLChunkHeader chunkHeader;
                chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_Skeleton;
                chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
                    + sizeof(bqMDLChunkHeaderSkeleton)
                    + (sizeof(bqMDLBoneData) * m_bonesNum);
                m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));

                bqMDLChunkHeaderSkeleton chunkHeaderSkeleton;
                chunkHeaderSkeleton.m_boneNum = m_bonesNum;
                m_fileBuffer.Add(&chunkHeaderSkeleton, sizeof(chunkHeaderSkeleton));
                
                // кости нужно записать в порядке индексов от 0 до конца.
                // в map всё упорядочено по имени.
                for (int32_t i = 0; i < m_bonesNum; ++i)
                {
                    SkeletonBone sBone = GetBone(i);
                    if (sBone.m_node)
                    {

                        bqMDLBoneData boneData;
                        boneData.m_nameIndex = sBone.m_nameIndex;
                        boneData.m_parent = sBone.m_parentIndex;

                        if (sBone.m_parentIndex != -1)
                        {
                            SkeletonBone sBoneParent = GetBone(sBone.m_parentIndex);
                            printf("Bone [%s] parent [%s]\n", GetString(sBone.m_nameIndex).c_str(),
                                GetString(sBoneParent.m_nameIndex).c_str());
                        }
                        else
                            printf("Bone [%s] NO PARENT\n", GetString(sBone.m_nameIndex).c_str());

                        boneData.m_position[0] = sBone.m_position[0];
                        boneData.m_position[1] = sBone.m_position[1];
                        boneData.m_position[2] = sBone.m_position[2];
                        boneData.m_scale[0] = sBone.m_scale[0];
                        boneData.m_scale[1] = sBone.m_scale[1];
                        boneData.m_scale[2] = sBone.m_scale[2];
                        boneData.m_rotation[0] = sBone.m_rotation[0];
                        boneData.m_rotation[1] = sBone.m_rotation[1];
                        boneData.m_rotation[2] = sBone.m_rotation[2];
                        boneData.m_rotation[3] = sBone.m_rotation[3];

                        m_fileBuffer.Add(&boneData, sizeof(boneData));
                    }
                }
            }

            fileHeader.m_compression = fileHeader.compression_fastlz;
            if (fileHeader.m_compression)
            {
                switch (fileHeader.m_compression)
                {
                case fileHeader.compression_fastlz:
                default:
                {
                    auto sizeUncompressed = m_fileBuffer.Size();
                    uint8_t* output = (uint8_t*)malloc(sizeUncompressed + (sizeUncompressed / 3));
                    if (output)
                    {
                        int compressed_size = fastlz_compress_level(
                            (int)2,
                            m_fileBuffer.Data(),
                            sizeUncompressed,
                            output);

                        if ((uint32_t)compressed_size >= sizeUncompressed)
                        {
                        }
                        else
                        {
                            uint8_t* output2 = (uint8_t*)realloc(output, compressed_size);
                            if (output2)
                            {
                                output = output2;

                                fileHeader.m_cmpSz = compressed_size;
                                fileHeader.m_uncmpSz = sizeUncompressed;
                                fwrite(output, 1, compressed_size, f);

                            }
                        }

                        free(output);
                    }


                }break;
                }
            }
            else
            {
                if (m_fileBuffer.Size() && m_fileBuffer.Data())
                    fwrite(m_fileBuffer.Data(), 1, m_fileBuffer.Size(), f);
            }

            fseek(f, 0, SEEK_SET);
            fwrite(&fileHeader, 1, sizeof(fileHeader), f);


            fclose(f);
        }
    }


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

        float m_position[3];
        float m_scale[3];
        float m_rotation[4];
    };
    std::map<std::string, SkeletonBone> m_skeleton;
    int m_bonesNum = 0;


    void AddBone(INode* node, Object* obj)
    {
        std::string nodeName(GetAString(node->GetName()).c_str());


        // предполагается что будут добавляться
        // новые ноды, повторений не будет.
        // Но для аккуратности пусть будет поиск.
        if (m_skeleton.find(nodeName) == m_skeleton.end())
        {
            SkeletonBone b;
            b.m_index = m_bonesNum;
            b.m_node = node;
            b.m_nameIndex = AddString(nodeName.c_str());
            printf("Add Bone: [%s][%i][%I64x] nameIndex [%u]\n", 
                nodeName.c_str(), m_bonesNum, (uint64_t)node,
                b.m_nameIndex);

            ++m_bonesNum;

            m_skeleton[nodeName] = b;
        }
    }
    void BuildSkeleton()
    {
        printf("\nFind parents...\n");

        // Нахождение всех родителей
        auto it = m_skeleton.begin();
        while (it != m_skeleton.end())
        {
            SkeletonBone & bone = (*it).second;
            printf("Bone: [%s][%i] ... INode[%s][%I64x]\n",
                GetString(bone.m_nameIndex).c_str(),
                bone.m_index,
                GetAString(bone.m_node->GetName()).c_str(),
                (uint64_t)bone.m_node);

            INode* parentNode = bone.m_node->GetParentNode();
            if (parentNode)
            {
                printf("Parent INode name [%s][%I64x]\n", 
                    GetAString(parentNode->GetName()).c_str(),
                    (uint64_t)parentNode);

                if (parentNode != m_sceneRootNode)
                {

                    SkeletonBone parentBone = GetBone(parentNode);
                    if (parentBone.m_node == parentNode)
                    {
                        bone.m_parentIndex = parentBone.m_index;
                        printf("Set parent: ... [%s][%i]\n",
                            GetString(parentBone.m_nameIndex).c_str(),
                            parentBone.m_index);

                    }
                }
            }
            it++;
        }

        // установка позиции и т.д.
        it = m_skeleton.begin();
        while (it != m_skeleton.end())
        {
            SkeletonBone& bone = (*it).second;

            Matrix3 tm(bone.m_node->GetNodeTM(m_timeValue));
            Matrix3 ptm(bone.m_node->GetParentTM(m_timeValue));
            Control* tmc = bone.m_node->GetTMController();
            Class_ID cid = tmc->ClassID();
            if (cid == BIPBODY_CONTROL_CLASS_ID || cid == BIPED_CLASS_ID) {
                /*if (m_config.getInvertYZ()) {
                    Matrix3 m = RotateXMatrix(PI / 2.0f);
                    tm = tm * Inverse(m);
                }*/
            }
            else
                tm = tm * Inverse(ptm);
            Point3 pos = tm.GetTrans();
            AngAxis aa(tm);
            Quat q(tm);

            Control* sC = tmc->GetScaleController();
            ScaleValue sc;
            if (sC)
                sC->GetValue(m_timeValue, &sc);

            bone.m_position[0] = pos.x;
            bone.m_position[1] = pos.y;
            bone.m_position[2] = pos.z;
            bone.m_scale[0] = sc.s.x;
            bone.m_scale[1] = sc.s.y;
            bone.m_scale[2] = sc.s.z;
            bone.m_rotation[0] = q.x;
            bone.m_rotation[1] = q.y;
            bone.m_rotation[2] = q.z;
            bone.m_rotation[3] = q.w;

            it++;
        }
    }

    SkeletonBone GetBone(const char* name)
    {
        SkeletonBone bone;

        if (name)
        {
            auto it = m_skeleton.find(name);
            if (it != m_skeleton.end())
            {
                bone = (*it).second;
            }
        }

        return bone;
    }
    SkeletonBone GetBone(INode* node)
    {
        printf("GetBoneIBone [%s][%llu]\n",
            GetAString(node->GetName()).c_str(),
            (uint64_t)node);

        SkeletonBone bone;
        if (node)
        {
            auto it = m_skeleton.begin();
            while (it != m_skeleton.end())
            {
                SkeletonBone & _b = (*it).second;

                printf("\t- - - [%s][%llu]  -  -  - [%s][%llu]\n",
                    GetAString(node->GetName()).c_str(),
                    (uint64_t)node,
                    GetAString(_b.m_node->GetName()).c_str(),
                    (uint64_t)_b.m_node);

                if((uint64_t)node == (uint64_t)_b.m_node)
                {
                    printf("\t- - - FOUND\n");
                    return _b;
                }
                it++;
            }
        }
        return bone;
    }
    SkeletonBone GetBone(int index)
    {
        SkeletonBone bone;
        if (index >= 0)
        {
            auto it = m_skeleton.begin();
            while (it != m_skeleton.end())
            {
                if ((*it).second.m_index == index)
                {
                    return (*it).second;
                }
                it++;
            }
        }
        return bone;
    }


    std::string GetAString(const wchar_t* str)
    {
        MaxSDK::Util::MaxString mstr;
        mstr.AllocBuffer(0xfff);
        mstr.SetUTF16(str);

        size_t len = 0;
        auto astr = mstr.ToUTF8(&len);

        std::string s;

        if (len)
            s.append(astr.data());

        return s;
    }

    class MyITreeEnumProc : public ITreeEnumProc
    {
        Plugin* m_plugin = 0;
        Interface* m_ip = 0;
    public:
        MyITreeEnumProc(Plugin* p, Interface* ip) : m_plugin(p),
        m_ip(ip){}
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
                (c->ClassID() == BIPBODY_CONTROL_CLASS_ID)  ||
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
                m_plugin->AddMesh(node, obj->ConvertToType(m_plugin->m_timeValue, Class_ID(TRIOBJ_CLASS_ID, 0)));
                return TREE_CONTINUE;
            }
            //printf("superClassID %#010x\n", superClassID);
            //printf("classID %#010x %#010x\n", classID.PartA(), classID.PartB());

           // Append(node, obj);

            //if (obj->ClassID() != Class_ID(POLYOBJ_CLASS_ID, 0))
            //{
            //    if (obj->ClassID() == BONE_OBJ_CLASSID)
            //    {
            //        Append(node, obj);
            //    }
            //    //return TREE_CONTINUE;
            //}
            //else
            //{
            //    if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
            //    {
            //        Append(node, obj->ConvertToType(m_plugin->m_timeValue, Class_ID(TRIOBJ_CLASS_ID, 0)));
            //        //   mtlList->AddMtl(node->GetMtl());
            //        return TREE_CONTINUE;
            //    }
            //}

            return TREE_CONTINUE;
        }

       /* void Append(INode* node, Object* obj)
        {
            m_plugin->m_sceneEntries.push_back(new SceneEntry(node, obj));
        }*/
    };

    TimeValue m_timeValue = 0;
    INode* m_sceneRootNode = 0;

    //void _onMesh(INode* node, Object* obj)
    //{
    //    TriObject* triObj = dynamic_cast<TriObject*>(obj);
    //    if (triObj)
    //    {
    //        auto nameIndex = m_mdl.AddString(node->GetName());

    //        Mesh* mesh = &triObj->mesh;
    //        
    //        Matrix3 TM =  node->GetNodeTM(m_timeValue);

    //        int faceNum = mesh->getNumFaces();
    //        int vertNum = mesh->getNumVerts();

    //        if (faceNum)
    //        {
    //            bool skinReady = false;

    //            Modifier* skinModifier = 0;
    //            ISkin* skin = 0;
    //            ISkinContextData* skinData = 0;
    //           // if (m_mdl.m_bones.size())
    //            {
    //                //  mesh->setVDataSupport(VDATA_WEIGHT);
    //                //mesh->SupportVertexWeights();
    //                skinModifier = 0;

    //                Object* pObj = node->GetObjectRef();
    //                if (pObj)
    //                {
    //                    while (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
    //                    {
    //                        IDerivedObject* pDerObj = (IDerivedObject*)(pObj);
    //                        int Idx = 0;
    //                        while (Idx < pDerObj->NumModifiers())
    //                        {
    //                            // Get the modifier. 
    //                            Modifier* mod = pDerObj->GetModifier(Idx);

    //                            if (mod->ClassID() == SKIN_CLASSID)
    //                            {
    //                                skinModifier = mod;
    //                                break;
    //                            }
    //                            Idx++;
    //                        }
    //                        pObj = pDerObj->GetObjRef();
    //                    }
    //                }

    //                if (skinModifier)
    //                {
    //                    skin = (ISkin*)skinModifier->GetInterface(I_SKIN);
    //                    if (skin)
    //                    {
    //                        skinData = skin->GetContextInterface(node);
    //                        if (skinData)
    //                        {
    //                            skinReady = true;
    //                            _onSkin(skin, skinData);
    //                        }
    //                    }
    //                }

    //            }

    //            /*  WideCharToMultiByte(CP_UTF8, 0, entry->m_node->GetName(), -1, cname, 0xffff, 0, 0);
    //              fwrite(cname, 1, strlen(cname), f);
    //              fwrite("\n", 1, 1, f);

    //              fprintf(f, "_faceNum = %i\n", faceNum);
    //              fprintf(f, "_vertNum = %i\n", vertNum);*/

    //            uint32_t newVertNum = faceNum * 3;
    //            uint8_t indexType = (newVertNum < 0xffff) 
    //                ? bqMDLChunkHeaderMesh::IndexType_16bit
    //                : bqMDLChunkHeaderMesh::IndexType_32bit;

    //            bqMDLChunkHeaderMesh headerMesh;
    //            headerMesh.m_nameIndex = nameIndex;
    //            headerMesh.m_indexType = indexType;
    //            headerMesh.m_vertNum = newVertNum;
    //            headerMesh.m_indNum = newVertNum;

    //            headerMesh.m_vertexType = bqMDLChunkHeaderMesh::VertexType_Triangle;
    //            if(skinReady)
    //                headerMesh.m_vertexType = bqMDLChunkHeaderMesh::VertexType_TriangleSkinned;

    //            int vsz = sizeof(bqMDLMeshVertex);
    //            if (headerMesh.m_vertexType == bqMDLChunkHeaderMesh::VertexType_TriangleSkinned)
    //                vsz = sizeof(bqMDLMeshVertexSkinned);

    //            headerMesh.m_vertBufSz =
    //                headerMesh.m_vertNum *
    //                vsz;
    //            if (headerMesh.m_indexType == bqMDLChunkHeaderMesh::IndexType_16bit)
    //                headerMesh.m_indBufSz = headerMesh.m_indNum * 2;
    //            else
    //                headerMesh.m_indBufSz = headerMesh.m_indNum * 4;

    //            auto newMesh = m_mdl.CreateMesh(headerMesh);

    //            uint16_t* in16 = (uint16_t*)newMesh->m_indices;
    //            uint32_t* in32 = (uint32_t*)newMesh->m_indices;

    //            // количество индексов будет равно количеству вершин
    //            // на каждую вершину свой индекс
    //            // значит индексы будут просто идти попорядку 0 1 2 3 4 5...
    //            for (uint32_t i2 = 0; i2 < headerMesh.m_indNum; ++i2)
    //            {
    //                if (headerMesh.m_indexType == bqMDLChunkHeaderMesh::IndexType_16bit)
    //                    *in16 = i2;
    //                else
    //                    *in32 = i2;

    //                ++in16;
    //                ++in32;
    //            }

    //            uint8_t* vert8ptr = newMesh->m_vertices;

    //            // теперь надо взять позиции
    //            auto verts = mesh->getVertPtr(0);
    //            auto faces = mesh->getFacePtr(0);

    //            mesh->checkNormals(TRUE);

    //            Matrix3 tm = node->GetObjTMAfterWSM(m_timeValue);

    //            auto rVerts = mesh->getRVertPtr(0);
    //            auto tVerts = mesh->getTVertPtr(0);
    //            auto _GetVertexNormal = [&](Mesh* mesh, int faceNo, RVertex* rv) -> Point3
    //                {
    //                    Face* f = &mesh->faces[faceNo];
    //                    DWORD smGroup = f->smGroup;
    //                    int numNormals = 0;
    //                    Point3 vertexNormal;

    //                    // Is normal specified
    //                    // SPCIFIED is not currently used, but may be used in future versions.
    //                    if (rv->rFlags & SPECIFIED_NORMAL) {
    //                        vertexNormal = rv->rn.getNormal();
    //                    }
    //                    // If normal is not specified it's only available if the face belongs
    //                    // to a smoothing group
    //                    else if ((numNormals = rv->rFlags & NORCT_MASK) != 0 && smGroup) {
    //                        // If there is only one vertex is found in the rn member.
    //                        if (numNormals == 1) {
    //                            vertexNormal = rv->rn.getNormal();
    //                        }
    //                        else {
    //                            // If two or more vertices are there you need to step through them
    //                            // and find the vertex with the same smoothing group as the current face.
    //                            // You will find multiple normals in the ern member.
    //                            for (int i2 = 0; i2 < numNormals; i2++) {
    //                                if (rv->ern[i2].getSmGroup() & smGroup) {
    //                                    vertexNormal = rv->ern[i2].getNormal();
    //                                }
    //                            }
    //                        }
    //                    }
    //                    else {
    //                        // Get the normal from the Face if no smoothing groups are there
    //                        vertexNormal = mesh->getFaceNormal(faceNo);
    //                    }

    //                    return vertexNormal;
    //                };

    //            int ti1 = 0;
    //            int ti2 = 1;
    //            int ti3 = 2;
    //            
    //           /* int fvx1 = 0;
    //            int fvx2 = 2;
    //            int fvx3 = 1;*/
    //            int vertInds[3] = { 0, 1, 2 };
    //            printf("VNum %u\n", newMesh->m_chunkHeaderMesh.m_vertNum);
    //            printf("VType %u\n", newMesh->m_chunkHeaderMesh.m_vertexType);
    //            printf("VSize %u\n", newMesh->m_chunkHeaderMesh.m_vertBufSz);
    //            printf("Stride %u\n", newMesh->m_stride);

    //            /*
    //            
    //               */
    //            uint8_t* vptr = vert8ptr;
    //            bqMDLMeshVertex* meshVertex = (bqMDLMeshVertex*)vptr;
    //            for (int fi = 0; fi < faceNum; ++fi)
    //            {
    //                Face* face = &faces[fi];
    //                auto& faceNormal = mesh->getFaceNormal(fi);
    //                int vx1 = 0;
    //                int vx2 = 1;
    //                int vx3 = 2;
    //                for (int ii = 0; ii < 3; ++ii)
    //                {
    //                    auto& faceVertIndex = face->v[vertInds[ii]];
    //                    Point3 pos = verts[faceVertIndex];
    //                    meshVertex->m_position[vx1] = pos.x;
    //                    meshVertex->m_position[vx2] = pos.y * -1.f;
    //                    meshVertex->m_position[vx3] = pos.z;
    //                    Point3 vn = _GetVertexNormal(mesh, fi, mesh->getRVertPtr(faceVertIndex));
    //                    meshVertex->m_normal[vx1] = vn.x;
    //                    meshVertex->m_normal[vx2] = vn.z * -1.f;
    //                    meshVertex->m_normal[vx3] = vn.y;

    //                    vptr += newMesh->m_stride;
    //                    meshVertex = (bqMDLMeshVertex*)vptr;
    //                }

    //            }

    //            if (skinReady)
    //            {
    //                vptr = vert8ptr;
    //                meshVertex = (bqMDLMeshVertex*)vptr;
    //                for (int fi = 0; fi < faceNum; ++fi)
    //                {
    //                    Face* face = &faces[fi];

    //                    Matrix3 initTMBone;
    //                    Matrix3 initTMNode;
    //                    skin->GetSkinInitTM(node, initTMBone, false);
    //                    skin->GetSkinInitTM(node, initTMNode, true);
    //                    for (int ii = 0; ii < 3; ++ii)
    //                    {
    //                        auto& faceVertIndex = face->v[vertInds[ii]];
    //                        int boneNum = skin->GetNumBones();

    //                        bqMDLMeshVertexSkinned* meshVertexSkinned = (bqMDLMeshVertexSkinned*)vptr;

    //                        //  for (int iin = 0; iin < headerMesh.m_indNum; ++iin)
    //                        //  {
    //                        int nbones = skinData->GetNumAssignedBones(faceVertIndex);
    //                        for (int bi = 0; bi < nbones; ++bi)
    //                        {
    //                            meshVertexSkinned->m_weights[bi] = skinData->GetBoneWeight(faceVertIndex, bi);
    //                            meshVertexSkinned->m_boneInds[bi] = skinData->GetAssignedBone(faceVertIndex, bi);
    //                          //  printf("W: %f", meshVertexSkinned->m_weights[bi]);
    //                          //  printf(" I: %u\n", meshVertexSkinned->m_boneInds[bi]);
    //                        }
    //                       // if (nbones)
    //                       //     printf("\n");

    //                        vptr += newMesh->m_stride;
    //                    }
    //                }
    //            }

    //            vptr = vert8ptr;
    //            meshVertex = (bqMDLMeshVertex*)vptr;
    //            for (int fi = 0; fi < faceNum; ++fi)
    //            {
    //                int numTVx = mesh->getNumTVerts();
    //                if (numTVx)
    //                {
    //                    for (int ii = 0; ii < 3; ++ii)
    //                    {
    //                        auto tci1 = mesh->tvFace[fi].t[vertInds[ii]];
    //                        meshVertex->m_uv[0] = tVerts[tci1].x;
    //                        meshVertex->m_uv[1] = 1.f - tVerts[tci1].y;
    //                        vptr += newMesh->m_stride;
    //                        meshVertex = (bqMDLMeshVertex*)vptr;
    //                    }
    //                }
    //            }
    //            
    //        }
    //    }
    //}


    void AddMesh(INode* node, Object* obj)
    {
        bqMDLChunkHeaderMesh meshHeader;
        meshHeader.m_nameIndex = AddString(node->GetName());
        printf("Add Mesh\n");
        printf("-\tName: %s\n", GetAString(node->GetName()).c_str());

        TriObject* triObj = dynamic_cast<TriObject*>(obj);
        if (triObj)
        {            
            Modifier* skinModifier = 0;
            ISkin* skin = 0;
            ISkinContextData* skinData = 0;
            Object* pObj = node->GetObjectRef();
            while (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
            {
                IDerivedObject* pDerObj = (IDerivedObject*)(pObj);
                int Idx = 0;
                while (Idx < pDerObj->NumModifiers())
                {
                    // Get the modifier. 
                    Modifier* mod = pDerObj->GetModifier(Idx);
                    if (mod->ClassID() == SKIN_CLASSID)
                    {
                        skinModifier = mod;
                        break;
                    }
                    Idx++;
                }
                pObj = pDerObj->GetObjRef();
            }
            if (skinModifier)
            {
                skin = (ISkin*)skinModifier->GetInterface(I_SKIN);
                if (skin)
                    skinData = skin->GetContextInterface(node);
            }
         //   skinData = 0;

            Mesh* mesh = &triObj->mesh;
            int faceNum = mesh->getNumFaces();
            int vertNum = mesh->getNumVerts();
            printf("faceNum %u\n", faceNum);
            printf("vertNum %u\n", vertNum);

            if (faceNum)
            {
                meshHeader.m_vertNum = faceNum * 3;
                meshHeader.m_indexType = (meshHeader.m_vertNum < 0xffff)
                    ? bqMDLChunkHeaderMesh::IndexType_16bit
                    : bqMDLChunkHeaderMesh::IndexType_32bit;
                meshHeader.m_indNum = meshHeader.m_vertNum;
                
                meshHeader.m_vertexType = bqMDLChunkHeaderMesh::VertexType_Triangle;
                if (skinData)
                    meshHeader.m_vertexType = bqMDLChunkHeaderMesh::VertexType_TriangleSkinned;

                int vsz = sizeof(bqMDLMeshVertex);
                if (meshHeader.m_vertexType == bqMDLChunkHeaderMesh::VertexType_TriangleSkinned)
                    vsz = sizeof(bqMDLMeshVertexSkinned);

                meshHeader.m_vertBufSz = meshHeader.m_vertNum * vsz;
                
                if (meshHeader.m_indexType == bqMDLChunkHeaderMesh::IndexType_16bit)
                    meshHeader.m_indBufSz = meshHeader.m_indNum * 2;
                else
                    meshHeader.m_indBufSz = meshHeader.m_indNum * 4;
            }

            _Mesh* newM = new _Mesh(meshHeader);
            newM->m_triObj = triObj;
            newM->m_node = node;
          //  printf("-\t triObj :[%llu]\n", (uint64_t)triObj);
         //   printf("-\t node :[%llu]\n", (uint64_t)node);
         //   printf("-\t mesh :[%llu]\n", (uint64_t)mesh);

            if (skinData)
            {
                newM->m_skin = skin;
                newM->m_skinData = skinData;
            }
            m_meshes.push_back(newM);
        }
    }

    Point3 GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
    {
        Face* f = &mesh->faces[faceNo];
        DWORD smGroup = f->smGroup;
        int numNormals = 0;
        Point3 vertexNormal;

        // Is normal specified
        // SPCIFIED is not currently used, but may be used in future versions.
        if (rv->rFlags & SPECIFIED_NORMAL) {
            vertexNormal = rv->rn.getNormal();
        }
        // If normal is not specified it's only available if the face belongs
        // to a smoothing group
        else if ((numNormals = rv->rFlags & NORCT_MASK) != 0 && smGroup) {
            // If there is only one vertex is found in the rn member.
            if (numNormals == 1) {
                vertexNormal = rv->rn.getNormal();
            }
            else {
                // If two or more vertices are there you need to step through them
                // and find the vertex with the same smoothing group as the current face.
                // You will find multiple normals in the ern member.
                for (int i2 = 0; i2 < numNormals; i2++) {
                    if (rv->ern[i2].getSmGroup() & smGroup) {
                        vertexNormal = rv->ern[i2].getNormal();
                    }
                }
            }
        }
        else {
            // Get the normal from the Face if no smoothing groups are there
            vertexNormal = mesh->getFaceNormal(faceNo);
        }

        return vertexNormal;
    }

    void BuildMeshes()
    {
        printf("BuildMeshes\n");
        printf("Num of meshes %zu\n", m_meshes.size());

        for (size_t i = 0, sz = m_meshes.size(); i < sz; ++i)
        {
            _Mesh* newMesh = m_meshes[i];
            Mesh* mesh = &newMesh->m_triObj->mesh;

         //   printf("-\t newMesh->m_triObj :[%llu]\n", (uint64_t)newMesh->m_triObj);
         //   printf("-\t mesh :[%llu]\n", (uint64_t)mesh);
         //   printf("-\t newMesh->m_node :[%llu]\n", (uint64_t)newMesh->m_node);

            auto headerMesh = newMesh->m_chunkHeaderMesh;

            uint16_t* in16 = (uint16_t*)newMesh->m_indices;
            uint32_t* in32 = (uint32_t*)newMesh->m_indices;

            printf("Index Num %u\n", headerMesh.m_indNum);
            printf("Index Type %u\n", headerMesh.m_indexType);

            // количество индексов будет равно количеству вершин
            // на каждую вершину свой индекс
            // значит индексы будут просто идти попорядку 0 1 2 3 4 5...
            for (uint32_t i2 = 0; i2 < headerMesh.m_indNum; ++i2)
            {
                if (headerMesh.m_indexType == bqMDLChunkHeaderMesh::IndexType_16bit)
                    *in16 = i2;
                else
                    *in32 = i2;

                    ++in16;
                    ++in32;
            }
            uint8_t* vert8ptr = newMesh->m_vertices;
            
            // теперь надо взять позиции
            auto verts = mesh->getVertPtr(0);
            /*{
                int vertNum = mesh->getNumVerts();
                for (int j = 0; j < vertNum; ++j)
                {
                    printf("P: %f %f %f\n", verts[j].x, verts[j].y, verts[j].z);
                }
            }*/
            auto faces = mesh->getFacePtr(0);

            mesh->checkNormals(TRUE);
            auto rVerts = mesh->getRVertPtr(0);
            auto tVerts = mesh->getTVertPtr(0);

            int ti1 = 0;
            int ti2 = 1;
            int ti3 = 2;
            int vertInds[3] = { 0, 2, 1 };

            printf("VNum %u\n", newMesh->m_chunkHeaderMesh.m_vertNum);
            printf("VType %u\n", newMesh->m_chunkHeaderMesh.m_vertexType);
            printf("VSize %u\n", newMesh->m_chunkHeaderMesh.m_vertBufSz);
            printf("Stride %u\n", newMesh->m_stride);

            int faceNum = mesh->getNumFaces();
            printf("faceNum %u\n", faceNum);

            uint8_t* vptr = vert8ptr;
            bqMDLMeshVertex* meshVertex = (bqMDLMeshVertex*)vptr;
            for (int fi = 0; fi < faceNum; ++fi)
            {
                Face* face = &faces[fi];
                
                //auto& faceNormal = mesh->getFaceNormal(fi);
                int vx1 = 0;
                int vx2 = 1;
                int vx3 = 2;
                for (int ii = 0; ii < 3; ++ii)
                {
                    auto faceVertIndex = face->v[vertInds[ii]];

                  //  printf("F ii[%i] faceVertIndex[%i]\n", ii, faceVertIndex);

                    Point3 pos = verts[faceVertIndex];
                 //   printf("P %f %f %f\n", pos.x, pos.y, pos.z);

                    meshVertex->m_position[vx1] = pos.x;
                    meshVertex->m_position[vx2] = pos.y;
                    meshVertex->m_position[vx3] = pos.z;
                    Point3 vn = GetVertexNormal(mesh, fi, mesh->getRVertPtr(faceVertIndex));
                    meshVertex->m_normal[vx1] = vn.x;
                    meshVertex->m_normal[vx2] = vn.y;
                    meshVertex->m_normal[vx3] = vn.z;

                //    printf("%f %f %f\n", meshVertex->m_position[0], meshVertex->m_position[1], meshVertex->m_position[2]);

                    vptr += newMesh->m_stride;
                    meshVertex = (bqMDLMeshVertex*)vptr;

                 }
            }

            if (newMesh->m_skinData)
            {
                vptr = vert8ptr;
                meshVertex = (bqMDLMeshVertex*)vptr;

                int boneNum = newMesh->m_skin->GetNumBones();
                std::vector<INode*> _bones;
                for (int bi = 0; bi < boneNum; ++bi)
                {
                    _bones.push_back(newMesh->m_skin->GetBone(bi));
                }

                for (int fi = 0; fi < faceNum; ++fi)
                {
                    Face* face = &faces[fi];

                    Matrix3 initTMBone;
                    Matrix3 initTMNode;
                    newMesh->m_skin->GetSkinInitTM(newMesh->m_node, initTMBone, false);
                    newMesh->m_skin->GetSkinInitTM(newMesh->m_node, initTMNode, true);
                    for (int ii = 0; ii < 3; ++ii)
                    {
                        auto& faceVertIndex = face->v[vertInds[ii]];

                        bqMDLMeshVertexSkinned* meshVertexSkinned = (bqMDLMeshVertexSkinned*)vptr;

                        //  for (int iin = 0; iin < headerMesh.m_indNum; ++iin)
                        //  {
                        int nbones = newMesh->m_skinData->GetNumAssignedBones(faceVertIndex);
                        for (int bi = 0; bi < nbones; ++bi)
                        {
                            meshVertexSkinned->m_weights[bi] = newMesh->m_skinData->GetBoneWeight(faceVertIndex, bi);

                            int boneIndex = newMesh->m_skinData->GetAssignedBone(faceVertIndex, bi);
                            INode* _b = _bones[boneIndex];
                            SkeletonBone skeletonBone = GetBone(GetAString(_b->GetName()).c_str());
                            meshVertexSkinned->m_boneInds[bi] = skeletonBone.m_index;

                            //INode* skinBone = newMesh->m_skin->GetBone(boneIndex);
                            //meshVertexSkinned->m_boneInds[bi] = newMesh->m_skinData->GetAssignedBone(faceVertIndex, bi);

                           // meshVertexSkinned->m_boneInds[bi] = boneIndex;
                            //  printf(" W: %f", meshVertexSkinned->m_weights[bi]);
                            //  printf(" I: %u\n", meshVertexSkinned->m_boneInds[bi]);
                        }
                        // if (nbones)
                        //     printf("\n");

                        vptr += newMesh->m_stride;
                    }
                }
            }

            vptr = vert8ptr;
            meshVertex = (bqMDLMeshVertex*)vptr;
            for (int fi = 0; fi < faceNum; ++fi)
            {
                int numTVx = mesh->getNumTVerts();
                if (numTVx)
                {
                    for (int ii = 0; ii < 3; ++ii)
                    {
                        auto tci1 = mesh->tvFace[fi].t[vertInds[ii]];
                        meshVertex->m_uv[0] = tVerts[tci1].x;
                        meshVertex->m_uv[1] = 1.f - tVerts[tci1].y;
                        vptr += newMesh->m_stride;
                        meshVertex = (bqMDLMeshVertex*)vptr;
                    }
                }
            }
        }
    }
    
    virtual int DoExport(const MCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts = FALSE, DWORD options = 0)
    {
        m_timeValue = ip->GetAnimRange().Start();
        m_sceneRootNode = ip->GetRootNode();

        // Сначала надо получить всё необходимое.
        // Потом записывать в файл.

        MyITreeEnumProc nodeEnumerator(this, ip);
        ei->theScene->EnumTree(&nodeEnumerator);

        BuildSkeleton();
        BuildMeshes();
        
        Save(name);


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
