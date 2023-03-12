#include "pch.h"
#include "Input.h"
#include "Engine.h"

void Input::Init(HWND hWnd)
{
	m_hWnd = hWnd;
	m_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update()
{
	HWND hWnd = ::GetActiveWindow();
	if (m_hWnd != hWnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
			m_states[key] = KEY_STATE::NONE;

		return;
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// Ű�� ���� ������ true
		if (asciiKeys[key] & 0x80)
		{
			KEY_STATE& state = m_states[key];

			// ���� �����ӿ� Ű�� ���� ���¶�� PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = m_states[key];

			// ���� �����ӿ� Ű�� ���� ���¶�� UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	::GetCursorPos(&m_mousePos);
	::ScreenToClient(GEngine->GetWindow().hWnd, &m_mousePos);
}