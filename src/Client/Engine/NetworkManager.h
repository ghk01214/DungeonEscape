#pragma once

class CGameObject;

namespace network
{
	/*class NetworkManager
	{
		DECLARE_SINGLE(NetworkManager);

	public:
		void Init();
		void RegisterObject(std::shared_ptr<CGameObject> object);

		void Connect();
		void WaitForThreadTermination();

#pragma region [SEND PACKET]
		void SendLoginPacket();
		void SendMovePacket(DIRECTION direction);
		void SendRotationPacket(ROTATION direction);
#pragma endregion
	private:
		void ProcessThread();
		void EndThreadProcess();

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
		void ProcessIFPacket(ProtocolID type);
		void ProcessITPacket(ProtocolID type);
		void ProcessCMPacket(ProtocolID type);
		void ProcessECPacket(ProtocolID type);
		void ProcessGMPacket(ProtocolID type);
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

		std::unordered_map<uint32_t, std::shared_ptr<CGameObject>> m_objects;
	};*/
}