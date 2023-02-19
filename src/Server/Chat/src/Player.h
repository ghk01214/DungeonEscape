#pragma once

namespace chat
{
	SKIP_STEADY_CLOCK_NAMESPACE;

	class CPlayer
	{
	public:
		CPlayer(const uint16_t id);
		virtual ~CPlayer() = default;

		virtual void Broadcast(const packet::CPacket message) {}
	protected:
		uint16_t m_id;
		steady_clock m_sendTime;
	};
}