#include "pch.h"
#include "Effect.h"

#include "Resources.h"
#include "MeshRenderer.h"
#include "Timer.h"
#include "Transform.h"
#include "GameObject.h"

#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"

Effect::Effect() : Component(COMPONENT_TYPE::Effect)
{
}

Effect::~Effect()
{
}

void Effect::Awake()
{
}

void Effect::Start()
{
}

void Effect::Update()
{
	// 시간을 누적
	m_fAccumulateTime += DELTA_TIME;

	shared_ptr<MeshRenderer> meshRenderer = GetMeshRenderer();

	// 메쉬 렌더러가 존재할 경우
	if (nullptr != meshRenderer)
	{
		// 누적 시간이 넘어가는 시간보다 많아질 경우
		if (m_fAccumulateTime > m_fPassingTime)
		{
			m_curTextureNumber = (m_curTextureNumber + 1) % m_textures.size();

			// 텍스쳐를 바꾼다.
			uint32 size = meshRenderer->GetMaterialSize();
			for (uint32 i = 0; i < size; ++i)
			{
				meshRenderer->GetMaterial(i)->SetFloat(2, m_fAlpha);
				meshRenderer->GetMaterial(i)->SetTexture(0, m_textures[m_curTextureNumber]);
			}
		}

		// 누적 시간 초기화
		m_fAccumulateTime = 0.f;
	}
}

void Effect::LateUpdate()
{
}

void Effect::FinalUpdate()
{
}

void Effect::SetEffectInfo(std::vector<shared_ptr<class Texture>> textures, float m_fTime)
{
	// 텍스쳐 설정
	m_textures = textures;

	// 텍스쳐 넘기는 시간 설정 
	m_fPassingTime = m_fTime;
}
