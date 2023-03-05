#define CHAT_SERVER_PORT 5000
#define GAME_SERVER_PORT 6000

#define MAX_STR_SIZE 100
#define MAX_LENGTH 4086
#define MAX_USER 2000

// 이동방향 enum class
enum class DIRECTION : uint8_t
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

// 회전 방향 enum class
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