#include "pch.h"
#include "Input.h"
#include "Engine.h"

void Input::Init(HWND hWnd)
{
	m_hWnd = hWnd;
	m_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);

	::GetCursorPos(&m_curMousePos);
	::ScreenToClient(GEngine->GetWindow().hWnd, &m_curMousePos);
	m_preMousePos = m_curMousePos;

	SetCursor(nullptr);

	m_useKeyType.clear();
	m_useKeyType.push_back(KEY_TYPE::SPACE);
	m_useKeyType.push_back(KEY_TYPE::W);
	m_useKeyType.push_back(KEY_TYPE::A);
	m_useKeyType.push_back(KEY_TYPE::S);
	m_useKeyType.push_back(KEY_TYPE::D);
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
	EncodeKeyInput();

	// 이전 프레임의 마우스 좌표 저장
	m_preMousePos = m_curMousePos;

	// 현재 프레임의 마우스 좌표 계산
	::GetCursorPos(&m_curMousePos);
	::ScreenToClient(GEngine->GetWindow().hWnd, &m_curMousePos);
}

void Input::EncodeKeyInput(void)
{
	/*
		각 키별로 2bit 씩 정보 전송
		KEY_STATE 는 총 4가지 ==> NONE, PRESS, DOWN, UP
		각각 누르지 않았다. 누르는 중이다. 눌렀다. 눌렀다 땠다.
		이를 00, 01, 10, 11로 표현, 각각 0, 1, 2, 3인 상황.

		2bit씩 for 문으로 bit를 전송

		1. 모든 사용하는 키 정보와 상태를 받아온다.
		2. 상태에 맞게 bit를 추가
	*/

	// 사용할 키 정보 설정

	// 인코딩
	m_keyInputState.reset();
	for (int i = 0; i < m_useKeyType.size(); ++i)
	{
		int temp = static_cast<int>(GetState(m_useKeyType[i]));

		temp <<= (i * 2);

		m_keyInputState |= temp;
	}

	std::cout << m_keyInputState.to_string() << std::endl;

	// 디코딩
	long long key = m_keyInputState.to_ulong();
	for (int i = 0; i < m_useKeyType.size(); ++i)
	{
		int temp = 3;

		temp <<= (i * 2);
		temp &= key;
		temp >>= (i * 2);

		switch (static_cast<KEY_STATE>(temp))
		{
			case KEY_STATE::NONE:
			if (i == 0)
				std::cout << "UP::None";
			if (i == 1)
				std::cout << "DOWN::None";
			if (i == 2)
				std::cout << "LEFT::None";
			if (i == 3)
				std::cout << "RIGHT::None";
			if (i == 4)
				std::cout << "SPACE::None";
			break;
			case KEY_STATE::PRESS:
			if (i == 0)
				std::cout << "UP::PRESS";
			if (i == 1)
				std::cout << "DOWN::PRESS";
			if (i == 2)
				std::cout << "LEFT::PRESS";
			if (i == 3)
				std::cout << "RIGHT::PRESS";
			if (i == 4)
				std::cout << "SPACE::PRESS";
			break;
			case KEY_STATE::DOWN:
			if (i == 0)
				std::cout << "UP::DOWN";
			if (i == 1)
				std::cout << "DOWN::DOWN";
			if (i == 2)
				std::cout << "LEFT::DOWN";
			if (i == 3)
				std::cout << "RIGHT::DOWN";
			if (i == 4)
				std::cout << "SPACE::DOWN";
			break;
			case KEY_STATE::UP:
			if (i == 0)
				std::cout << "UP::UP";
			if (i == 1)
				std::cout << "DOWN::UP";
			if (i == 2)
				std::cout << "LEFT::UP";
			if (i == 3)
				std::cout << "RIGHT::UP";
			if (i == 4)
				std::cout << "SPACE::UP";
			break;
			case KEY_STATE::END:
			break;
			default:
			break;
		}

		std::cout << std::endl;
	}
}

Vec2 Input::GetMouseMove(void)
{
	Vec2 move;
	move.x = m_curMousePos.x - m_preMousePos.x;
	move.y = m_curMousePos.y - m_curMousePos.y;

	return move;
}
