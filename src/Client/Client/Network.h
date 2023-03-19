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
		void EndThreadProcess();
		void WaitForThreadTermination();

#pragma region [SEND PACKET]
		void SendLoginPacket();
		void SendMovePacket(DIRECTION direction);
		void SendRotationPacket(ROTATION direction);
#pragma endregion
	private:
		void Recv();
		void Send(network::CPacket& packet);

		void Recv(DWORD bytes, network::OVERLAPPEDEX* pOverEx);
		void Send(DWORD bytes, network::OVERLAPPEDEX* pOverEx);

#pragma region [PROCESS PACKET]
		void ProcessPacket();
		void ProcessAUPacket(ProtocolID type);
		void ProcessMYPacket(ProtocolID type);
		void ProcessWRPacket(ProtocolID type);
		void ProcessBTPacket(ProtocolID type);
		void ProcessITPacket(ProtocolID type);
		void ProcessCMPacket(ProtocolID type);
		void ProcessTTPacket(ProtocolID type);
#pragma endregion

	private:
		HANDLE m_iocp;
		SOCKET m_socket;
		int32_t m_serverKey;

		std::thread m_networkThread;

		OVERLAPPEDEX m_recvEx;
		OVERLAPPEDEX m_sendEx;

		char* m_pRecvPacket;
		network::CPacket m_packet;
		int32_t m_remainSize;

		// 임시 map
	public:
		std::unordered_map<int32_t, int32_t> m_idMatch;
	};
}