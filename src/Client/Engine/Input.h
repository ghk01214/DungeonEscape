#pragma once

#include <bitset>

enum class KEY_TYPE
{
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

	MAX = 19
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

	const POINT& GetMousePos() { return m_curMousePos; }
	const unsigned long GetKeyInput() const { return m_keyInputState.to_ulong(); }
	const bool IsNoInput() const { return m_keyInputState.none(); }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return m_states[static_cast<uint8>(key)]; }
	void EncodeKeyInput(void);

public:
	Vec2 GetMouseMove(void);	// 이전 프레임과 현재 프레임의 마우스의 x,y 좌표의 움직이는 정도를 반환하는 함수

private:
	HWND m_hWnd;
	vector<KEY_STATE> m_states;
	POINT m_curMousePos = POINT{ 0, 0 };
	POINT m_preMousePos = POINT{ 0, 0 };

	std::bitset<KEY_STATE_USE> m_keyInputState;

	vector<KEY_TYPE> m_useKeyType;
};

