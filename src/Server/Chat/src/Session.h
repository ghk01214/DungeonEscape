#pragma once

namespace chat
{
	SKIP_TCP_NAMESPACE;

	class CSession : public CPlayer, public std::enable_shared_from_this<CSession>
	{
	public:
		CSession(tcp::socket& socket, CRoom& room);
		~CSession();
		
		void Start();
		void Broadcast(const packet::CPacket message);
	private:
		void ReadHeader();
		void ReadData();
		void Write();

	private:
		tcp::socket& m_socket;

		CRoom& m_room;
		packet::CPacket m_readMessage;
		tbb::concurrent_queue<packet::CPacket> m_writeMessages;
	};
}