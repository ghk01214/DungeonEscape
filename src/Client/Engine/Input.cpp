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
		m_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);

		return;
	}

	BYTE asciiKeys[KEY_TYPE_COUNT] = {};
	if (::GetKeyboardState(asciiKeys) == false)
		return;

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// 키가 눌려 있으면 true
		if (asciiKeys[key] & 0x80)
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

	// 모든 비트 0으로 초기화
	m_keyInput.reset();

	for (int32_t i = 0; i < KEY_USE_COUNT; ++i)
	{
		if (GetState(static_cast<KEY_TYPE>(i)) == KEY_STATE::PRESS)
		{
			m_keyInput[i] = true;
			std::cout << i << ", ";
		}
	}

	//std::cout << std::endl;

	::GetCursorPos(&m_mousePos);
	::ScreenToClient(GEngine->GetWindow().hWnd, &m_mousePos);
}