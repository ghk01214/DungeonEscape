#include "pch.h"
#include "FontManager.h"

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Font.h"
#include "Resources.h"
#include "MeshRenderer.h"
#include "SceneManager.h"

void FontManager::Reset(void)
{
	for (uint32 i = 0; i < m_objectCount; ++i)
	{
		std::shared_ptr<Font> font = m_fontReserveObjects[i]->GetFont();
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadFontMesh(font->GetTextVB("temp"));

		m_fontReserveObjects[i]->GetMeshRenderer()->SetMesh(mesh);
	}

	m_objectCount = 0;
	m_fontRenderObjects.clear();
}

void FontManager::RenderFonts(const wstring& text, Vec2 vPos, Vec2 vSize, float gap)
{
	assert(m_objectCount < m_maxCount);

	m_fontReserveObjects[m_objectCount]->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI"));

	// 폰트 크기 및 문장(텍스트) 설정
	std::shared_ptr<Font> font = m_fontReserveObjects[m_objectCount]->GetFont();
	font->SetFontGap(gap);

	std::string strText{ text.begin(), text.end() };
	shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadFontMesh(font->GetTextVB(strText));
	m_fontReserveObjects[m_objectCount]->GetMeshRenderer()->SetMesh(mesh);

	// 위치 설정
	std::shared_ptr<Transform> transform = m_fontReserveObjects[m_objectCount]->GetTransform();

	transform->SetLocalScale(Vec3(vSize.x, vSize.y, 1.f));

	Matrix matWorld = Matrix::CreateTranslation(Vec3(vPos.x, vPos.y, 1.f));
	transform->SetWorldMatrix(matWorld);

	// 렌더링 오브젝트에 추가
	m_fontRenderObjects.push_back(m_fontReserveObjects[m_objectCount]);

	++m_objectCount;
}

void FontManager::ReserveFontObjects(uint32 num)
{
	for (uint32 i = 0; i < num; ++i)
	{
		shared_ptr<CGameObject> object = std::make_shared<CGameObject>();

		// 위치 추가
		object->AddComponent(make_shared<Transform>());

		// 렌더러 추가
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();

		// 폰트 설정
		shared_ptr<Font> font = make_shared<Font>();

		// 메쉬 생성
		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadFontMesh(font->GetTextVB("temp"));	// 의미 없는 아무런 값.
		meshRenderer->SetMesh(mesh);

		// 마테리얼 생성
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Font");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"deathspirit", L"..\\Resources\\Font\\DeathSpirit.png");

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);

		object->AddComponent(meshRenderer);
		object->AddComponent(font);

		// 객체 저장
		m_fontReserveObjects.push_back(object);
	}

	m_maxCount = num;
}
