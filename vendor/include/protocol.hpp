﻿#pragma once

enum ProtocolBase : unsigned short
{
	PROTOCOL_VERSION = 0x0000,

	PROTOCOL_OFFSET = PROTOCOL_VERSION + 0x0000,
	PROTOCOL_OFFSET_AU = PROTOCOL_OFFSET + 0x0000, // Autorization 인증 관련 프로토콜
	PROTOCOL_OFFSET_MY = PROTOCOL_OFFSET + 0x0100, // My 나 자신(정보, 플레이...) 관련 프로토콜
	PROTOCOL_OFFSET_WR = PROTOCOL_OFFSET + 0x0200, // World 게임 월드 관련 프로토콜
	PROTOCOL_OFFSET_BT = PROTOCOL_OFFSET + 0x0300, // Battle 플레이어 관련 프로토콜
	PROTOCOL_OFFSET_IF = PROTOCOL_OFFSET + 0x0400, // Information 정보 관련 프로토콜
	PROTOCOL_OFFSET_IT = PROTOCOL_OFFSET + 0x0500, // Item 정보 관련 프로토콜
	PROTOCOL_OFFSET_CM = PROTOCOL_OFFSET + 0x0600, // Community 커뮤니티 관련 프로토콜
	PROTOCOL_OFFSET_EC = PROTOCOL_OFFSET + 0x0700, // Economy 인게임 경제 관련 프로토콜
	PROTOCOL_OFFSET_GM = PROTOCOL_OFFSET + 0x0800, // GameMaster GM(운영자) 관련 프로토콜
	PROTOCOL_OFFSET_TT = PROTOCOL_OFFSET + 0x0900, // Test 테스트 관련 프로토콜
};

enum class ProtocolID : unsigned short
{
#pragma region [AUTORIZATION]
	PROTOCOL_AU_BEGIN = ProtocolBase::PROTOCOL_OFFSET_AU,

	AU_LOGIN_REQ,
	AU_LOGIN_ACK,

	PROTOCOL_AU_END = ProtocolBase::PROTOCOL_OFFSET_MY,
#pragma endregion

#pragma region [MY]
	PROTOCOL_MY_BEGIN = PROTOCOL_AU_END,

	MY_ADD_REQ,
	MY_ADD_ACK,

	MY_TRANSFORM_REQ,	// 플레이어 이동 요청
	MY_TRANSFORM_ACK,	// 플레이어 이동 응답

	MY_JUMP_REQ,
	MY_JUMP_ACK,

	MY_ANI_REQ,
	MY_ANI_ACK,

	PROTOCOL_MY_END = ProtocolBase::PROTOCOL_OFFSET_WR,
#pragma endregion

#pragma region [WORLD]
	PROTOCOL_WR_BEGIN = PROTOCOL_MY_END,

	WR_ADD_REQ,		// 오브젝트 추가 요청
	WR_ADD_ACK,		// 오브젝트 추가 응답

	WR_REMOVE_REQ,	// 오브젝트 삭제 요청
	WR_REMOVE_ACK,	// 오브젝트 삭제 응답

	WR_TRANSFORM_REQ,
	WR_TRANSFORM_ACK,

	WR_JUMP_REQ,
	WR_JUMP_ACK,

	WR_ANI_REQ,
	WR_ANI_ACK,

	PROTOCOL_WR_END = ProtocolBase::PROTOCOL_OFFSET_BT,
#pragma endregion

#pragma region [BATTLE]
	PROTOCOL_BT_BEGIN = PROTOCOL_WR_END,

	PROTOCOL_BT_END = ProtocolBase::PROTOCOL_OFFSET_IF,
#pragma endregion

#pragma region [INFORMATION]
	PROTOCOL_IF_BEGIN = PROTOCOL_BT_END,
	PROTOCOL_IF_END = ProtocolBase::PROTOCOL_OFFSET_IT,
#pragma endregion

#pragma region [ITEM]
	PROTOCOL_IT_BEGIN = PROTOCOL_IF_END,
	PROTOCOL_IT_END = ProtocolBase::PROTOCOL_OFFSET_CM,
#pragma endregion

#pragma region [COMMUNITY]
	PROTOCOL_CM_BEGIN = PROTOCOL_IT_END,
	PROTOCOL_CM_END = ProtocolBase::PROTOCOL_OFFSET_EC,
#pragma endregion

#pragma region [ECONOMY]
	PROTOCOL_EC_BEGIN = PROTOCOL_CM_END,
	PROTOCOL_EC_END = ProtocolBase::PROTOCOL_OFFSET_GM,
#pragma endregion

#pragma region [GAME MASTER]
	PROTOCOL_GM_BEGIN = PROTOCOL_EC_END,
	PROTOCOL_GM_END = ProtocolBase::PROTOCOL_OFFSET_GM,
#pragma endregion

#pragma region [TEST]
	PROTOCOL_TT_BEGIN = PROTOCOL_GM_END,
	PROTOCOL_TT_END
#pragma endregion
};