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

#include "../../DemoApp.h"

#include "badcoiq/scene/bqSceneObject.h"
#include "badcoiq/geometry/bqSkeleton.h"


ExampleBasicsSkeletalAnimation::ExampleBasicsSkeletalAnimation(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsSkeletalAnimation::~ExampleBasicsSkeletalAnimation()
{
}

void ExampleBasicsSkeletalAnimation::_onCamera()
{
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	if (bqInput::IsKeyHold(bqInput::KEY_SPACE))
	{
		m_camera->Rotate(bqInput::GetData()->m_mouseMoveDelta, *m_app->m_dt);

		// move cursor to center of the screen
		bqPoint windowCenter;
		m_app->GetWindow()->GetCenter(windowCenter);
		bqInput::SetMousePosition(m_app->GetWindow(), windowCenter.x, windowCenter.y);
	}

	if (bqInput::IsKeyHold(bqInput::KEY_A))
		m_camera->MoveLeft(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_D))
		m_camera->MoveRight(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_W))
		m_camera->MoveForward(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_S))
		m_camera->MoveBackward(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_Q))
		m_camera->MoveDown(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_E))
		m_camera->MoveUp(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_R))
		m_camera->Rotate(0.f, 0.f, 10.f * *m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_F))
		m_camera->Rotate(0.f, 0.f, -10.f * *m_app->m_dt);
}

bool ExampleBasicsSkeletalAnimation::Init()
{
	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(0.f, 10.f, 8.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(0.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqPolygonMesh pm;
	bqMat4 mat;
	pm.AddSphere(1.f, 32, mat);
	pm.GenerateNormals(true);
	pm.GenerateUVPlanar(211.f);
	
	auto m = pm.CreateMesh();
	m->ToSkinned();

	m_skeleton = new bqSkeleton;
	mat.Identity();
	bqVec4 P;
	P.Set(0.f, -1.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "0", -1);
	P.Set(0.f, 2.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "1", 0);
	P.Set(-1.f, 0.f, 0.f, 1.f);
	m_skeleton->AddJoint(bqQuaternion(), P, bqVec4(1.0), "2", 1);
	m_skeleton->CalculateBind();

	m->ApplySkeleton(m_skeleton);

	m_mesh = m_gs->CreateMesh(m);
	delete m;

	m_sceneObject = new bqSceneObject;

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsSkeletalAnimation::Shutdown()
{
	BQ_SAFEDESTROY(m_skeleton);
	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_mesh);
	BQ_SAFEDESTROY(m_sceneObject);
}

void ExampleBasicsSkeletalAnimation::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	_onCamera();


	m_gs->BeginGUI();
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	// Обязательно надо установить матрицы
	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);

	//m_sceneObject->GetPosition().x = -5.f;
	m_sceneObject->RecalculateWorldMatrix();
	auto & W = m_sceneObject->GetMatrixWorld();
	bqFramework::SetMatrix(bqMatrixType::World, &W);

	bqMat4 WVP;
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);

	m_gs->SetMesh(m_mesh);

	bqMaterial material;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_shaderType = bqShaderType::StandartSkinned;
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);
	m_gs->SetShader(material.m_shaderType, 0);

	bqFramework::GetMatrixSkinned()[0] = m_skeleton->GetJoints().m_data[0].m_data.m_matrixFinal;
	bqFramework::GetMatrixSkinned()[1] = m_skeleton->GetJoints().m_data[1].m_data.m_matrixFinal;
	bqFramework::GetMatrixSkinned()[2] = m_skeleton->GetJoints().m_data[2].m_data.m_matrixFinal;

	m_gs->Draw();

	// rotate and draw some joints
	static float a = 0.f; a += 1.1f * (*m_app->m_dt); if (a > PIPI) a = 0.f;
	if (bqInput::IsKeyHit(bqInput::KEY_F1))
		a = 0.f;
	m_skeleton->GetJoints().m_data[2].m_data.m_transformation.m_base.m_rotation.Set(a, 0, 0);
	m_skeleton->GetJoints().m_data[2].m_data.m_transformation.CalculateMatrix();
	m_skeleton->GetJoints().m_data[1].m_data.m_transformation.m_base.m_rotation.Set(0, a, 0);
	m_skeleton->GetJoints().m_data[1].m_data.m_transformation.CalculateMatrix();
	m_skeleton->Update();

	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	m_gs->SetShader(bqShaderType::Line3D, 0);
	m_gs->DisableDepth();
	auto joints = &m_skeleton->GetJoints().m_data[0];
	if (m_skeleton->GetJoints().m_size)
	{
		size_t jsz = m_skeleton->GetJoints().m_size;
		for (size_t i = 0; i < jsz; ++i)
		{
			bqVec3 p;
			bqVec3 p2;
			float sz = 0.3f;
			p = bqVec3(sz, 0., 0.);


			bqMat4 mI2 = joints[i].m_data.m_matrixBindInverse;
			mI2.Invert();

			bqMat4 mI = joints[i].m_data.m_matrixFinal * mI2;

			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3].GetVec3(), mI.m_data[3].GetVec3() + p2, bq::ColorRed);

			p = bqVec3(0., sz, 0.);
			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3].GetVec3(), mI.m_data[3].GetVec3() + p2, bq::ColorLime);

			p = bqVec3(0., 0., sz);
			bqMath::Mul(mI, p, p2);
			m_gs->DrawLine3D(mI.m_data[3].GetVec3(), mI.m_data[3].GetVec3() + p2, bq::ColorBlue);
		}
	}
	m_gs->EnableDepth();

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

