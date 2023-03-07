#pragma once

enum ProtocolBase : unsigned short
{
	PROTOCOL_VERSION = 0x0000,

	PROTOCOL_OFFSET = PROTOCOL_VERSION + 0x1000,
	PROTOCOL_OFFSET_AU = PROTOCOL_OFFSET + 0x0000, // Autorization 인증 관련 프로토콜
	PROTOCOL_OFFSET_MY = PROTOCOL_OFFSET + 0x0100, // My 나 자신(정보, 플레이...) 관련 프로토콜
	PROTOCOL_OFFSET_BT = PROTOCOL_OFFSET + 0x0200, // Battle 플레이어 관련 프로토콜
	PROTOCOL_OFFSET_IF = PROTOCOL_OFFSET + 0x0300, // Information 정보 관련 프로토콜
	PROTOCOL_OFFSET_IT = PROTOCOL_OFFSET + 0x0400, // Item 정보 관련 프로토콜
	PROTOCOL_OFFSET_CM = PROTOCOL_OFFSET + 0x0500, // Community 커뮤니티 관련 프로토콜
	PROTOCOL_OFFSET_EC = PROTOCOL_OFFSET + 0x0600, // Economy 인게임 경제 관련 프로토콜
	PROTOCOL_OFFSET_GM = PROTOCOL_OFFSET + 0x0700, // GameMaster GM(운영자) 관련 프로토콜
	PROTOCOL_OFFSET_TT = PROTOCOL_OFFSET + 0x0800, // Test 테스트 관련 프로토콜
};

enum class ProtocolID : unsigned short
{
#pragma region [AUTORIZATION]
	PROTOCOL_AU_BEGIN = ProtocolBase::PROTOCOL_OFFSET_AU,

	AU_LOGIN_REQ,
	AU_LOGIN_ACK,
#pragma endregion

#pragma region [MY]
	PROTOCOL_MY_BEGIN = ProtocolBase::PROTOCOL_OFFSET_MY,

	MY_NUM_REQ,		// 상호작용 오브젝트 번호 요청
	MY_NUM_ACK,		// 상호작용 오브젝트 번호 응답

	MY_MOVE_REQ,	// 플레이어 이동 요청
	MY_MOVE_ACK,	// 플레이어 이동 응답

	MY_ROTATE_REQ,	// 플레이어 회전 요청
	MY_ROTATE_ACK,	// 플레이어 회전 응답
#pragma endregion

#pragma region [BATTLE]
	PROTOCOL_BATTLE_BEGIN = ProtocolBase::PROTOCOL_OFFSET_BT,
#pragma endregion

#pragma region [INFORMATION]
	PROTOCOL_INFORMATION_BEGIN = ProtocolBase::PROTOCOL_OFFSET_IF,
#pragma endregion

#pragma region [ITEM]
	PROTOCOL_ITEM_BEGIN = ProtocolBase::PROTOCOL_OFFSET_IT,
#pragma endregion

#pragma region [COMMUNITY]
	PROTOCOL_COMMUNITY_BEGIN = ProtocolBase::PROTOCOL_OFFSET_CM,
#pragma endregion

#pragma region [ECONOMY]
	PROTOCOL_ECONOMY_BEGIN = ProtocolBase::PROTOCOL_OFFSET_EC,
#pragma endregion

#pragma region [GAME MASTER]
	PROTOCOL_GAME_MASTER_BEGIN = ProtocolBase::PROTOCOL_OFFSET_GM,
#pragma endregion

#pragma region [TEST]
	PROTOCOL_TEST_BEGIN = ProtocolBase::PROTOCOL_OFFSET_TT,
#pragma endregion
};