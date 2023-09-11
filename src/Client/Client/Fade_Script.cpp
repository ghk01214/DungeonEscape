#include "pch.h"
#include "Fade_Script.h"

#include "Timer.h"

#include "SceneManager.h"
#include "Scene_Loading.h"

#include "MeshRenderer.h"
#include "Material.h"

#include "GameObject.h"

#include "Input.h"

Fade_Script::Fade_Script()
{
}

Fade_Script::~Fade_Script()
{
}

void Fade_Script::Awake()
{
}

void Fade_Script::Start()
{
	m_eState = Fade_State::Fade_In;
	m_accTime = 0.f;
	m_curTextureCount = 0;
}

void Fade_Script::Update()
{
	if (m_bFadeIn)
	{
		m_bActivate = true;
		m_eState = Fade_State::Fade_In;
		m_accTime = 0.f;
		m_curTextureCount = 0;
		m_bLogoEnd = false;
		m_bFadeIn = false;
	}

	if (m_bActivate)
	{
		if (m_bLogoEnd == true)
		{
			m_bActivate = false;
		}

		m_accTime += DELTA_TIME;

		// fade에 걸리는 시간관련 자동 상태변경
		switch (m_eState)
		{
			case Fade_Script::Fade_State::Fade_In:
			if (m_fadeInDuration < m_accTime)
			{
				m_eState = Fade_State::Contents;
				m_accTime = 0.f;
				*m_fadeIn = false;
			}
			else
			{
				m_fAlpha = m_accTime / m_fadeInDuration;

				if (m_fAlpha > 1)
					m_fAlpha = 1;
			}
			break;

			case Fade_Script::Fade_State::Contents:
			//if (m_contentsDuration < m_accTime)
			//{
			//	m_eState = Fade_State::Fade_Out;
			//	m_accTime = 0.f;
			//}

			if (m_bFadeOut)
			{
				m_eState = Fade_State::Fade_Out;
				m_accTime = 0.f;
				m_bFading = false;
			}
			else
				m_bFading = true;

			break;

			case Fade_Script::Fade_State::Fade_Out:
			if (m_fadeOutDuration < m_accTime)
			{
				if (m_curTextureCount < (m_textures.size() - 1))
				{
					++m_curTextureCount;
					m_eState = Fade_State::Fade_In;
				}
				else
					m_bLogoEnd = true;

				*m_fadeOut = false;
			}
			else
			{
				m_fAlpha = 1 - m_accTime / m_fadeOutDuration;

				if (m_fAlpha < 0)
					m_fAlpha = 0;
			}
			break;

			default:
			break;
		}

		uint32 size = GetMeshRenderer()->GetMaterialSize();
		for (uint32 i = 0; i < size; ++i)
		{
			GetMeshRenderer()->GetMaterial(i)->SetFloat(2, m_fAlpha);
			GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[m_curTextureCount]);
		}
	}
}

void Fade_Script::LateUpdate()
{
}

void Fade_Script::SetLogoInfo(float fadeInDuration, float fadeOutDuration, vector<shared_ptr<Texture>> textures)
{
	m_fadeInDuration = fadeInDuration;	// Fade In에 걸리는 시간
	m_fadeOutDuration = fadeOutDuration;	// Fade Out에 걸리는 시간
	//m_contentsDuration = contentsDuration;	// 내용을 보여주는데 걸리는 시간
	m_textures = textures;
}

void Fade_Script::SetFade(std::shared_ptr<bool> in, std::shared_ptr<bool> out)
{
	m_fadeIn = in;
	m_fadeOut = out;
}