// ================================ ================================
// ================================ ================================
// ================================ ================================
// ================================ ================================

class MyMegaModelClass
{
	class mesh_buffer
	{
	public:
		mesh_buffer() {}
		~mesh_buffer()
		{
			if (m_mesh)
				delete m_mesh;
		}

		bqGPUMesh* m_mesh = 0;
		bqTexture* m_texture = 0;
		bqString m_materialName;
	};

	class MyMeshLoaderCallback : public bqMeshLoaderCallback, public bqUserData
	{
	public:
		MyMeshLoaderCallback() {}
		virtual ~MyMeshLoaderCallback() {}

		virtual void OnSkeleton(bqSkeleton* s)
		{
			if (s)
			{
				MyMegaModelClass* m = (MyMegaModelClass*)GetUserData();
				if (!m->m_skeleton)
				{
					m->m_skeleton = s;
					s->CalculateBind();
					s->Update();
				}
				else
				{
					// каждый reference имеет свой скелет
					// вполне может быть такое что скелеты разные
					// и в таком случае надо будет что-то сделать.
					//  Какую нибудь проверку, и так сказать увеличение
					//  количества джоинтов если надо.
					// пока удаляю
					bqDestroy(s);
				}
			}
		}
		virtual void OnAnimation(bqSkeletonAnimation* a)
		{
			if (a)
			{
				MyMegaModelClass* m = (MyMegaModelClass*)GetUserData();
				m->m_loadedAnimations.push_back(a);
			}
		}

		virtual void OnMaterial(bqMaterial* m) override
		{
			if (m)
			{
				bqStringA stra;
				m->m_name.to_utf8(stra);

				MyMegaModelClass* mesh = (MyMegaModelClass*)GetUserData();

				mesh->m_materials.push_back(*m);
			}
		}

		virtual void OnMesh(bqMesh* newMesh, bqString* name, bqString* materialName) override
		{
			if (newMesh)
			{
				if (name)
				{
					bqStringA stra;
					name->to_utf8(stra);
				}

				MyMegaModelClass* m = (MyMegaModelClass*)GetUserData();
				mesh_buffer* mb = new mesh_buffer;
				mb->m_mesh = m->m_gs->CreateMesh(newMesh);
				m->m_meshBuffers.push_back(mb);
				mb->m_materialName = *materialName;

				bqDestroy(newMesh);
			}
		}

