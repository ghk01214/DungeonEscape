#include "pch.h"
#include "LoginButton_Script.h"

#include <Input.h>
#include <SceneManager.h>

#include "Scene_Loading.h"

LoginButton_Script::LoginButton_Script(bool active)
{
	m_active = active;
}

LoginButton_Script::~LoginButton_Script()
{
}

void LoginButton_Script::Awake()
{
	__super::Awake();
}

void LoginButton_Script::Start()
{
	__super::Start();
}

void LoginButton_Script::Update()
{
	if (m_active == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_click = false;

			ChangeTexture(BUTTON);
			GET_SINGLE(SceneManager)->LoadScene(Scene_Loading::Create(SCENE_CHARACTER_SELECT));
		}
	}
	else if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true)
				m_click = true;

			ChangeTexture(BUTTON_PRESSED);
		}
		else
		{
			ChangeTexture(BUTTON);
		}
	}
	else
	{
		ChangeTexture(BUTTON);
	}
}

void LoginButton_Script::LateUpdate()
{
	__super::LateUpdate();
}
