﻿namespace server
{
#define CHAT_SERVER_PORT 5000
#define GAME_SERVER_PORT 5000

#define MAX_STR_SIZE 100
#define MAX_USER 3000

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

	enum class EFFECT_TYPE
	{
		NONE = 0,

		BLINK,

		BRIGHT_FLARE_DARK_GRAY,
		BRIGHT_FLARE_DARK_GRAY_HUED,
		BRIGHT_FLARE_GRAY,
		BRIGHT_FLARE_INFINITY,
		BRIGHT_FLARE_PURPLE_HUED,

		CIRCLE_FLAME_DARK_BLUE,
		CIRCLE_FLAME_DARK_GRAY,
		CIRCLE_FLAME_GRAY,
		CIRCLE_FLAME_PURPLE,
		CIRCLE_FLAME_RED,
		CIRCLE_FLAME_YELLOW,

		CIRCLE_WAVE,

		EXPLODE,

		EXPLOSION_STAR_PARTICLE,

		FIRE,

		FLASH_IN_DARK,
		FLASH_IN_DARK_BLUE,
		FLASH_IN_GREEN,
		FLASH_IN_LIGHT_BLUE,
		FLASH_IN_RED,

		HIT,

		IMPLODE_DARK_BLUE,
		IMPLODE_DARK_GRAY,
		IMPLODE_GRAY,
		IMPLODE_GREEN,
		IMPLODE_ORANGE_BROWN,
		IMPLODE_PURPLE,
		IMPLODE_RED,
		IMPLODE_YELLOW,

		IMPLOSION_GRAY,
		IMPLOSION_GREEN,

		IMPACT1,
		IMPACT2,
		IMPACT3,
		IMPACT4,
		IMPACT5,
		IMPACT6,
		IMPACT7,
		IMPACT8,
		IMPACT9,
		IMPACT10,
		IMPACT11,
		IMPACT13,
		IMPACT14,
		IMPACT15,
		IMPACT16,
		IMPACT17,
		IMPACT18,
		IMPACT19,

		IN_DISPERSAL,

		IN_STAR_BURST_GRAY,
		IN_STAR_BURST_INFINITY,

		PARALYS,

		SHIELD_ELECTRIC_DARK_GRAY,
		SHIELD_ELECTRIC_GREEN,

		SLASH,
		SLASH_SPECIAL,

		SMOKEY_FLICKER_FLARE_DARK,

		SMOKEY_SWIRL,

		SPARKLING_FIREBALL,
		SPARKLING_FIREBALL_SMALL,
		SPARKLING_FIREBALL_WIND,
		SPARKLING_FIREBALL_WIND_SMALL,

		STAR_OUT_AREA_ELIPSE_DARK,
		STAR_OUT_AREA_ELIPSE_RED,

		SWORD1,
		FIRE_SWORD,
		ICE_SWORD,
		LIGHTNING_SWORD,
		STAR_SWORD,
		V_SWORD,
		LIGHT_WAVE_SWORD,
		LUNE_SWORD,
		CROSS_SWORD,
		SPELL_SWORD,

		TELEPORT_CIRCLE,

		TORCH_DRIPPING_DARK_BLUE,
		TORCH_DRIPPING_DARK_GRAY,
		TORCH_DRIPPING_GREEN,
		TORCH_DRIPPING_ORANGE_BROWN,
		TORCH_DRIPPING_PURPLE,
		TORCH_DRIPPING_RED,
		TORCH_DRIPPING_YELLOW,

		WHIRL_POOL_DARK_BLUE,
		WHIRL_POOL_DARK_GRAY,
		WHIRL_POOL_GRAY,
		WHIRL_POOL_GREEN,
		WHIRL_POOL_YELLOW,

		ZAP_SPIRAL_GRAY,
		ZAP_SPIRAL_GREEN,
		ZAP_SPIRAL_YELLOW,

		MAX
	};

	enum class SOUND_TYPE
	{
		NONE = 0,

		OPENING,
		WORLD,
		BATTLE,

		PUNCH,
		SMASH,
		LANDING,
		ROAR,

		MAX
	};
}