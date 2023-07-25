#include "pch.h"
#include "Lobby_StartButton.h"

#include "Timer.h"

#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"
#include "Input.h"
#include "Engine.h"

#include "Scene_Loading.h"

Lobby_StartButton::Lobby_StartButton() :
	m_click{ false },
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

	m_pos = GetTransform()->GetLocalPosition();
	m_scale = GetTransform()->GetLocalScale();
}

void Lobby_StartButton::Update()
{
	if (m_lobbyEnd == true)
		GET_SINGLE(SceneManager)->LoadScene(Scene_Loading::Create(SCENE_GAMEPLAY));

	__super::Update();

	auto input{ GET_SINGLE(CInput)->GetInstance() };
	auto mousePos{ input->GetMousePos() };

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	mousePos.x -= width / 2;
	mousePos.y -= height / 2;
	mousePos.y = -mousePos.y;

	if (m_pos.x - (m_scale.x / 2) <= mousePos.x and mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= mousePos.y and mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (input->Button_Down(CInput::DIMB_LBUTTON) == true)
			{
				for (int32_t i = 0; i < GetMeshRenderer()->GetMaterialSize(); ++i)
				{
					GetMeshRenderer()->GetMaterial(i)->SetTexture(0, m_textures[START_BUTTON_PRESSED]);
				}

				m_click = true;
			}
			else if (input->Button_Up(CInput::DIMB_LBUTTON) == true)
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