		virtual void Finale() override
		{
			MyMegaModelClass* m = (MyMegaModelClass*)GetUserData();
			m->OnFinale();
		}
	};
	MyMeshLoaderCallback m_cb;

public:
	MyMegaModelClass(bqGS* gs) :m_gs(gs)
	{
		m_cb.SetUserData(this);
	}
	~MyMegaModelClass() 
	{
		BQ_SAFEDESTROY(m_skeletonForAnimation);
		BQ_SAFEDESTROY(m_skeleton);
		BQ_SAFEDESTROY(m_skeletonAnimationObject);

		for (size_t i = 0; i < m_loadedAnimations.m_size; ++i)
		{
			delete m_loadedAnimations.m_data[i];
		}

		for (size_t i = 0; i < m_loadedTextures.m_size; ++i)
		{
			delete m_loadedTextures.m_data[i];
		}

		for (size_t i = 0; i < m_meshBuffers.m_size; ++i)
		{
			delete m_meshBuffers.m_data[i];
		}
	}

	void Load(const char* f)
	{
		bqFramework::CreateMesh(f, &m_cb);
	}

	void OnFinale()
	{
		// надо найти материалы по имени, и загрузить текстуры.
		for (size_t i = 0; i < m_meshBuffers.m_size; ++i)
		{
			mesh_buffer* mb = m_meshBuffers.m_data[i];

			bqMaterial* material = GetMaterial(m_meshBuffers.m_data[i]->m_materialName);
			if (material)
			{
				if (material->m_maps[0].m_filePath)
				{
					bqImage* img = bqFramework::CreateImage((const char*)material->m_maps[0].m_filePath);
					if (img)
					{
						bqTextureInfo ti;
						mb->m_texture = m_gs->CreateTexture(img, ti);
						m_loadedTextures.push_back(mb->m_texture);
						delete img;
					}
				}
			}
		}

		if (m_skeleton)
		{
			m_skeletonForAnimation = m_skeleton->Duplicate();
			
			for (size_t i = 0; i < m_loadedAnimations.m_size; ++i)
			{
				// правильнее хранить эти объекты в объектах сцены
				// то есть сейчас мы в классе модели.
				//  достаточно 1 объекта на все объекты на сцене
				//   анимации нужны в одном экземпляре
				// а текущая поза (хранится в скелете) и объект анимации
				//   уже требуется отдельные на каждый отдельный объект на сцене
				bqSkeletonAnimationObject* ao = new bqSkeletonAnimationObject;
				m_animationObjects.push_back(ao);
				ao->Init(m_loadedAnimations.m_data[i], m_skeletonForAnimation, "test");
			}
		}
	}

	bqMaterial* GetMaterial(const bqString& name)
	{
		for (size_t i = 0; i < m_materials.m_size; ++i)
		{
			if (m_materials.m_data[i].m_name == name)
				return &m_materials.m_data[i];
		}

		return 0;
	}

	void SetScale(float v)
	{
		if (m_skeletonForAnimation)
		{
			bqMat4 S;
			S.m_data[0].x = v;
			S.m_data[1].y = v;
			S.m_data[2].z = v;
			m_skeletonForAnimation->m_preRotation = m_skeleton->m_preRotation * S;
			m_skeletonForAnimation->Update();
		}
	}

	bqSkeleton* m_skeleton = 0;
	bqSkeleton* m_skeletonForAnimation = 0;
	bqSkeletonAnimationObject* m_skeletonAnimationObject = 0;
	bqSkeletonAnimationObject* m_currSkeletonAnimationObject = 0;

