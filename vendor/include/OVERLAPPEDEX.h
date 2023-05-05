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

	enum class POST_COMPLETION
	{
		NONE = 0,
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
		virtual void Reset();

	public:
		OVERLAPPED over;
		WSABUF wsa;
		COMPLETION type;
		int32_t targetID;
		char data[CPacket::BUFF_SIZE];
	};

	class PostOVERLAPPEDEX : private OVERLAPPEDEX
	{
	public:
		PostOVERLAPPEDEX(POST_COMPLETION type = POST_COMPLETION::BROADCAST);
		~PostOVERLAPPEDEX() = default;

		void Reset() override;

		OVERLAPPED& GetOVERLAPPED() { return over; }

	public:
		POST_COMPLETION type;
		ProtocolID msgProtocol;
		int32_t objID;
		int32_t roomID;
		unsigned long keyInput;
	};
}