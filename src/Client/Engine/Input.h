#pragma once

#include <bitset>
#include <dinput.h>

enum class KEY_TYPE
{
	ESC = DIK_ESCAPE,

	NUM_1 = DIK_1,
	NUM_2 = DIK_2,
	NUM_3 = DIK_3,
	NUM_4 = DIK_4,
	NUM_5 = DIK_5,
	NUM_6 = DIK_6,
	NUM_7 = DIK_7,
	NUM_8 = DIK_8,
	NUM_9 = DIK_9,
	NUM_0 = DIK_0,
	MINUS = DIK_MINUS,
	EQUALS = DIK_EQUALS,
	BACKSPACE = DIK_BACK,

	TAB = DIK_TAB,
	Q = DIK_Q,
	W = DIK_W,
	E = DIK_E,
	R = DIK_R,
	T = DIK_T,
	Y = DIK_Y,
	U = DIK_U,
	I = DIK_I,
	O = DIK_O,
	P = DIK_P,
	LBRACKET = DIK_LBRACKET,
	RBRACKET = DIK_RBRACKET,
	BACKSLASH = DIK_BACKSLASH,

	CAPSLOCK = DIK_CAPITAL,
	A = DIK_A,
	S = DIK_S,
	D = DIK_D,
	F = DIK_F,
	G = DIK_G,
	H = DIK_H,
	J = DIK_J,
	K = DIK_K,
	L = DIK_L,
	SEMICOLON = DIK_SEMICOLON,
	APOSTROPHE = DIK_APOSTROPHE,
	ENTER = DIK_RETURN,

	LSHIFT = DIK_LSHIFT,
	Z = DIK_Z,
	X = DIK_X,
	C = DIK_C,
	V = DIK_V,
	B = DIK_B,
	N = DIK_N,
	M = DIK_M,
	COMMA = DIK_COMMA,
	PERIOD = DIK_PERIOD,
	SLASH = DIK_SLASH,
	RSHIFT = DIK_RSHIFT,

	LCONTROL = DIK_LCONTROL,
	LALT = DIK_LMENU,		// LEFT ALT
	SPACE = DIK_SPACE,
	RALT = DIK_RMENU,

	NUMLOCK = DIK_NUMLOCK,
	NUMPAD_DIVIDE = DIK_DIVIDE,
	NUMPAD_MULTIPLY = DIK_MULTIPLY,
	NUMPAD_MINUS = DIK_SUBTRACT,
	NUMPAD_ADD = DIK_ADD,
	NUMPAD_1 = DIK_NUMPAD1,
	NUMPAD_2 = DIK_NUMPAD2,
	NUMPAD_3 = DIK_NUMPAD3,
	NUMPAD_4 = DIK_NUMPAD4,
	NUMPAD_5 = DIK_NUMPAD5,
	NUMPAD_6 = DIK_NUMPAD6,
	NUMPAD_7 = DIK_NUMPAD7,
	NUMPAD_8 = DIK_NUMPAD8,
	NUMPAD_9 = DIK_NUMPAD9,
	NUMPAD_0 = DIK_NUMPAD0,
	NUMPAD_DECIMAL = DIK_DECIMAL,
	NUMPAD_ENTER = DIK_NUMPADENTER,

	INSERT = DIK_INSERT,
	DEL = DIK_DELETE,
	HOME = DIK_HOME,
	END = DIK_END,

	UP = DIK_UP,
	DOWN = DIK_DOWN,
	LEFT = DIK_LEFT,
	RIGHT = DIK_RIGHT,

	MAX = 81
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
	signed char Get_DIMButtonState(MOUSEBUTTONSTATE eDIMBState) { return m_MouseState.rgbButtons[eDIMBState]; }
	signed long Get_DIMMoveState(MOUSEMOVESTATE eDIMMState) { return ((signed long*)&m_MouseState)[eDIMMState]; }

public:
	bool      Button_Pressing(MOUSEBUTTONSTATE eDIMBState);
	bool      Button_Down(MOUSEBUTTONSTATE eDIMBState);
	bool      Button_Up(MOUSEBUTTONSTATE eDIMBState);
	bool      Button_OneClick(MOUSEBUTTONSTATE eDIMBState);

public:
	void StartInput(void);
	void EndInput(void);
	wstring GetInputString(void) { return m_inputString; }

	void Typing(void);

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

private:
	bool m_bInputString;
	wstring m_inputString;
};

