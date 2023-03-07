#include <Packet.h>
#include "Player.h"
#include <numeric>

namespace game
{
	CPlayer::CPlayer() :
		m_id{ std::numeric_limits<uint32_t>::max() },
		m_accessID{ std::numeric_limits<uint16_t>::max() },
		m_connected{ false }
	{
	}

	CPlayer::~CPlayer()
	{
	}

	void CPlayer::Init(uint32_t id)
	{
		m_accessID = id;
	}
}