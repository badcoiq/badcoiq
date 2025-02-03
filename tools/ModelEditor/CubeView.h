/*
BSD 2-Clause License

Copyright (c) 2025, badcoiq
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

#ifndef _CUBEVIEW_H_
#define _CUBEVIEW_H_

class ViewportView;
class CubeView
{
    friend class ViewportView;
public:
    enum
    {
        meshID_front,
        meshID_back,
        meshID_top,
        meshID_bottom,
        meshID_left,
        meshID_right,
        meshID_TB,
        meshID_TL,
        meshID_TF,
        meshID_TR,
        meshID_BckR,
        meshID_BckL,
        meshID_FL,
        meshID_FR,
        meshID_BR,
        meshID_BB,
        meshID_BL,
        meshID_BF,
        meshID_TRB,
        meshID_TLB,
        meshID_TLF,
        meshID_TRF,
        meshID_BRF,
        meshID_BRB,
        meshID_BLB,
        meshID_BLF,

        meshID__size
    };


private:
    bqMesh* m_cubeViewMesh[meshID__size];
    bqGPUMesh* m_cubeViewGPUMesh[meshID__size];

    bqTexture* m_texture[meshID__size];
    bqTexture* m_texture_i[meshID__size];

    uint32_t _IsMouseRayIntersect(const bqRay&, uint32_t meshID, bqReal& T);
    uint32_t m_hoverMesh = meshID__size;
    uint32_t m_LMBHitMesh = meshID__size;
    
    bool m_isClickedLMB = false;
    bool m_isMouseMove = false;


public:
    CubeView();
    ~CubeView();

    bool Init();
    void Draw(bqCamera*);

    void Update(ViewportView*);

    uint32_t IsMouseRayIntersect(bqCamera*, const bqVec4f&);
};

#endif
