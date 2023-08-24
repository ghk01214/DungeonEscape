#include "pch.h"
#include "DescriptionButton_Script.h"

#include <MeshRenderer.h>
#include <Material.h>
#include <GameObject.h>
#include <UI.h>
#include <Input.h>
#include <Timer.h>
#include <Transform.h>
#include <SceneManager.h>

#include "Scene_Loading.h"

DescriptionButton_Script::DescriptionButton_Script() :
	m_select{ false }
{
	m_active = true;
}

DescriptionButton_Script::~DescriptionButton_Script()
{
}

void DescriptionButton_Script::Awake()
{
	__super::Awake();
}

void DescriptionButton_Script::Start()
{
	__super::Start();
}

void DescriptionButton_Script::Update()
{
	if (m_active == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_click = false;
			m_select = false;

			ChangeTexture(BUTTON);
		}
	}
	else if (m_pos.x - (m_scale.x / 2) <= m_mousePos.x and m_mousePos.x <= m_pos.x + (m_scale.x / 2))
	{
		if (m_pos.y - (m_scale.y / 2) <= m_mousePos.y and m_mousePos.y <= m_pos.y + (m_scale.y / 2))
		{
			if (m_input->Button_Down(CInput::DIMB_LBUTTON) == true)
			{
				m_click = true;
				m_select = true;
			}

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

void DescriptionButton_Script::LateUpdate()
{
	__super::LateUpdate();
}

void DescriptionButton_Script::SetCharacterType(server::FBX_TYPE type)
{
	m_character = type;
}

void DescriptionButton_Script::SetCharacterType(int32_t index)
{
	m_character = magic_enum::enum_value<server::FBX_TYPE>(index);
}

void DescriptionButton_Script::SetSelect(bool flag)
{
	m_select = flag;
}
