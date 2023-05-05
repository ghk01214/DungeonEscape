#pragma once

namespace network
{
	class CPacket;
}

namespace network
{
	enum class COMPLETION
	{
		NONE = 0,
		ACCEPT,
		RECV,
		SEND,
		BROADCAST,
		QUIT,
		MAX
	};

	class OVERLAPPEDEX
	{
	public:
		OVERLAPPEDEX(COMPLETION type = COMPLETION::RECV);
		~OVERLAPPEDEX() = default;

		void Set(CPacket& packet);
		void Set(char* packet);
		void Reset();

	public:
		OVERLAPPED over;
		WSABUF wsa;
		COMPLETION type;
		ProtocolID msgProtocol;
		int32_t playerID;
		int32_t objID;
		int32_t roomID;
		char data[CPacket::BUFF_SIZE];
	};
}