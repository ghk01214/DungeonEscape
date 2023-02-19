#include "Player.h"
#include "Server.h"

namespace chat
{
	std::array<CPlayer, MAX_USER> CServer::m_players{};
	std::atomic_int32_t CServer::m_playerNum{ 0 };
	std::atomic_int32_t CServer::m_userID{ 0 };
	tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> CServer::m_reuseID{};

	CServer::CServer(tcp::socket socket) :
		m_socket{ std::move(socket) },
		m_recvPacketSize{ 0 },
		m_prevDataSize{ 0 }
	{
		InitPlayers();
	}

	CServer::~CServer()
	{
	}

	void CServer::Run()
	{
		m_id = GetNewID();

		std::cout << std::format("Client[{}] Connected\n", m_id + 1);

		m_players[m_id].pSession = this;
		m_players[m_id].connected = true;
		m_players[m_id].startTime = steady_clock::now();

		Read();
	}

	void CServer::InitPlayers()
	{
		std::ranges::for_each(m_players,
			[](Player& player)
			{
				player.connected = false;
				player.message.reserve(MAX_LENGTH);
			});
	}

	void CServer::Send(packet::CPacket& packet, uint32_t id)
	{
		m_players[id].pSession->Write(packet);
	}

	void CServer::ProcessPacket(packet::CPacket& packet, int32_t id)
	{
		std::wstring wstr{};
		packet.ReadWString(wstr);
		std::wcout << std::format(L"{} : {}\n", id, wstr);

		packet::CPacket sendPacket;
		sendPacket.SetData(id);
		sendPacket.WriteWString(wstr);

		for (int32_t i = 0; i < m_playerNum; ++i)
		{
			Send(sendPacket, i);
		}
	}

	void CServer::Read()
	{
		auto pSelf{ shared_from_this() };

		m_socket.async_read_some(boost::asio::buffer(m_recvData, packet::CPacket::BUFF_SIZE),
			[this, pSelf](boost::system::error_code errorCode, std::size_t length)
			{
				if (errorCode)
				{
					if (errorCode.value() == boost::asio::error::operation_aborted)
						return;

					if (m_players[m_id].connected)
						return;

					std::cout << std::format("Receive Error on Session[{}], Error Code : ", m_id) << errorCode << "\n";

					m_socket.shutdown(m_socket.shutdown_both);
					m_socket.close();

					m_players[m_id].connected = false;
					m_players[m_id].message.clear();
					m_reuseID.push(m_id);
					--m_playerNum;

					return;
				}

				int32_t dataToProcess{ static_cast<int32_t>(length) };
				uint8_t* pRecvBuf{ m_recvData.data() };
				auto recvIter{ m_recvData.begin() };

				while (dataToProcess > 0)
				{
					if (m_recvPacketSize == 0)
					{
						m_recvPacketSize = pRecvBuf[2];

						if (m_recvPacketSize > 200)
						{
							std::cout << std::format("Invalid Packet Size[{}] Terminating Server!\n", m_recvPacketSize);
							exit(-1);
						}
					}

					int32_t needToBuild{ m_recvPacketSize - m_prevDataSize };

					if (needToBuild <= dataToProcess)
					{
						m_packet.SetData(recvIter, needToBuild, m_prevDataSize);
						ProcessPacket(m_packet, m_id);

						m_recvPacketSize = 0;
						m_prevDataSize = 0;

						dataToProcess -= needToBuild;
						pRecvBuf += needToBuild;
						std::advance(recvIter, needToBuild);
					}
					else
					{
						m_packet.SetData(recvIter, dataToProcess, m_prevDataSize);

						m_prevDataSize += dataToProcess;
						pRecvBuf += dataToProcess;
						std::advance(recvIter, dataToProcess);
						dataToProcess = 0;
					}
				}
				
				Read();
			});

	}

	void CServer::Write(packet::CPacket& packet)
	{
		auto pSelf{ shared_from_this() };
		auto size{ packet.GetTotalPacketSize() };

		//std::memcpy(m_sendData.data(), &packet.GetPacketData(), size);

		m_socket.async_write_some(boost::asio::buffer(packet.GetPacketData(), size),
			[this, pSelf, size](boost::system::error_code errorCode, std::size_t bytesTransffered)
			{
				if (errorCode)
					return;
				
				if (size != bytesTransffered)
				{
					std::cout << std::format("Incomplete Send occured on session[{}]. This session should be closed.\n", m_id);
				}
			});
	}

	void CServer::EmptyPacket(PacketData& packet)
	{
		packet.fill(0);
	}

	const int32_t CServer::GetNewID()
	{
		if (m_playerNum >= MAX_USER)
		{
			std::cout << std::format("MAX USER FULL\n");
			return -1;
		}
		
		++m_playerNum;

		if (m_reuseID.empty() == true)
			return m_userID++;

		//int32_t newID{ m_reuseID.top() };
		//m_reuseID.pop();

		return 0;
	}
}