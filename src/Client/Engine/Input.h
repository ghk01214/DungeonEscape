#pragma once

#include <bitset>

enum class KEY_TYPE
{
	NONE = 0,

	UP,
	DOWN,
	LEFT,
	RIGHT,
	SPACE,

	W,
	A,
	S,
	D,

	Q,
	E,
	Z,
	C,

	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,

	LBUTTON,
	RBUTTON,

	MAX
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
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
	KEY_USE_COUNT = static_cast<int32_t>(KEY_TYPE::MAX)
};

class Input
{
	DECLARE_SINGLE(Input);

public:
	void Init(HWND hWnd);
	void Update();

	// 누르고 있을 때
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

	const POINT& GetMousePos() { return m_mousePos; }
	//constexpr std::bitset<KEY_USE_COUNT>& GetKeyInput() { return m_keyInput; }
	const unsigned long GetKeyInput() const { return m_keyInput.to_ulong(); }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return m_states[static_cast<uint8>(key)]; }

private:
	HWND m_hWnd;
	vector<KEY_STATE> m_states;
	POINT m_mousePos = {};

	std::bitset<KEY_USE_COUNT> m_keyInput;
};

