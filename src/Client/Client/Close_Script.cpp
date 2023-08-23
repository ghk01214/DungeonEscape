#include "pch.h"
#include "Close_Script.h"

#include <Material.h>
#include <MeshRenderer.h>
#include <GameObject.h>
#include <UI.h>
#include <Input.h>

Close_Script::Close_Script() :
	m_closePopUp{ false }
{
}

Close_Script::~Close_Script()
{
}

void Close_Script::Awake()
{
	__super::Awake();
}

void Close_Script::Start()
{
	__super::Start();
}

void Close_Script::Update()
{
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_click = false;
			m_closePopUp = true;

			ChangeTexture(BUTTON);
		}
	}
	else if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true and m_click == false)
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

void Close_Script::LateUpdate()
{
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	__super::LateUpdate();
}

void Close_Script::SetClosePopUpFlag(bool flag)
{
	m_closePopUp = flag;
}
