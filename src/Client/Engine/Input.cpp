#include "pch.h"
#include "Input.h"
#include "Engine.h"

HRESULT CInput::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;
	m_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);

	::GetCursorPos(&m_curMousePos);
	::ScreenToClient(GEngine->GetWindow().hWnd, &m_curMousePos);
	m_preMousePos = m_curMousePos;

	SetCursor(nullptr);

	m_useKeyType.clear();
	m_useKeyType.push_back(KEY_TYPE::W);
	m_useKeyType.push_back(KEY_TYPE::S);
	m_useKeyType.push_back(KEY_TYPE::A);
	m_useKeyType.push_back(KEY_TYPE::D);
	m_useKeyType.push_back(KEY_TYPE::R);
	m_useKeyType.push_back(KEY_TYPE::SPACE);
	m_useKeyType.push_back(KEY_TYPE::KEY_1);
	m_useKeyType.push_back(KEY_TYPE::KEY_2);
	m_useKeyType.push_back(KEY_TYPE::KEY_3);
	m_useKeyType.push_back(KEY_TYPE::KEY_4);

	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();
}

void CInput::SetUp_InputDeviceState()
{
	if (nullptr == m_pMouse)
		return;

	ZeroMemory(&m_MouseState, sizeof(m_MouseState));

	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
}

void CInput::Update()
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
		KEY_STATE& state = m_states[key];

		// 키가 눌려 있으면 true
		if (asciiKeys[key] & 0x80)
		{
			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	// 현재 프레임의 입력 정보 세팅
	SetUp_InputDeviceState();

	// 모든 키 정보들에 대해 확인해줄 것
	EncodeKeyInput();

	// 이전 프레임의 마우스 좌표 저장
	m_preMousePos = m_curMousePos;

	// 현재 프레임의 마우스 좌표 계산
	::GetCursorPos(&m_curMousePos);
	::ScreenToClient(GEngine->GetWindow().hWnd, &m_curMousePos);
}

void CInput::EncodeKeyInput(void)
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
		int temp = static_cast<int32>(GetState(m_useKeyType[i]));

		temp <<= (i * 2);

		m_keyInputState |= temp;
	}
}

Vec2 CInput::GetMouseMove(void)
{
	Vec2 move;
	move.x = m_curMousePos.x - m_preMousePos.x;
	move.y = m_curMousePos.y - m_curMousePos.y;

	return move;
}

bool CInput::Button_Pressing(MOUSEBUTTONSTATE eDIMBState)
{
	// 예외처리
	if (eDIMBState < DIMB_LBUTTON || eDIMBState >= DIMB_END)
		return false;

	if (Get_DIMButtonState(eDIMBState))
		return true;

	return false;
}

bool CInput::Button_Down(MOUSEBUTTONSTATE eDIMBState)
{
	// 예외처리
	if (eDIMBState < DIMB_LBUTTON || eDIMBState >= DIMB_END)
		return false;

	// 이전에 눌림이 없고, 현재 눌림이 있는 경우
	if (!m_bButtonState[eDIMBState] && (Get_DIMButtonState(eDIMBState) & 0x8000))
	{
		m_bButtonState[eDIMBState] = !m_bButtonState[eDIMBState];
		return true;
	}

	if (m_bButtonState[eDIMBState] && !(Get_DIMButtonState(eDIMBState) & 0x8000))
		m_bButtonState[eDIMBState] = !m_bButtonState[eDIMBState];

	return false;
}

bool CInput::Button_Up(MOUSEBUTTONSTATE eDIMBState)
{
	// 예외처리
	if (eDIMBState < DIMB_LBUTTON || eDIMBState >= DIMB_END)
		return false;

	// 이전에 눌림이 있고, 현재 눌리지 않았을 경우
	if (m_bButtonState[eDIMBState] && !(Get_DIMButtonState(eDIMBState) & 0x8000))
	{
		m_bButtonState[eDIMBState] = !m_bButtonState[eDIMBState];
		return true;
	}

	if (!m_bButtonState[eDIMBState] && (Get_DIMButtonState(eDIMBState) & 0x8000))
		m_bButtonState[eDIMBState] = !m_bButtonState[eDIMBState];

	return false;
}