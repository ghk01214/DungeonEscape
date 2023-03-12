#pragma once

class GameObject;

namespace network
{
	class CNetwork
	{
	public:
		CNetwork();
		~CNetwork();

		void Connect();
		void ProcessThread();
		void EndThread();

		void Recv();
		void Recv(DWORD bytes, network::OVERLAPPEDEX* pOverEx);
		void Send(DWORD bytes, network::OVERLAPPEDEX* pOverEx);

#pragma region [SEND PACKET]
		void SendLoginPacket(std::vector<std::shared_ptr<GameObject>>& m_gameObjects);
		void SendMovePacket(DIRECTION direction);
		void SendRotationPacket(ROTATION direction);
#pragma endregion
	private:
		void Send(network::CPacket& packet);

		void ProcessPacket();
		void ProcessAUPacket(ProtocolID type);
		void ProcessMYPacket(ProtocolID type);
		void ProcessBTPacket(ProtocolID type);
		void ProcessITPacket(ProtocolID type);
		void ProcessCMPacket(ProtocolID type);
		void ProcessTTPacket(ProtocolID type);

	private:
		HANDLE m_iocp;
		SOCKET m_socket;
		int32_t m_serverKey;

		std::thread m_networkThread;

		OVERLAPPEDEX m_recvEx;
		OVERLAPPEDEX m_sendEx;

		char* m_recvPacket;
		network::CPacket m_packet;
		int32_t m_remainSize;

		uint16_t m_myTarget;
	};
}