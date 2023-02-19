#include "Object.h"
#include "Server.h"
#include "Session.h"

namespace game
{
	CServer::CServer(boost::asio::io_context* context, int32_t port) :
		m_pContext{ context },
		m_accpetor{ *context, tcp::endpoint(tcp::v4(), port) },
		m_socket{ *context },
		m_userID{ 0 },
		m_activeSessionNum{0},

		m_targetNum{ 0 }
	{
		//std::setlocale(LC_ALL, "UTF-8");

		Accept();
	}

	CServer::~CServer()
	{
	}

	void CServer::Run()
	{
		for (int32_t i = 0; i < 4; ++i)
		{
			m_workerThreads.push_back(std::thread{ &game::CServer::WorkerThread, this });
		}

		std::ranges::for_each(m_workerThreads, [](std::thread& thread) { thread.join(); });
	}
	 
	void CServer::ProcessPacket(packet::CPacket& packet)
	{
		uint32_t id{ packet.ReadID() };
		ProtocolID packetType{ packet.Read<ProtocolID>() };

		if (ProtocolID::PROTOCOL_AU_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_MY_BEGIN)
		{
			AuthorizePacketProcess(packet, id, packetType);
		}
		else if (ProtocolID::PROTOCOL_MY_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_BATTLE_BEGIN)
		{
			MyPacketProcess(packet, id, packetType);
		}
	}

	void CServer::AuthorizePacketProcess(packet::CPacket& packet, uint32_t id, ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::AU_LOGIN_REQ:
			{
				uint16_t objectNum{ packet.Read<uint16_t>() };
				tbb::concurrent_hash_map<uint16_t, CObject*>::accessor access;

				for (uint16_t i = 0; i < objectNum; ++i)
				{
					uint16_t objIndex{ packet.Read<uint16_t>() };
					m_objects.insert(access, objIndex);

					CObject* obj{ new CObject{} };
					float x{ packet.Read<float>() };
					float y{ packet.Read<float>() };
					float z{ packet.Read<float>() };

					obj->SetPos(Pos{ x, y, z });
					access->second = obj;
				}

				m_targetNum = 0;

				SendTargetNumPacket(0);
			}
			break;
			default:
			break;
		}
	}

	void CServer::MyPacketProcess(packet::CPacket& packet, uint32_t id, ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::MY_NUM_REQ:
			{
				m_targetNum = packet.Read<uint16_t>();
			}
			break;
			case ProtocolID::MY_MOVE_REQ:
			{
				//m_sessions[id]->Move(packet);
				//SendMovePacket(id);
				tbb::concurrent_hash_map<uint16_t, CObject*>::accessor access;
				m_objects.find(access, m_targetNum);

				DIRECTION direction{ packet.Read<DIRECTION>() };
				access->second->Move(direction);

				SendMovePacket(id);
			}
			break;
			case ProtocolID::MY_ROTATE_REQ:
			{
				//m_sessions[id]->Rotate(packet);
				//SendRotatePacket(id);
				tbb::concurrent_hash_map<uint16_t, CObject*>::accessor access;
				m_objects.find(access, m_targetNum);

				ROTATION direction{ packet.Read<ROTATION>() };
				access->second->Rotate(direction);

				SendRotatePacket(id);
			}
			break;
			default:
			break;
		}
	}

	void CServer::DecreaseActiveSessionNum()
	{
		--m_activeSessionNum;
	}

	void CServer::WorkerThread()
	{
		m_pContext->run();
	}

	void CServer::Accept()
	{
		m_accpetor.async_accept(m_socket,
			[this](boost::system::error_code errorCode)
			{
				if (!errorCode)
				{
					std::format("Accepted\n");

					uint32_t newID{ GetNewAccessID() };
					m_sessions[newID] = std::make_shared<CSession>(this, std::move(m_socket));
					m_sessions[newID]->Run(newID);
				}

				Accept();
			});
	}

	const uint32_t CServer::GetNewID()
	{
		//++m_activeSessionNum;
		//
		//uint32_t newID{ m_randomID(dre) };
		//
		//while (m_nonavailableID.contains(newID) == true)
		//{
		//	newID = m_randomID(dre);
		//}
		//
		//m_nonavailableID.insert(newID);
		//
		//return newID;
		return 0;
	}

	uint32_t CServer::GetNewAccessID()
	{
		if (m_activeSessionNum >= MAX_USER)
		{
			std::cout << std::format("MAX USER FULL\n");
			return MAX_USER + 1;
		}

		++m_activeSessionNum;
		bool issueNewID{ m_reusableID.empty() };

		if (issueNewID == true)
			return m_userID++;

		uint32_t newID{ std::numeric_limits<uint32_t>::max() };

		while (true)
		{
			bool issueReuseID{ m_reusableID.try_pop(newID) };

			if (issueReuseID == true)
				return newID;
		}
	}

	void CServer::SendTargetNumPacket(uint32_t id)
	{
		packet::CPacket packet;
		
		packet.WriteID(id);
		packet.Write<ProtocolID>(ProtocolID::AU_LOGIN_ACK);
		packet.Write<uint16_t>(m_targetNum);

		m_sessions[id]->Send(packet);
	}

	void CServer::SendMovePacket(uint32_t id)
	{
		packet::CPacket packet;
		tbb::concurrent_hash_map<uint16_t, CObject*>::accessor access;
		m_objects.find(access, m_targetNum);
		Pos pos{ access->second->GetPos() };

		packet.WriteID(id);
		packet.Write<ProtocolID>(ProtocolID::MY_MOVE_ACK);
		packet.Write<float>(pos.x);
		packet.Write<float>(pos.y);
		packet.Write<float>(pos.z);

		m_sessions[id]->Send(packet);
	}
	
	void CServer::SendRotatePacket(uint32_t id)
	{
		packet::CPacket packet;
		tbb::concurrent_hash_map<uint16_t, CObject*>::accessor access;
		m_objects.find(access, m_targetNum);
		Rotation rotate{ access->second->GetRotate() };

		packet.WriteID(id);
		packet.Write<ProtocolID>(ProtocolID::MY_ROTATE_ACK);
		packet.Write<float>(rotate.x);
		packet.Write<float>(rotate.y);
		packet.Write<float>(rotate.z);

		m_sessions[id]->Send(packet);
	}
}