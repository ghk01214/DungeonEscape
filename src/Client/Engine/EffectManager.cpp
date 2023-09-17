
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

uint32 EffectManager::CreateEffect(const std::wstring& effectName, float passingTime)
{
	assert(!m_effectTextures[effectName].empty());

	m_effectReserveObjects[m_effectCount]->GetEffect()->SetEffectInfo(m_effectTextures[effectName], passingTime);

	return m_effectCount++;
}

uint32 EffectManager::CreateBillBoard(const std::wstring& effectName, float passingTime)
{
	assert(!m_effectTextures[effectName].empty());

	m_billboardReserveObjects[m_billboardCount]->GetBillBoard()->SetBBInfo(BB_TYPE::ATLAS, m_effectTextures[effectName], passingTime);

	return m_billboardCount++;
}

//void EffectManager::Play(const std::wstring& effectName, Vec3 vPos, Vec3 vScale, Vec3 vRotation, float fAlpha, float fPassingTime)
//{
//	assert(m_effectCount < m_maxCount);
//
//	// 출력 레이어 설정 -> 카메라
//	m_effectReserveObjects[m_effectCount]->SetLayerIndex(m_sceneIndex);
//
//	// 이펙트 텍스쳐 설정
//	m_effectReserveObjects[m_effectCount]->GetEffect()->SetEffectInfo(m_effectTextures[effectName], fPassingTime);
//
//	// 위치 설정
//	std::shared_ptr<Transform> transform = m_effectReserveObjects[m_effectCount]->GetTransform();
//	transform->SetLocalScale(vScale);
//	transform->SetLocalRotation(vRotation);
//	Matrix matWorld = Matrix::CreateTranslation(vPos);
//	transform->SetWorldMatrix(matWorld);
//
//	m_renderObjects.push_back(m_effectReserveObjects[m_effectCount]);
//
//	++m_effectCount;
//}
//
//void EffectManager::PlayBillBoard(const std::wstring& effectName, Vec3 vPos, Vec3 vScale, float fAlpha, float fPassingTime)
//{
//	assert(m_billboardCount < m_maxCount);
//
//	// 출력 레이어 설정 -> 카메라
//	m_billboardReserveObjects[m_billboardCount]->SetLayerIndex(m_sceneIndex);
//
//	// 이펙트 텍스쳐 설정
//	m_billboardReserveObjects[m_billboardCount]->GetBillBoard()->SetBBInfo(BB_TYPE::ATLAS, m_effectTextures[effectName], fPassingTime);
//	m_billboardReserveObjects[m_billboardCount]->GetBillBoard()->SetAlpha(fAlpha);
//
//	// 위치 설정
//	std::shared_ptr<Transform> transform = m_billboardReserveObjects[m_billboardCount]->GetTransform();
//	transform->SetLocalScale(vScale);
//	Matrix matWorld = Matrix::CreateTranslation(vPos);
//	transform->SetWorldMatrix(matWorld);
//
//	m_renderObjects.push_back(m_billboardReserveObjects[m_billboardCount]);
//
//	++m_billboardCount;
//}

void EffectManager::Play(uint32 iEffectNumber)
{
	m_effectReserveObjects[iEffectNumber]->GetEffect()->SetPlayOnce(false);
}

void EffectManager::PlayBillBoard(uint32 iEffectNumber)
{
	m_billboardReserveObjects[iEffectNumber]->GetBillBoard()->SetPlayOnce(false);
}

void EffectManager::Stop(uint32_t iEffectNumber)
{
	m_effectReserveObjects[iEffectNumber]->GetEffect()->SetPlayOnce(true);
}

void EffectManager::StopBillBoard(uint32_t iEffectNumber)
{
	m_billboardReserveObjects[iEffectNumber]->GetBillBoard()->SetPlayOnce(true);
}

void EffectManager::SetEffectInfo(uint32 iEffectNumber, Vec3 vPos, Vec3 vScale, Vec3 vRotation, float fPassingTime, float fAlpha)
{
	// 출력 레이어 설정 -> 카메라
	m_effectReserveObjects[iEffectNumber]->SetLayerIndex(m_sceneIndex);

	// 이펙트 텍스쳐 넘어가는 시간 설정
	m_effectReserveObjects[iEffectNumber]->GetEffect()->SetAlpha(fAlpha);
	m_effectReserveObjects[iEffectNumber]->GetEffect()->SetPassingTime(fPassingTime);

	// 위치 설정
	std::shared_ptr<Transform> transform = m_effectReserveObjects[iEffectNumber]->GetTransform();
	transform->SetLocalScale(vScale);
	transform->SetLocalRotation(vRotation);
	Matrix matWorld = Matrix::CreateTranslation(vPos);
	transform->SetWorldMatrix(matWorld);
}

void EffectManager::SetBillBoardInfo(uint32 iEffectNumber, Vec3 vPos, Vec3 vScale, float fPassingTime, float fAlpha)
{
	// 출력 레이어 설정 -> 카메라
	m_billboardReserveObjects[iEffectNumber]->SetLayerIndex(m_sceneIndex);

	// 이펙트 텍스쳐 넘어가는 시간 설정
	m_billboardReserveObjects[iEffectNumber]->GetBillBoard()->SetPassingTime(fPassingTime);
	m_billboardReserveObjects[iEffectNumber]->GetBillBoard()->SetAlpha(fAlpha);

	// 위치 설정
	std::shared_ptr<Transform> transform = m_billboardReserveObjects[iEffectNumber]->GetTransform();
	transform->SetLocalScale(vScale);
	Matrix matWorld = Matrix::CreateTranslation(vPos);
	transform->SetWorldMatrix(matWorld);
}

void EffectManager::Reset(void)
{
	m_renderObjects.clear();
}

void EffectManager::Push(void)
{
	for (uint32 i = 0; i < m_effectCount; ++i)
	{
		if (m_effectReserveObjects[i]->GetEffect()->GetContinue())
			m_renderObjects.push_back(m_effectReserveObjects[i]);
	}

	for (uint32 i = 0; i < m_billboardCount; ++i)
	{
		if (m_billboardReserveObjects[i]->GetBillBoard()->GetContinue())
			m_renderObjects.push_back(m_billboardReserveObjects[i]);
	}
}

const bool EffectManager::GetPlayOnce(uint32_t index) const
{
	return m_effectReserveObjects[index]->GetEffect()->GetPlayOnce();
}

const bool EffectManager::GetBillboardPlayOnce(uint32_t index) const
{
	return m_billboardReserveObjects[index]->GetBillBoard()->GetPlayOnce();
}

