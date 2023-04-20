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

	// winapi가 제공하는 모든 키(VirtualKey) 정보에 대해 입력을 받을 준비
	BYTE asciiKeys[KEY_TYPE_COUNT] = {};

	// 256개의 virtual key에 대한 배열 정보를 받아와 완료되면 true, 아니면 false 반환
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

	// 모든 키 정보들에 대해 확인해줄 것
	ChangeKeyInput();

	::GetCursorPos(&m_mousePos);
	::ScreenToClient(GEngine->GetWindow().hWnd, &m_mousePos);
}

void Input::ChangeKeyInput(void)
{
	/*
		현재 사용되고 있는 키 정보들

		NONE = 0,

		UP = VK_UP,
		DOWN = VK_DOWN,
		LEFT = VK_LEFT,
		RIGHT = VK_RIGHT,
		SPACE = VK_SPACE,

		W = 'W',
		A = 'A',
		S = 'S',
		D = 'D',

		Q = 'Q',
		E = 'E',
		Z = 'Z',
		C = 'C',

		// 여기 있는 번호 키는 키보드 영/한 자판 위에 있는 번호 1 ~ 0 사이의 버튼, 오른쪽에 있는 것은 NUMPAD 숫자.
		KEY_1 = '1',
		KEY_2 = '2',
		KEY_3 = '3',
		KEY_4 = '4',

		// 마우스 왼쪽 오른쪽 버튼
		LBUTTON = VK_LBUTTON,
		RBUTTON = VK_RBUTTON,

		MAX
	*/

	// 모든 비트 0으로 초기화
	m_keyInput.reset();

	// KEY_STATE::PRESS 정보만 전송 가능
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::UP)] = GetButton(KEY_TYPE::UP);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::DOWN)] = GetButton(KEY_TYPE::DOWN);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::LEFT)] = GetButton(KEY_TYPE::LEFT);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::RIGHT)] = GetButton(KEY_TYPE::RIGHT);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::SPACE)] = GetButton(KEY_TYPE::SPACE);

	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::W)] = GetButton(KEY_TYPE::W);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::A)] = GetButton(KEY_TYPE::A);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::S)] = GetButton(KEY_TYPE::S);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::D)] = GetButton(KEY_TYPE::D);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::Q)] = GetButton(KEY_TYPE::Q);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::E)] = GetButton(KEY_TYPE::E);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::Z)] = GetButton(KEY_TYPE::Z);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::C)] = GetButton(KEY_TYPE::C);

	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::KEY_1)] = GetButton(KEY_TYPE::KEY_1);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::KEY_2)] = GetButton(KEY_TYPE::KEY_2);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::KEY_3)] = GetButton(KEY_TYPE::KEY_3);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::KEY_4)] = GetButton(KEY_TYPE::KEY_4);

	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::LBUTTON)] = GetButton(KEY_TYPE::LBUTTON);
	m_keyInput[static_cast<uint32>(BITSET_KEY_TYPE::RBUTTON)] = GetButton(KEY_TYPE::RBUTTON);
}