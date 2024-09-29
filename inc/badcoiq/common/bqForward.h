/*
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
#ifndef __BQ_FORWARD_H__
#define __BQ_FORWARD_H__


template<typename T>
class bqVec2_t;
template<typename T>
class bqVec3_t;
template<typename T>
class bqVec4_t;
template<typename T>
class bqMatrix4_t;
template<typename T>
class bqMatrix3_t;
template<typename T>
class bqMatrix2_t;

using bqVec2  = bqVec2_t<bqReal>;
using bqVec2f = bqVec2_t<float32_t>;
using bqVec2i = bqVec2_t<int32_t>;
using bqVec3  = bqVec3_t<bqReal>;
using bqVec3f = bqVec3_t<float32_t>;
using bqVec3i = bqVec3_t<int32_t>;
using bqVec4  = bqVec4_t<bqReal>;
using bqVec4f = bqVec4_t<float32_t>;
using bqVec4i = bqVec4_t<int32_t>;
using bqMat2 = bqMatrix2_t<float32_t>;
using bqMat3 = bqMatrix3_t<bqReal>;
using bqMat4  = bqMatrix4_t<bqReal>;


class bqMaterial;
class bqColor;
class bqAabb;
class bqCursor;
enum class bqCursorType : uint32_t;	

#ifdef BQ_WITH_SCENE
class bqSceneObject;
#endif

#ifdef BQ_WITH_WINDOW
class bqWindow;
class bqWindowCallback;
#endif

#ifdef BQ_WITH_GS
class bqGS;
#endif

#ifdef BQ_WITH_MESH
class bqPolygonMeshPolygon;
class bqMeshPolygonCreator;
class bqPolygonMeshControlPoint;
class bqGPUMesh;
class bqMesh;
class bqPolygonMesh;
class bqMeshLoader;
class bqMeshLoaderCallback;
class bqTriangle;
class bqSkeleton;
class bqSkeletonAnimation;
class bqMDL;
class bqSkeletonAnimationObject;
class bqMDLCollision;
#endif

#ifdef BQ_WITH_ARCHIVE
struct bqCompressionInfo;
struct bqArchiveZipFile;
#endif

#ifdef BQ_WITH_SOUND
class bqSound;
class bqSoundSystem;
struct bqSoundBufferInfo;
class bqSoundStream;
class bqSoundMixer;
class bqSoundEffect;
#endif

#ifdef BQ_WITH_IMAGE
class bqImage;
class bqTexture;
class bqImageLoader;
#endif

#ifdef BQ_WITH_SPRITE
class bqSprite;
#endif

#ifdef BQ_WITH_GUI
class bqGUIScrollbar;
class bqGUIFont;
class bqGUIDrawTextCallback;
enum class bqGUIStyleTheme;
struct bqGUIStyle;
class bqGUIWindow;
struct bqGUIState;
class bqGUIElement;
class bqGUICommon;
class bqGUIButton;
enum class bqGUIDefaultFont;
class bqGUITextEditor;
#endif

#ifdef BQ_WITH_PHYSICS
class bqGravityObject;
class bqPhysics;
class bqPhysicsShape;
class bqRigidBody;
class bqPhysicsShapeSphere;
class bqPhysicsDebugDraw;
#endif

class bqPopupData;

class bqVectorGraphicsShape;

#endif

