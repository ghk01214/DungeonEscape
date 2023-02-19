#pragma once

class CGameObject;

namespace network
{
	using boost::asio::ip::tcp;

	class CNetwork
	{
	public:
		CNetwork(boost::asio::io_context& context, const tcp::endpoint& endpoints);
		~CNetwork();

		void Connect(const tcp::endpoint& endpoints);
		void Recv();

		void Run();

		void SendLoginRequestPacket(std::vector<std::shared_ptr<CGameObject>>& m_gameObjects);
		void SendMoveRequestPacket(DIRECTION direction);
		void SendRotationRequestPacket(ROTATION direction);
		void SendTargetNumPacket(uint8_t targetNum);

		void Update();
	private:
		void Send(packet::CPacket& packet);

		void ProcessPacket();
		void AuthorizePacketProcess(ProtocolID type);
		void MyPacketProcess(ProtocolID type);

	private:
		boost::asio::io_context& m_ioContext;
		tcp::socket m_socket;
		uint32_t m_id;

		std::array<uint8_t, packet::CPacket::BUFF_SIZE> m_recvPacket;
		packet::CPacket m_packet;
		uint16_t m_recvPacketSize;
		uint16_t m_prevPacketSize;
	};
}