namespace server
{
#define CHAT_SERVER_PORT 5000
#define GAME_SERVER_PORT 5000

#define MAX_STR_SIZE 100
#define MAX_LENGTH 4086
#define MAX_USER 3
#define MAX_SCENE 3

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

	enum class KEY_STATE : uint8_t
	{
		NONE = 0,
		DOWN,
		PRESS,
		UP,
		MAX
	};

	enum class OBJECT_TYPE
	{
		NONE = 0,

		PLAYER,
		REMOTE_PLAYER,
		MONSTER,
		BOSS,
		MAP_OBJECT,

		PLAYER_FIREBALL,
		PLAYER_ICEBALL,
		PLAYER_THUNDERBALL,
		PLAYER_POISONBALL,
		PLAYER_METEOR,

		WEEPER_CAST1_BALL,
		WEEPER_CAST2_BALL,
		WEEPER_CAST2_BALL_SCATTER,
		WEEPER_CAST2_BALL_NUCLEAR,
		WEEPER_CAST3_BALL,
		WEEPER_CAST4_BALL,

		MONSTER_FIREBALL,
		MONSTER_ICEBALL,
		MONSTER_THUNDERBALL,
		MONSTER_POISONBALL,

		TRIGGER_OBJECT,

		PHYSX_OBJECT,

		MAX
	};

	enum class TRIGGER_TYPE
	{
		NONE = 0,

		SINGLE_STRIKE,
		MAGMA,
		WIND_RIGHT,
		WIND_LEFT,
		WIND_FRONT,
		WIND_BACK,
		WIND_UP,
		WIND_DOWN,

		MAX
	};

	enum class COLLIDER_TYPE
	{
		NONE = 0,

		MESH,
		BOX,
		SPHERE,
		CAPSULE,

		MAX
	};

	enum class PATTERN_TYPE
	{
		NONE = 0,

		WEEPER1,
		WEEPER2,
		WEEPER3,
		WEEPER4,

		GOLEM1,
		GOLEM2,
		GOLEM3,
		GOLEM4,
		GOLEM5,
		GOLEM6,

		MAX
	};

	enum class FBX_TYPE : uint8_t
	{
		NONE = 0,

		// PLYAER
		NANA,
		MISTIC,
		CARMEL,

		// MONSTER
		WEEPER1,
		WEEPER2,
		WEEPER3,
		WEEPER4,
		WEEPER5,
		WEEPER6,
		WEEPER7,
		WEEPER_EMISSIVE,
		BLUE_GOLEM,
		GREEN_GOLEM,
		RED_GOLEM,
		BLACK_SCORPION,
		ORANGE_SCORPION,
		PURPLE_SCORPION,
		RED_SCORPION,
		DRAGON,

		// SKILL
		PLAYER_FIREBALL,
		PLAYER_ICEBALL,
		PLAYER_THUNDERBALL,
		PLAYER_POISONBALL,
		PLAYER_METEOR,

		WEEPER_CAST1_BALL,
		WEEPER_CAST2_BALL,
		WEEPER_CAST2_BALL_SCATTER,
		WEEPER_CAST2_BALL_NUCLEAR,
		WEEPER_CAST3_BALL,
		WEEPER_CAST4_BALL,

		MONSTER_ICEBALL,
		MONSTER_THUNDERBALL,
		MONSTER_POISONBALL,
		MONSTER_DARKBALL,

		// OBJECT
		SCATTER_ROCK,		// 돌 굴리기 기믹의 벽 FBX
		ROLLING_ROCK,		// 돌 굴리기 기믹의 돌 FBX
		PILLAR_BRIDGE,		// 돌 다리 기믹의 다리 FBX

		MAX,
	};
}