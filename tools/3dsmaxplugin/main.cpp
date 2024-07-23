#include "Max.h"
#include "bqmdlinfo.h"

#include <vector>
#include <string>
#include <map>

#include "fastlz.h"

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

class bqMDLMax
{
    uint32_t m_stringIndex = 0;
public:
    bqMDLMax()
    {
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        //printf("TEST\n");
    }
    ~bqMDLMax()
    {
        for (size_t i = 0, sz = m_meshes.size(); i < sz; ++i)
        {
            delete m_meshes[i];
        }
    }

    std::vector<std::string> m_strings;
    std::map<std::string, uint32_t> m_stringMap;
    std::vector<_Mesh*> m_meshes;

    FileBuffer m_fileBuffer;

    struct bone
    {
        std::string m_name;
        std::string m_parentName;
        int m_parent = -1;

        bqMDLBoneData m_boneData;
    };
    std::vector<bone> m_bones;

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
            auto stringChunkNum = m_strings.size();

            bqMDLFileHeader fileHeader;
            fileHeader.m_chunkNum = meshChunkNum
                + stringChunkNum;

            if (m_bones.size())
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
                auto & str = m_strings[i];

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

            if (m_bones.size())
            {
                size_t boneNum = m_bones.size();

                bqMDLChunkHeader chunkHeader;
                chunkHeader.m_chunkType = bqMDLChunkHeader::ChunkType_Skeleton;
                chunkHeader.m_chunkSz = sizeof(bqMDLChunkHeader)
                    + sizeof(bqMDLChunkHeaderSkeleton)
                    + (sizeof(bqMDLBoneData) * boneNum);
                m_fileBuffer.Add(&chunkHeader, sizeof(chunkHeader));

                bqMDLChunkHeaderSkeleton chunkHeaderSkeleton;
                chunkHeaderSkeleton.m_boneNum = boneNum;
                m_fileBuffer.Add(&chunkHeaderSkeleton, sizeof(chunkHeaderSkeleton));

                for (uint32_t i = 0; i < boneNum; ++i)
                {
                    auto& bone = m_bones[i];
                    m_fileBuffer.Add(&bone.m_boneData, sizeof(bone.m_boneData));
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
                if(m_fileBuffer.Size() && m_fileBuffer.Data())
                    fwrite(m_fileBuffer.Data(), 1, m_fileBuffer.Size(), f);
            }
            
            fseek(f, 0, SEEK_SET);
            fwrite(&fileHeader, 1, sizeof(fileHeader), f);

           
            fclose(f);
        }
    }

    void _skeletonFindParents()
    {
        // построение иерархии (то из чего и состоит скелет)
        // будет происходить поиском имени родителя.

        auto boneNum = m_bones.size();
        if (boneNum)
        {
            for (size_t i = 0; i < boneNum; ++i)
            {
                auto& bone = m_bones[i];
                bone.m_parent = -1;

                for (size_t pi = 0; pi < boneNum; ++pi)
                {
                    auto& secondBone = m_bones[pi];

                    if (bone.m_parentName == secondBone.m_name)
                    {
                        bone.m_parent = pi;
                        break;
                    }
                }
            }
        }
    }
};

class Plugin : public SceneExport
{
    bqMDLMax m_mdl;
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

            printf("superClassID %#010x\n", superClassID);
            printf("classID %#010x %#010x\n", classID.PartA(), classID.PartB());

            Append(node, obj);

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

