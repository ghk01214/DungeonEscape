#include "pch.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Network.h"
#include "Transform.h"
#include <iostream>

namespace network
{
	CNetwork::CNetwork(boost::asio::io_context& context, const tcp::endpoint& endpoints) :
		m_ioContext{ context },
		m_socket{ context },
		m_recvPacketSize{ 0 },
		m_prevPacketSize{ 0 },
		m_id{ INT_MAX }
	{
		Connect(endpoints);
	}

	CNetwork::~CNetwork()
	{
	}

	void CNetwork::Connect(const tcp::endpoint& endpoints)
	{
		m_socket.async_connect(endpoints,
			[this](boost::system::error_code error)
			{
				if (error)
				{
					// Connection Failed
					return;
				}

				Recv();
			});
	}

	void CNetwork::Recv()
	{
		m_socket.async_read_some(boost::asio::buffer(m_recvPacket, packet::CPacket::BUFF_SIZE),
			[this](boost::system::error_code error, std::size_t length)
			{
				if (error)
				{
					// Recv Failed
					m_socket.close();

					return;
				}

				int32_t dataToProcess{ static_cast<int32_t>(length) };
				uint8_t* pRecvBuf{ m_recvPacket.data() };
				auto recvIter{ m_recvPacket.begin() };

				while (dataToProcess > 0)
				{
					if (m_recvPacketSize == 0)
					{
						m_recvPacketSize = pRecvBuf[4];

						// 왜 200인지는 몰?루
						if (m_recvPacketSize > 200)
						{
							// Invalid Packet Size
							exit(-1);
						}
					}

					int32_t needToBuild{ m_recvPacketSize - m_prevPacketSize };

					if (needToBuild <= dataToProcess)
					{
						m_packet.SetData(recvIter, needToBuild, m_prevPacketSize);
						ProcessPacket();

						m_recvPacketSize = 0;
						m_prevPacketSize = 0;

						dataToProcess -= needToBuild;
						pRecvBuf += needToBuild;
						std::advance(recvIter, needToBuild);
					}
					else
					{
						m_packet.SetData(recvIter, dataToProcess, m_prevPacketSize);

						m_prevPacketSize += dataToProcess;
						std::advance(recvIter, dataToProcess);
						dataToProcess = 0;
					}
				}

				Recv();
			});
	}

	void CNetwork::Run()
	{
		std::thread th{ &CNetwork::Update, this };
		th.join();	
	}

	void CNetwork::Update()
	{
		m_ioContext.run();
	}

	void CNetwork::Send(packet::CPacket& packet)
	{
		std::cout << "Start send" << std::endl;

		auto size{ packet.GetTotalPacketSize() };

		m_socket.async_write_some(boost::asio::buffer(packet.GetPacketData(), size),
			[this, size](boost::system::error_code error, std::size_t bytesTransferred)
			{
				if (error)
				{
					// Send error
					std::cout << "Send error, error code : " << error << std::endl;
					return;
				}

				if (size != bytesTransferred)
				{
					// Incomplete send
					std::cout << "Incomplete send occured, error code : " << error << std::endl;
				}

				std::cout << "Send complete" << std::endl;
			});
	}

	void CNetwork::ProcessPacket()
	{
		if (m_id == INT_MAX)
		{
			m_id = m_packet.ReadID();
			return;
		}

		if (m_id != m_packet.ReadID())
		{
			return;
		}

		ProtocolID packetType{ m_packet.Read<ProtocolID>() };

		if (ProtocolID::PROTOCOL_AU_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_MY_BEGIN)
		{
			AuthorizePacketProcess(packetType);
		}
		else if (ProtocolID::PROTOCOL_MY_BEGIN <= packetType and
			packetType < ProtocolID::PROTOCOL_BATTLE_BEGIN)
		{
			MyPacketProcess(packetType);
		}
	}

	void CNetwork::AuthorizePacketProcess(ProtocolID type)
	{
		switch (type)
		{
			case ProtocolID::AU_LOGIN_ACK:
			{
				GET_SINGLE(CSceneManager)->SetTarget(m_packet.Read<uint16_t>());
			}
			break;
			default:
			break;
		}
	}

	void CNetwork::MyPacketProcess(ProtocolID type)
	{
		uint8_t targetNum{ m_packet.Read<uint8_t>() };
		GET_SINGLE(CSceneManager)->SetTarget(targetNum);

		switch (type)
		{
			case ProtocolID::MY_MOVE_ACK:
			{
				Vec3 pos;
				pos.x = m_packet.Read<float>();
				pos.y = m_packet.Read<float>();
				pos.z = m_packet.Read<float>();

				GET_SINGLE(CSceneManager)->ObjectTranslationMode(pos);
			}
			break;
			case ProtocolID::MY_ROTATE_ACK:
			{
				Vec3 rotation;
				rotation.x = m_packet.Read<float>();
				rotation.y = m_packet.Read<float>();
				rotation.z = m_packet.Read<float>();

				GET_SINGLE(CSceneManager)->ObjectRotationMode(rotation);
			}
			break;
			default:
			break;
		}
	}

	void CNetwork::SendLoginRequestPacket(std::vector<std::shared_ptr<CGameObject>>& m_gameObjects)
	{
		std::cout << "Start packet build" << std::endl;
		packet::CPacket packet;

		packet.WriteID(m_id);
		packet.Write<ProtocolID>(ProtocolID::AU_LOGIN_REQ);
		packet.Write<uint16_t>(static_cast<uint16_t>(m_gameObjects.size()));

		for (int32_t i = 0; i < m_gameObjects.size(); ++i)
		{
			packet.Write<uint16_t>(static_cast<uint16_t>(i));
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalPosition().x);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalPosition().y);
			packet.Write<float>(m_gameObjects[i]->GetTransform()->GetLocalPosition().z);
		}

		std::cout << "Packet build finished" << std::endl;

		Send(packet);
	}

	void CNetwork::SendMoveRequestPacket(DIRECTION direction)
	{
		packet::CPacket packet;

		packet.WriteID(m_id);
		packet.Write<ProtocolID>(ProtocolID::MY_MOVE_REQ);
		packet.Write<DIRECTION>(direction);
		
		Send(packet);
	}

	void CNetwork::SendRotationRequestPacket(ROTATION direction)
	{
		packet::CPacket packet;

		packet.WriteID(m_id);
		packet.Write<ProtocolID>(ProtocolID::MY_ROTATE_REQ);
		packet.Write<ROTATION>(direction);

		Send(packet);
	}

	void CNetwork::SendTargetNumPacket(uint8_t targetNum)
	{
		packet::CPacket packet;

		packet.WriteID(m_id);
		packet.Write<ProtocolID>(ProtocolID::MY_NUM_REQ);

		Send(packet);
	}
}