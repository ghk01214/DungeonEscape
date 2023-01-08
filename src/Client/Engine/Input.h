#pragma once

enum class KEY_TYPE
{
	Number1 = '1',
	Number2 = '2',
	Number3 = '3',
	Number4 = '4',

	NumPad1 = VK_NUMPAD1,
	NumPad2 = VK_NUMPAD2,
	NumPad3 = VK_NUMPAD3,
	NumPad4 = VK_NUMPAD4,

	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	W = 'W',
	A = 'A',
	S = 'S',
	D = 'D',

	Q = 'Q',
	E = 'E',

	R = 'R',
	T = 'T',

	C = 'C',
	V = 'V',

	PAGEUP = VK_PRIOR,
	PAGEDOWN = VK_NEXT
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),		// 키 256개의 상태를 관리할 수 있다. 2의 8승 => 256
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class CInput
{
	DECLARE_SINGLE(CInput);

public:
	void Init(HWND hwnd);
	void Update();

	// 누르고 있을 때
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return m_states[static_cast<uint8>(key)]; }

private:
	HWND m_hwnd;
	std::vector<KEY_STATE> m_states;
};

