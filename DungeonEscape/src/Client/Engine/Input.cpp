﻿#include "pch.h"
#include "Input.h"


void CInput::Init(HWND hwnd)
{
	m_hwnd = hwnd;
	m_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void CInput::Update()
{
	HWND hwnd = ::GetActiveWindow();
	if (m_hwnd != hwnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
			m_states[key] = KEY_STATE::NONE;

		return;
	}

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// 키가 눌려 있으면 true
		if (::GetAsyncKeyState(key) & 0x8000)
		{
			KEY_STATE& state = m_states[key];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = m_states[key];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}
}