#pragma once

#include <bitset>
#include <dinput.h>

enum class KEY_TYPE
{
	UP = DIK_UP,
	DOWN = DIK_DOWN,
	LEFT = DIK_LEFT,
	RIGHT = DIK_RIGHT,
	SPACE = DIK_SPACE,

	W = DIK_W,
	A = DIK_A,
	S = DIK_S,
	D = DIK_D,

	Q = DIK_Q,
	E = DIK_E,
	Z = DIK_Z,
	C = DIK_C,
	R = DIK_R,

	// 여기 있는 번호 키는 키보드 영/한 자판 위에 있는 번호 1 ~ 0 사이의 버튼, 오른쪽에 있는 것은 NUMPAD 숫자.
	KEY_1 = DIK_1,
	KEY_2 = DIK_2,
	KEY_3 = DIK_3,
	KEY_4 = DIK_4,

	MAX = 18
};

enum class KEY_STATE
{
	NONE,
	DOWN,
	PRESS,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
	KEY_USE_COUNT = static_cast<int32_t>(KEY_TYPE::MAX),
	KEY_STATE_USE = static_cast<int32_t>(KEY_TYPE::MAX) * 2
};

class CInput
{
	DECLARE_SINGLE(CInput);

public:
	enum MOUSEBUTTONSTATE { DIMB_LBUTTON, DIMB_RBUTTON, DIMB_WHEEL, DIMB_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

public:
	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void SetUp_InputDeviceState();
	void Update();




public:
	// 누르고 있을 때
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

	const POINT& GetMousePos() { return m_curMousePos; }
	const unsigned long GetKeyInput() const { return m_keyInputState.to_ulong(); }
	const bool IsNoInput() const { return m_keyInputState.none(); }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return m_states[static_cast<uint8>(key)]; }
	void EncodeKeyInput(void);






public:
	Vec2 GetMouseMove(void);	// 이전 프레임과 현재 프레임의 마우스의 x,y 좌표의 움직이는 정도를 반환하는 함수

public:
	signed char Get_DIMButtonState(MOUSEBUTTONSTATE eDIMBState) { return m_MouseState.rgbButtons[eDIMBState];}
	signed long Get_DIMMoveState(MOUSEMOVESTATE eDIMMState) { return ((signed long*)&m_MouseState)[eDIMMState]; }

public:
	bool      Button_Pressing(MOUSEBUTTONSTATE eDIMBState);
	bool      Button_Down(MOUSEBUTTONSTATE eDIMBState);
	bool      Button_Up(MOUSEBUTTONSTATE eDIMBState);
	bool      Button_OneClick(MOUSEBUTTONSTATE eDIMBState);




private:
	HWND m_hWnd;
	vector<KEY_STATE> m_states;
	POINT m_curMousePos = POINT{ 0, 0 };
	POINT m_preMousePos = POINT{ 0, 0 };

	std::bitset<KEY_STATE_USE> m_keyInputState;

	vector<KEY_TYPE> m_useKeyType;

	LPDIRECTINPUT8			m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	DIMOUSESTATE		m_MouseState;
	bool				m_bButtonState[4];
};

