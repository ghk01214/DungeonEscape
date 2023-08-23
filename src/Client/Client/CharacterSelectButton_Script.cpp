#include "pch.h"
#include "CharacterSelectButton_Script.h"

#include <Input.h>
#include <Timer.h>
#include <Transform.h>
#include <GameObject.h>
#include <UI.h>

#include "Scene_Loading.h"

CharacterSelectButton_Script::CharacterSelectButton_Script(server::FBX_TYPE character) :
	m_character{ character },
	m_select{ false },
	m_prevSelect{ m_select }
{
}

CharacterSelectButton_Script::~CharacterSelectButton_Script()
{
}

void CharacterSelectButton_Script::Awake()
{
	__super::Awake();
}

void CharacterSelectButton_Script::Start()
{
	__super::Start();
}

void CharacterSelectButton_Script::Update()
{
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_select = !m_select;
			m_click = false;

			ChangeTexture(BUTTON);
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
		else if (m_select == false)
		{
			ChangeTexture(BUTTON);
		}
	}
	else if (m_select == false)
	{
		ChangeTexture(BUTTON);
	}

	if (m_prevSelect == m_select)
		return;

	TEXTURE_TYPE type{ Select() };

	ChangeTexture(type);
}

void CharacterSelectButton_Script::LateUpdate()
{
	__super::LateUpdate();
}

Button_Script::TEXTURE_TYPE CharacterSelectButton_Script::Select()
{
	m_prevSelect = m_select;

	if (m_select == true)
		return BUTTON_PRESSED;

	return BUTTON;
}

void CharacterSelectButton_Script::SetCharacterType(server::FBX_TYPE type)
{
	m_character = type;
}

void CharacterSelectButton_Script::SetCharacterType(int32_t index)
{
	m_character = magic_enum::enum_value<server::FBX_TYPE>(index);
}
