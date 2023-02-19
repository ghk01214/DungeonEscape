#include "Session.h"
#include "Server.h"

namespace game
{
	CSession::CSession(CServer* server, tcp::socket socket) :
		m_server{ server },
		m_socket{ std::move(socket) },
		m_recvPacketSize{ 0 },
		m_prevPacketSize{ 0 },
		m_player{ new CPlayer{} }
	{
	}

	CSession::~CSession()
	{
	}

	void CSession::Run(uint32_t id)
	{
		m_player->Init(id);
		Recv();
		
		packet::CPacket packet;
		packet.WriteID(id);
		packet.WriteSize();
		Send(packet);
	}

	void CSession::Move(packet::CPacket& packet)
	{
		DIRECTION direction{ packet.Read<DIRECTION>() };

		m_player->Move(direction);
	}

	void CSession::Rotate(packet::CPacket& packet)
	{
		ROTATION direction{ packet.Read<ROTATION>() };

		m_player->Rotate(direction);
	}

	void CSession::Recv()
	{
		auto pSelf{ shared_from_this() };
		auto pPlayer{ dynamic_cast<CPlayer*>(m_player) };

		m_socket.async_read_some(boost::asio::buffer(m_packet, packet::CPacket::BUFF_SIZE),
			[this, pSelf, pPlayer](boost::system::error_code errorCode, std::size_t length)
			{
				if (errorCode)
				{
					if (errorCode.value() == boost::asio::error::operation_aborted)
					{
						std::cout << std::format("Recv operation aborted\n");
						return;
					}

					std::cout << std::format("Receive error on session[{}], error code : ", pPlayer->GetAccessID()) << errorCode << "\n";

					m_server->DecreaseActiveSessionNum();

					m_socket.shutdown(m_socket.shutdown_both);
					m_socket.close();

					return;
				}

				int32_t dataToProcess{ static_cast<int32_t>(length) };
				uint8_t* pRecvBuf{ m_packet.data() };
				auto recvIter{ m_packet.begin() };

				while (dataToProcess > 0)
				{
					if (m_recvPacketSize == 0)
					{
						m_recvPacketSize = pRecvBuf[4];

						if (m_recvPacketSize > 200)
						{
							std::cout << std::format("Invalid packet size [{}]! Terminating server!\n", m_recvPacketSize);
							exit(-1);
						}
					}

					int32_t needToBuild{ m_recvPacketSize - m_prevPacketSize };

					if (needToBuild <= dataToProcess)
					{
						m_recvPacket.SetData(recvIter, needToBuild, m_prevPacketSize);
						m_server->ProcessPacket(m_recvPacket);

						m_recvPacketSize = 0;
						m_prevPacketSize = 0;

						dataToProcess -= needToBuild;
						pRecvBuf += needToBuild;
						std::advance(recvIter, needToBuild);
					}
					else
					{
						m_recvPacket.SetData(recvIter, dataToProcess, m_prevPacketSize);

						m_prevPacketSize += dataToProcess;
						std::advance(recvIter, dataToProcess);
						dataToProcess = 0;
					}
				}

				Recv();
			});
	}

	void CSession::Send(packet::CPacket& packet)
	{
		auto pSelf{ shared_from_this() };
		auto pPlayer{ dynamic_cast<CPlayer*>(m_player) };
		auto size{ packet.GetTotalPacketSize() };

		m_socket.async_write_some(boost::asio::buffer(packet.GetPacketData(), size),
			[this, pSelf, pPlayer, size](boost::system::error_code errorCode, std::size_t bytesTransferred)
			{
				if (errorCode)
				{
					std::cout << std::format("Send error on session[{}], error code : ", pPlayer->GetAccessID()) << errorCode << "\n";
					return;
				}

				if (size != bytesTransferred)
				{
					std::cout << std::format("Incomplete send occured on session[{}]. This session should be closed.\n", pPlayer->GetAccessID());
				}
			});
	}
}