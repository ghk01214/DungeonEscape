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
	// 이전 프레임의 텍스쳐가 최대 숫자일 경우
	if (m_curTextureNumber == (m_textures.size() - 1))
		// 한번만 플레이 정보를 true로 한다.
		m_bPlayOnce = true;

	// 한번만 재생 정보가 true일 경우
	if (m_bPlayOnce)
	{
		// 지속 재생을 false로, 텍스쳐 정보를 0으로 세팅 후 멈춘다.
		// 외부에서 m_bPlayOnce 정보를 false로 바꾸면 m_bContinuePlay가 true가 되고 이것이 그려진다. m_bContinuePlay의 감지는 이펙트 매니저에서 처리한다. 해당 변수가 true면 화면에 그려진다.
		m_bContinuePlay = false;
		m_curTextureNumber = 0;
		return;
	}
	else
		m_bContinuePlay = true;

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

void Effect::SetPlayOnce(bool bPlayOnce)
{
	m_bPlayOnce = bPlayOnce;
	m_bContinuePlay = true;
}
