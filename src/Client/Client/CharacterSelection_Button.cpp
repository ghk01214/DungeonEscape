#include "pch.h"
#include "CharacterSelection_Button.h"

#include "MeshRenderer.h"
#include "Material.h"
#include "Input.h"
#include "Timer.h"

#include "SceneManager.h"
#include "Scene_Loading.h"

CharacterSelection_Button::CharacterSelection_Button() :
	m_accTime{ 0.f },
	m_blinkCount{ 0 }
{
}

CharacterSelection_Button::~CharacterSelection_Button()
{
}

void CharacterSelection_Button::Awake()
{
	__super::Awake();
}

void CharacterSelection_Button::Start()
{
	__super::Start();
}

void CharacterSelection_Button::Update()
{
	if (m_blinkCount > 6)
	{
		FadeOutToShowLoadingScene();
	}

	__super::Update();

	m_accTime += DELTA_TIME;

	if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		//if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_OneClick(CInput::DIMB_LBUTTON) == true)
				m_click = true;
		}
	}

	BlinkCharacter();
}

void CharacterSelection_Button::LateUpdate()
{
	__super::LateUpdate();
}

void CharacterSelection_Button::BlinkCharacter()
{
	if (m_accTime > 0.1f)
	{
		m_accTime = 0.f;

		if (m_click == true and m_blinkCount < 7)
		{
			++m_blinkCount;

			TEXTURE_TYPE typeIndex;

			if (m_blinkCount % 2 == 0)
				typeIndex = BUTTON;
			else
				typeIndex = BUTTON_PRESSED;

			for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
			{
				GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[typeIndex]);
			}
		}
	}
}

void CharacterSelection_Button::FadeOutToShowLoadingScene()
{
	static float accTime{ 0.f };

	accTime += DELTA_TIME;

	if (accTime > 3.f)
	{
		auto nextScene{ std::dynamic_pointer_cast<Scene_Loading>(Scene_Loading::Create(SCENE_GAMEPLAY)) };

		if (m_name == L"Nana")
			nextScene->SetSelectedCharacter(server::FBX_TYPE::NANA);
		else if (m_name == L"Mistic")
			nextScene->SetSelectedCharacter(server::FBX_TYPE::MISTIC);
		else if (m_name == L"Carmel")
			nextScene->SetSelectedCharacter(server::FBX_TYPE::CARMEL);

		GET_SINGLE(SceneManager)->LoadScene(nextScene);
	}
	else
	{
		m_alpha = 1 - accTime / 3.f;

		if (m_alpha < 0.f)
			m_alpha = 0.f;
	}

	for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
	{
		GetMeshRenderer()->GetMaterial(i)->SetFloat(2, m_alpha);
	}
}

void CharacterSelection_Button::SetBlinkCount(int32_t count)
{
	m_blinkCount = count;
}
