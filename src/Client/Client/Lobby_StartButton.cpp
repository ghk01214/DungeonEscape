#include "pch.h"
#include "Lobby_StartButton.h"

#include "MeshRenderer.h"
#include "Material.h"
#include "Input.h"

#include "SceneManager.h"
#include "Scene_Loading.h"

Lobby_StartButton::Lobby_StartButton() :
	m_lobbyEnd{ false }
{
}

Lobby_StartButton::~Lobby_StartButton()
{
}

void Lobby_StartButton::Awake()
{
	__super::Awake();
}

void Lobby_StartButton::Start()
{
	__super::Start();
}

void Lobby_StartButton::Update()
{
	if (m_lobbyEnd == true)
		GET_SINGLE(SceneManager)->LoadScene(Scene_Loading::Create(SCENE_GAMEPLAY));

	__super::Update();

	if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true)
			{
				for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
				{
					GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[START_BUTTON_PRESSED]);
				}

				m_click = true;
			}
			else if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
			{
				for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
				{
					GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[START_BUTTON]);
				}

				if (m_click == true)
					m_lobbyEnd = true;
			}
		}
	}
}

void Lobby_StartButton::LateUpdate()
{
	__super::LateUpdate();
}