        void Append(INode* node, Object* obj)
        {
            m_plugin->m_sceneEntries.push_back(new SceneEntry(node, obj));
        }
    };

    TimeValue m_timeValue = 0;

    void _onMesh(INode* node, Object* obj)
    {
        TriObject* triObj = dynamic_cast<TriObject*>(obj);
        if (triObj)
        {

            auto nameIndex = m_mdl.AddString(node->GetName());

            Mesh* mesh = &triObj->mesh;

            int faceNum = mesh->getNumFaces();
            int vertNum = mesh->getNumVerts();

            if (faceNum)
            {
                /*  WideCharToMultiByte(CP_UTF8, 0, entry->m_node->GetName(), -1, cname, 0xffff, 0, 0);
                  fwrite(cname, 1, strlen(cname), f);
                  fwrite("\n", 1, 1, f);

                  fprintf(f, "_faceNum = %i\n", faceNum);
                  fprintf(f, "_vertNum = %i\n", vertNum);*/

                uint32_t newVertNum = faceNum * 3;
                uint8_t indexType = (newVertNum < 0xffff) ? 0 : 1;

                bqMDLChunkHeaderMesh headerMesh;
                headerMesh.m_nameIndex = nameIndex;
                headerMesh.m_indexType = indexType;
                headerMesh.m_vertNum = newVertNum;
                headerMesh.m_indNum = newVertNum;
                headerMesh.m_vertexType = 0;

                int vsz = sizeof(bqMDLMeshVertex);

                if (headerMesh.m_vertexType == 1)
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
                for (uint32_t i2 = 0; i2 < headerMesh.m_indNum; ++i2)
                {
                    if (headerMesh.m_indexType == 0)
                        *in16 = i2;
                    else
                        *in32 = i2;

                    ++in16;
                    ++in32;
                }

                auto meshVerts = newMesh->m_vertices;

                // теперь надо взять позиции
                auto verts = mesh->getVertPtr(0);
                auto faces = mesh->getFacePtr(0);

                mesh->checkNormals(TRUE);

                Matrix3 tm = node->GetObjTMAfterWSM(m_timeValue);

                auto rVerts = mesh->getRVertPtr(0);
                auto tVerts = mesh->getTVertPtr(0);
                /*triObj->mesh.normalCount
                triObj->mesh.getNormal();*/
                /*
                     auto glambda = [&](Face* face, int o, int fi)
                         {
                             meshVerts->m_position[0] = verts[face->v[o]].x;
                             meshVerts->m_position[1] = verts[face->v[o]].z;
                             meshVerts->m_position[2] = verts[face->v[o]].y;

                             auto rv = rVerts[face->getVert(o)];
                             meshVerts->m_normal[0] = rv.rn.getNormal().x;
                             meshVerts->m_normal[1] = rv.rn.getNormal().z * -1.f;
                             meshVerts->m_normal[2] = rv.rn.getNormal().y;

                             if (tVerts)
                             {
                                 meshVerts->m_uv[0] = tVerts[mesh->tvFace[fi].t[o]].x;

                                 float V = tVerts[mesh->tvFace[fi].t[o]].y;
                                 meshVerts->m_uv[1] = V;
                             }

                             ++meshVerts;
                         };*/
                auto _GetVertexNormal = [&](Mesh* mesh, int faceNo, RVertex* rv) -> Point3
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
                    };

                int ti1 = 0;
                int ti2 = 1;
                int ti3 = 2;

                int fvx1 = 0;
                int fvx2 = 2;
                int fvx3 = 1;

                for (int fi = 0; fi < faceNum; ++fi)
                {
                    Face* face = &faces[fi];
                    auto& faceNormal = mesh->getFaceNormal(fi);

                    int vx1 = 0;
                    int vx2 = 1;
                    int vx3 = 2;

                    auto& faceVertIndex = face->v[fvx1];
                    Point3 pos = verts[faceVertIndex];
                    meshVerts->m_position[vx1] = pos.x * -1.f;
                    meshVerts->m_position[vx3] = pos.y;
                    meshVerts->m_position[vx2] = pos.z;

                    Point3 vn = _GetVertexNormal(mesh, fi, mesh->getRVertPtr(faceVertIndex));
                    meshVerts->m_normal[vx1] = vn.x;
                    meshVerts->m_normal[vx2] = vn.z * -1.f;
                    meshVerts->m_normal[vx3] = vn.y;

                    ++meshVerts;

                    faceVertIndex = face->v[fvx2];
                    pos = verts[faceVertIndex];
                    meshVerts->m_position[vx1] = pos.x * -1.f;
                    meshVerts->m_position[vx3] = pos.y;
                    meshVerts->m_position[vx2] = pos.z;
                    vn = _GetVertexNormal(mesh, fi, mesh->getRVertPtr(faceVertIndex));
                    meshVerts->m_normal[vx1] = vn.x;
                    meshVerts->m_normal[vx2] = vn.z * -1.f;
                    meshVerts->m_normal[vx3] = vn.y;
                    ++meshVerts;

                    faceVertIndex = face->v[fvx3];
                    pos = verts[faceVertIndex];
                    meshVerts->m_position[vx1] = pos.x * -1.f;
                    meshVerts->m_position[vx3] = pos.y;
                    meshVerts->m_position[vx2] = pos.z;
                    vn = _GetVertexNormal(mesh, fi, mesh->getRVertPtr(faceVertIndex));
                    meshVerts->m_normal[vx1] = vn.x;
                    meshVerts->m_normal[vx2] = vn.z * -1.f;
                    meshVerts->m_normal[vx3] = vn.y;
                    ++meshVerts;


                }

                meshVerts = newMesh->m_vertices;
                for (int fi = 0; fi < faceNum; ++fi)
                {
                    int numTVx = mesh->getNumTVerts();
                    if (numTVx)
                    {
                        auto tci1 = mesh->tvFace[fi].t[fvx1];
                        auto tci2 = mesh->tvFace[fi].t[fvx2];
                        auto tci3 = mesh->tvFace[fi].t[fvx3];

                        meshVerts->m_uv[0] = tVerts[tci1].x;
                        meshVerts->m_uv[1] = 1.f - tVerts[tci1].y;
                        ++meshVerts;

                        meshVerts->m_uv[0] = tVerts[tci2].x;
                        meshVerts->m_uv[1] = 1.f - tVerts[tci2].y;
                        ++meshVerts;

                        meshVerts->m_uv[0] = tVerts[tci3].x;
                        meshVerts->m_uv[1] = 1.f - tVerts[tci3].y;
                        ++meshVerts;
                    }
                }

            }
        }
    }

    void _onBone(INode* node, Object* obj)
    {
        bqMDLMax::bone newBone;
        printf("New bone\n");
        
        //wprintf(L"Name: %s\n", node->GetName());
        
        MaxSDK::Util::MaxString mstr;
        mstr.AllocBuffer(0xfff);
        mstr.SetUTF16(node->GetName());


        size_t len = 0;
        auto astr = mstr.ToUTF8(&len);
        if (len)
        {
            newBone.m_name.append(astr.data());
            printf("Name: %s\n", newBone.m_name.c_str());
        }
        else
        {
            return;
        }

        
        Matrix3 W = node->GetNodeTM(m_timeValue);
        printf("Matrix: \n\t%f %f %f\n\t%f %f %f\n\t%f %f %f\n\n",
            W.GetRow(0).x, W.GetRow(0).y, W.GetRow(0).z,
            W.GetRow(1).x, W.GetRow(1).y, W.GetRow(1).z, 
            W.GetRow(2).x, W.GetRow(2).y, W.GetRow(2).z);

        Control* pC = node->GetTMController()->GetPositionController();
        Control* rC = node->GetTMController()->GetRotationController();
        Control* sC = node->GetTMController()->GetScaleController();

        Point3 pos;
        Quat rot;
        ScaleValue sc;
        if (pC)
        {
            pC->GetValue(m_timeValue, &pos);

            printf("Position: %f %f %f\n", pos.x, pos.y, pos.z);

            Matrix3 parentW = node->GetParentTM(m_timeValue);
            Matrix3 T = W * parentW;

            printf("Matrix * parentW: \n\t%f %f %f\n\t%f %f %f\n\t%f %f %f\n\n",
                T.GetRow(0).x, T.GetRow(0).y, T.GetRow(0).z,
                T.GetRow(1).x, T.GetRow(1).y, T.GetRow(1).z,
                T.GetRow(2).x, T.GetRow(2).y, T.GetRow(2).z);
        }
        else
        {
            return;
        }

        if (rC)
        {
            rC->GetValue(m_timeValue, &rot);

            printf("Rotation: %f %f %f [%f]\n", rot.x, rot.y, rot.z, rot.w);
        }
        else
        {
            return;
        }

        if (sC)
        {
            sC->GetValue(m_timeValue, &sc);

            printf("Scale: %f %f %f\n", sc.s.x, sc.s.y, sc.s.z);
        }
        else
        {
            return;
        }

        auto parent = node->GetParentNode();
        if (parent)
        {
            mstr.SetUTF16(parent->GetName());

            size_t len = 0;
            auto astr = mstr.ToUTF8(&len);
            if (len)
            {
                newBone.m_parentName.append(astr.data());

                printf("Parent name: %s\n", newBone.m_parentName.c_str());
            }
            else
            {
                return;
            }
        }
        else
        {
            return;
        }

        newBone.m_boneData.m_nameIndex = m_mdl.AddString(node->GetName());
        newBone.m_boneData.m_position[0] = pos.x;
        newBone.m_boneData.m_position[1] = pos.y;
        newBone.m_boneData.m_position[2] = pos.z;
        newBone.m_boneData.m_scale[0] = sc.s.x;
        newBone.m_boneData.m_scale[1] = sc.s.y;
        newBone.m_boneData.m_scale[2] = sc.s.z;
        newBone.m_boneData.m_rotation[0] = rot.x;
        newBone.m_boneData.m_rotation[1] = rot.y;
        newBone.m_boneData.m_rotation[2] = rot.z;
        newBone.m_boneData.m_rotation[3] = rot.w;

        m_mdl.m_bones.push_back(newBone);
    }

    

    virtual int DoExport(const MCHAR* name, ExpInterface* ei, Interface* ip, BOOL suppressPrompts = FALSE, DWORD options = 0)
    {
        m_timeValue = ip->GetTime();

        // Сначала надо получить всё необходимое.
        // Потом записывать в файл.

        MyITreeEnumProc nodeEnumerator(this, ip);
        ei->theScene->EnumTree(&nodeEnumerator);
        for (size_t i = 0, sz = m_sceneEntries.size(); i < sz; ++i)
        {
            auto entry = m_sceneEntries[i];
            auto obj = entry->m_obj;
            if (obj->ClassID() == BONE_OBJ_CLASSID)
            {
                _onBone(entry->m_node, obj);
            }
            else if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
            {
                _onMesh(entry->m_node, obj->ConvertToType(m_timeValue, Class_ID(TRIOBJ_CLASS_ID, 0)));
            }
            
        }

        m_mdl._skeletonFindParents();
        
        m_mdl.Save(name);


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
