﻿#include "Packet.h"
#include <WS2tcpip.h>
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

	void OVERLAPPEDEX::Set(network::CPacket& packet)
	{
		type = COMPLETION::SEND;
		wsa.len = static_cast<ULONG>(packet.GetPacketSize());
		wsa.buf = data;

		ZeroMemory(&over, sizeof(over));
		std::memcpy(data, packet.GetPacketAddr(), packet.GetPacketSize());
	}

	void OVERLAPPEDEX::Set(char* packet)
	{
		type = COMPLETION::SEND;
		wsa.len = static_cast<ULONG>(packet[0]);
		wsa.buf = data;

		ZeroMemory(&over, sizeof(over));
		std::memcpy(data, packet, packet[0]);
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