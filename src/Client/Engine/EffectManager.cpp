
#include "pch.h"
#include "EffectManager.h"

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Effect.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "BillBoard.h"

void EffectManager::ReserveEffects(uint32 num)
{
	for (uint32 i = 0; i < num; ++i)
	{
		shared_ptr<CGameObject> object = std::make_shared<CGameObject>();

		// 위치 추가
		object->AddComponent(make_shared<Transform>());

		// 렌더러 추가
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();

		// 이펙트 설정
		shared_ptr<Effect> font = make_shared<Effect>();

		// 메쉬 생성
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);

		// 마테리얼 생성 Effect
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Effect");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		meshRenderer->SetMaterial(material);

		object->AddComponent(meshRenderer);
		object->AddComponent(font);

		// 객체 저장
		m_effectReserveObjects.push_back(object);
	}

	for (uint32 i = 0; i < num; ++i)
	{
		shared_ptr<CGameObject> object = std::make_shared<CGameObject>();

		// 위치 추가
		object->AddComponent(make_shared<Transform>());

		// 렌더러 추가
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();

		// 빌보드 설정
		shared_ptr<BillBoard> billboard = make_shared<BillBoard>();

		// 메쉬 생성
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
		meshRenderer->SetMesh(mesh);

		// 마테리얼 생성 billboard
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"BillBoard_Texture");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		meshRenderer->SetMaterial(material);

		object->AddComponent(meshRenderer);
		object->AddComponent(billboard);

		// 객체 저장
		m_billboardReserveObjects.push_back(object);
	}

	m_maxCount = num;
}

void EffectManager::PushEffectTexture(const std::wstring& effectName, vector<shared_ptr<Texture>> textures)
{
	m_effectTextures[effectName] = textures;
}

void EffectManager::Play(const std::wstring& effectName, Vec3 vPos, Vec3 vScale, Vec3 vRotation, float fAlpha, float fPassingTime)
{
	assert(m_effectCount < m_maxCount);

	// 출력 레이어 설정 -> 카메라
	m_effectReserveObjects[m_effectCount]->SetLayerIndex(m_sceneIndex);

	// 이펙트 텍스쳐 설정
	m_effectReserveObjects[m_effectCount]->GetEffect()->SetEffectInfo(m_effectTextures[effectName], fPassingTime);

	// 위치 설정
	std::shared_ptr<Transform> transform = m_effectReserveObjects[m_effectCount]->GetTransform();
	transform->SetLocalScale(vScale);
	transform->SetLocalRotation(vRotation);
	Matrix matWorld = Matrix::CreateTranslation(vPos);
	transform->SetWorldMatrix(matWorld);

	m_renderObjects.push_back(m_effectReserveObjects[m_effectCount]);

	++m_effectCount;
}

void EffectManager::PlayBillBoard(const std::wstring& effectName, Vec3 vPos, Vec3 vScale, float fAlpha, float fPassingTime)
{
	assert(m_billboardCount < m_maxCount);

	// 출력 레이어 설정 -> 카메라
	m_billboardReserveObjects[m_billboardCount]->SetLayerIndex(m_sceneIndex);

	// 이펙트 텍스쳐 설정
	m_billboardReserveObjects[m_billboardCount]->GetBillBoard()->SetBBInfo(BB_TYPE::ATLAS, m_effectTextures[effectName], fPassingTime);
	m_billboardReserveObjects[m_billboardCount]->GetBillBoard()->SetAlpha(fAlpha);

	// 위치 설정
	std::shared_ptr<Transform> transform = m_billboardReserveObjects[m_billboardCount]->GetTransform();
	transform->SetLocalScale(vScale);
	Matrix matWorld = Matrix::CreateTranslation(vPos);
	transform->SetWorldMatrix(matWorld);

	m_renderObjects.push_back(m_billboardReserveObjects[m_billboardCount]);

	++m_billboardCount;
}

void EffectManager::Reset(void)
{
	m_effectCount = 0;
	m_billboardCount = 0;
	m_renderObjects.clear();
}