	bqArray<bqMaterial> m_materials;
	bqArray<mesh_buffer*> m_meshBuffers;
	bqArray<bqTexture*> m_loadedTextures;
	bqArray<bqSkeletonAnimation*> m_loadedAnimations;
	bqArray<bqSkeletonAnimationObject*> m_animationObjects;
	bqGS* m_gs = 0;
};

ExampleBasicsSkeletalAnimation2::ExampleBasicsSkeletalAnimation2(DemoApp* app)
	:
	DemoExample(app)
{
}

ExampleBasicsSkeletalAnimation2::~ExampleBasicsSkeletalAnimation2()
{
}

void ExampleBasicsSkeletalAnimation2::_onCamera()
{
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	if (bqInput::IsKeyHold(bqInput::KEY_SPACE))
	{
		m_camera->Rotate(bqInput::GetData()->m_mouseMoveDelta, *m_app->m_dt);

		// move cursor to center of the screen
		bqPoint windowCenter;
		m_app->GetWindow()->GetCenter(windowCenter);
		bqInput::SetMousePosition(m_app->GetWindow(), windowCenter.x, windowCenter.y);
	}

	if (bqInput::IsKeyHold(bqInput::KEY_A))
		m_camera->MoveLeft(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_D))
		m_camera->MoveRight(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_W))
		m_camera->MoveForward(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_S))
		m_camera->MoveBackward(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_Q))
		m_camera->MoveDown(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_E))
		m_camera->MoveUp(*m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_R))
		m_camera->Rotate(0.f, 0.f, 10.f * *m_app->m_dt);
	if (bqInput::IsKeyHold(bqInput::KEY_F))
		m_camera->Rotate(0.f, 0.f, -10.f * *m_app->m_dt);
}

bool ExampleBasicsSkeletalAnimation2::Init()
{
	const char* zipFileName = "../data/models/JillGlass.zip";
	
	bqString appPath = bqFramework::GetAppPath();
	bqString zipPath = appPath;
	zipPath.append(zipFileName);

	bqStringA strutf8;
	zipPath.to_utf8(strutf8);

	m_zipFile = bqArchiveSystem::ZipAdd(strutf8.c_str());
	if (!m_zipFile)
	{
		bqLog::PrintError("Can't add zip file [%s]\n", zipFileName);
		return false;
	}

	bqArray<bqStringA> files;
	bqArchiveSystem::GetFileList(m_zipFile, files);
	for (size_t i = 0; i < files.m_size; ++i)
	{
		files.m_data[i].to_lower();
	}

	if (!files.m_size)
	{
		bqLog::PrintError("Zip file is empty or corrupted [%s]\n", zipFileName);
		return false;
	}

	m_camera = new bqCamera();
	m_camera->m_position = bqVec3(0.f, 10.f, 8.f);
	m_camera->m_aspect = (float)m_app->GetWindow()->GetCurrentSize()->x / (float)m_app->GetWindow()->GetCurrentSize()->y;
	m_camera->Rotate(0.f, -45.f, 0.f);
	m_camera->SetType(bqCamera::Type::Perspective);
	m_camera->Update(0.f);
	m_camera->m_viewProjectionMatrix = m_camera->GetMatrixProjection() * m_camera->GetMatrixView();

	bqPolygonMesh pm;
	bqMat4 mat;
	pm.AddSphere(1.f, 32, mat);
	pm.GenerateNormals(true);
	pm.GenerateUVPlanar(211.f);

	m_sceneObject = new bqSceneObject;

	m_model = new MyMegaModelClass(m_gs);

	for (size_t i = 0; i < files.m_size; ++i)
	{
		if (files.m_data[i].check_extension(".smd"))
		{
			m_model->Load(files.m_data[i].c_str());
		}
	}
	m_model->OnFinale();
	m_model->SetScale(0.2f);

	// для 3D линии
	bqFramework::SetMatrix(bqMatrixType::ViewProjection, &m_camera->m_viewProjectionMatrix);

	m_gs->DisableBackFaceCulling();
	return true;
}

