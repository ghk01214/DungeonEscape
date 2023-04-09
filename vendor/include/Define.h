namespace server
{
#define CHAT_SERVER_PORT 5000
#define GAME_SERVER_PORT 5000

#define MAX_STR_SIZE 100
#define MAX_LENGTH 4086
#define MAX_USER 2000

#define MAX_SCENE 3

	enum class KEY_TYPE
	{
		NONE = 0x00,

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

		KEY_1 = '1',
		KEY_2 = '2',
		KEY_3 = '3',
		KEY_4 = '4',

		LBUTTON = VK_LBUTTON,
		RBUTTON = VK_RBUTTON,

		MAX
	};

	enum class KEY_STATE : uint8_t
	{
		NONE = 0,
		DOWN,
		PRESS,
		UP,
		MAX
	};

	enum class MOVE_DIRECTION : uint8_t
	{
		NONE = 0,

		LEFT,
		RIGHT,
		FRONT,
		BACK,
		UP,
		DOWN,

		MAX
	};

	enum class ROTATION : uint8_t
	{
		NONE = 0,

		X_INCREASE,
		X_DECREASE,
		Y_INCREASE,
		Y_DECREASE,
		Z_INCREASE,
		Z_DECREASE,

		MAX
	};
}