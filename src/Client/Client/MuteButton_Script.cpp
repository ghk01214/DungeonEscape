#include "pch.h"
#include "MuteButton_Script.h"

#include <Material.h>
#include <MeshRenderer.h>
#include <GameObject.h>
#include <UI.h>
#include <Input.h>
#include <SoundManager.h>

MuteButton_Script::MuteButton_Script(SOUND_TYPE type) :
	m_soundType{ type },
	m_mute{ false },
	m_prevMute{ m_mute }
{
}

MuteButton_Script::~MuteButton_Script()
{
}

void MuteButton_Script::Awake()
{
	__super::Awake();
}

void MuteButton_Script::Start()
{
	__super::Start();
}

void MuteButton_Script::Update()
{
	if (GetGameObject()->GetUI()->GetVisible() == false)
		return;

	__super::Update();

	if (m_click == true)
	{
		if (m_input->Button_Up(CInput::DIMB_LBUTTON) == true)
		{
			m_mute = !m_mute;
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
		else if (m_mute == false)
		{
			ChangeTexture(BUTTON);
		}
	}
	else if (m_mute == false)
	{
		ChangeTexture(BUTTON);
	}

	if (m_prevMute == m_mute)
		return;

	TEXTURE_TYPE type{ Mute() };

	ChangeTexture(type);
}

void MuteButton_Script::LateUpdate()
{
	__super::LateUpdate();
}

Button_Script::TEXTURE_TYPE MuteButton_Script::Mute()
{
	TEXTURE_TYPE type{};

	if (m_mute == true)
	{
		type = BUTTON_PRESSED;

		if (m_soundType == MuteButton_Script::SOUND_TYPE::BGM)
			GET_SINGLE(CSoundMgr)->MuteBGM(true);
		else if (m_soundType == MuteButton_Script::SOUND_TYPE::SE)
			GET_SINGLE(CSoundMgr)->MuteSE(true);
	}
	else
	{
		type = BUTTON;

		if (m_soundType == MuteButton_Script::SOUND_TYPE::BGM)
			GET_SINGLE(CSoundMgr)->MuteBGM(false);
		else if (m_soundType == MuteButton_Script::SOUND_TYPE::SE)
			GET_SINGLE(CSoundMgr)->MuteSE(false);
	}

	m_prevMute = m_mute;

	return type;
}
