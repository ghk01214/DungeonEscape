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

		void Set(network::CPacket& packet);
		void Set(char* packet);
		void Reset();

	public:
		OVERLAPPED over;
		WSABUF wsa;
		COMPLETION type;
		int targetID;
		char data[network::CPacket::BUFF_SIZE];
	};
}