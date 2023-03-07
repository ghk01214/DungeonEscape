#include "pch.h"
#include "OVERLAPPEDEX.h"

namespace network
{
	OVERLAPPEDEX::OVERLAPPEDEX(COMPLETION type) :
		wsa{ 0, nullptr },
		type{ type },
		targetID{ -1 }
	{
		ZeroMemory(&over, sizeof(over));
	}

	void OVERLAPPEDEX::Set(packet::CPacket& packet)
	{
		type = COMPLETION::SEND;
		wsa.len = static_cast<ULONG>(packet.GetDataSize());
		wsa.buf = data;

		ZeroMemory(&over, sizeof(over));
		std::memcpy(data, packet.GetPacketAddr(), packet.GetPacketSize());
	}

	void OVERLAPPEDEX::Reset()
	{
		wsa.len = 0;
		wsa.buf = nullptr;
		type = COMPLETION::RECV;
		targetID = -1;

		ZeroMemory(&over, sizeof(over));
	}
}