void ExampleBasicsSkeletalAnimation2::Shutdown()
{
	if (m_zipFile)
	{
		bqArchiveSystem::ZipRemove(m_zipFile);
		m_zipFile = 0;
	}

	if (m_model)
	{
		delete m_model;
		m_model = 0;
	}

//	BQ_SAFEDESTROY(m_skeleton);
	BQ_SAFEDESTROY(m_camera);
	BQ_SAFEDESTROY(m_sceneObject);
}

void ExampleBasicsSkeletalAnimation2::OnDraw()
{
	if (bqInput::IsKeyHit(bqInput::KEY_ESCAPE))
	{
		m_app->StopExample();
		return;
	}

	_onCamera();


	m_gs->BeginGUI();
	m_gs->DrawGUIText(U"Жми стрелки лево право чтобы сменить анимацию", 46, bqVec2f(), m_app->GetTextDrawCallback());
	m_gs->EndGUI();

	m_gs->BeginDraw();
	m_gs->ClearAll();

	// Обязательно надо установить матрицы
	bqMat4 viewI = m_camera->GetMatrixView();
	viewI.Invert();
	bqFramework::SetMatrix(bqMatrixType::ViewInvert, &viewI);
	bqFramework::SetMatrix(bqMatrixType::View, &m_camera->m_viewMatrix);
	bqFramework::SetMatrix(bqMatrixType::Projection, &m_camera->m_projectionMatrix);

	//m_sceneObject->GetPosition().x = -5.f;
	m_sceneObject->RecalculateWorldMatrix();
	auto& W = m_sceneObject->GetMatrixWorld();
	bqFramework::SetMatrix(bqMatrixType::World, &W);

	bqMat4 WVP;
	WVP = m_camera->m_projectionMatrix * m_camera->m_viewMatrix * W;
	bqFramework::SetMatrix(bqMatrixType::WorldViewProjection, &WVP);

	bqMaterial material;
	material.m_sunPosition.Set(2.f, 2.f, 1.f);
	material.m_sunPosition.Normalize();
	
	// из-за m_preRotation освещение не такое какое нужно
	// возможно надо изменить нормали в вершинах
	// пока вот решение
	if (m_model->m_skeleton)
	{
		bqMat4 pI = m_model->m_skeleton->m_preRotation;
		pI.Invert();
		bqVec3 sunPosRotated;
		bqMath::Mul(pI, material.m_sunPosition, sunPosRotated);
		material.m_sunPosition = sunPosRotated;
	}

	material.m_shaderType = bqShaderType::StandartSkinned;
	material.m_maps[0].m_texture = m_app->m_texture4x4;
	m_gs->SetMaterial(&material);
	m_gs->SetShader(material.m_shaderType, 0);

	if (m_model->m_skeletonForAnimation)
	{
		if (bqInput::IsKeyHit(bqInput::KEY_LEFT))
		{
			if (m_model->m_currSkeletonAnimationObject)
			{
				for (size_t i = 0; i < m_model->m_animationObjects.m_size; ++i)
				{
					auto ao = m_model->m_animationObjects.m_data[i];
					if (ao == m_model->m_currSkeletonAnimationObject)
					{
						if (i)
							m_model->m_currSkeletonAnimationObject = m_model->m_animationObjects.m_data[i-1];
						else
							m_model->m_currSkeletonAnimationObject = 0;
						
						break;
					}
				}
			}
		}

		if (bqInput::IsKeyHit(bqInput::KEY_RIGHT))
		{
			for (size_t i = 0; i < m_model->m_animationObjects.m_size; ++i)
			{
				auto ao = m_model->m_animationObjects.m_data[i];

				if (!m_model->m_currSkeletonAnimationObject)
				{
					m_model->m_currSkeletonAnimationObject = ao;
				}
				else if (ao == m_model->m_currSkeletonAnimationObject)
				{
					size_t next = i + 1;
					if (next < m_model->m_animationObjects.m_size)
					{
						m_model->m_currSkeletonAnimationObject = m_model->m_animationObjects.m_data[next];
						break;
					}
				}
			}
		}

		if (m_model->m_currSkeletonAnimationObject)
		{
			m_model->m_currSkeletonAnimationObject->AnimateInterpolate((*m_app->m_dt));
			m_model->m_skeletonForAnimation->Update();
		}

		bqSkeleton* s = m_model->m_skeletonForAnimation;
		auto& joints = s->GetJoints();
		if (joints.m_size)
		{
			for (size_t o = 0; o < joints.m_size; ++o)
			{
				bqFramework::GetMatrixSkinned()[o] = joints.m_data[o].m_data.m_matrixFinal;
			}
		}

		static float a = 0.f; a += 1.1f * (*m_app->m_dt); if (a > PIPI) a = 0.f;
		if (bqInput::IsKeyHit(bqInput::KEY_F1))
			a = 0.f;
		//m_skeletonForAnimation->GetJoints().m_data[2].m_data.m_transformation.m_base.m_rotation.Set(a, 0, 0);
		//m_skeletonForAnimation->GetJoints().m_data[2].m_data.m_transformation.CalculateMatrix();
	}

	for (size_t i = 0; i < m_model->m_meshBuffers.m_size; ++i)
	{
		m_gs->SetMesh(m_model->m_meshBuffers.m_data[i]->m_mesh);		
		material.m_maps[0].m_texture = m_model->m_meshBuffers.m_data[i]->m_texture;

		m_gs->Draw();
	}


	m_app->DrawGrid(14, (float)m_camera->m_position.y);

	if (m_model->m_skeletonForAnimation)
	{
		bqSkeleton* s = m_model->m_skeletonForAnimation;

		m_gs->SetShader(bqShaderType::Line3D, 0);
		m_gs->DisableDepth();
		auto joints = &s->GetJoints().m_data[0];
		if (s->GetJoints().m_size)
		{
			size_t jsz = s->GetJoints().m_size;
			for (size_t i = 0; i < jsz; ++i)
			{
				bqVec3 p;
				bqVec3 p2;
				float sz = 0.3f;
				p = bqVec3(sz, 0., 0.);

				bqJoint* joint = &joints[i];

				bqMat4 mI2 = joints[i].m_data.m_matrixBindInverse;
				mI2.Invert();

				bqMat4 mI = joints[i].m_data.m_matrixFinal * mI2;

				bqMath::Mul(mI, p, p2);
				m_gs->DrawLine3D(mI.m_data[3].GetVec3(), mI.m_data[3].GetVec3() + p2, bq::ColorRed);

				p = bqVec3(0., sz, 0.);
				bqMath::Mul(mI, p, p2);
				m_gs->DrawLine3D(mI.m_data[3].GetVec3(), mI.m_data[3].GetVec3() + p2, bq::ColorLime);

				p = bqVec3(0., 0., sz);
				bqMath::Mul(mI, p, p2);
				m_gs->DrawLine3D(mI.m_data[3].GetVec3(), mI.m_data[3].GetVec3() + p2, bq::ColorBlue);

				if (i > 0 && (joint->m_base.m_parentIndex != -1))
				{
					bqJoint* joint2 = &joints[joint->m_base.m_parentIndex];

					mI2 = joint2->m_data.m_matrixBindInverse;
					mI2.Invert();
					bqMat4 mI3 = joint2->m_data.m_matrixFinal * mI2;

					p = mI.m_data[3].GetVec3();
					p2 = mI3.m_data[3].GetVec3();
					m_gs->DrawLine3D(p, p2, bq::ColorBlue);
				}
			}
		}
		m_gs->EnableDepth();
	}

	m_gs->EndDraw();
	m_gs->SwapBuffers();
}

