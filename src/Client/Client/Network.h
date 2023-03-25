#pragma once

class CGameObject;

namespace network
{
	class CNetwork
	{
	public:
		CNetwork();
		~CNetwork();

		void Connect();
		void EndThreadProcess();
		void WaitForThreadTermination();

#pragma region [SEND PACKET]
		void SendLoginPacket();
		void SendMovePacket(DIRECTION direction);
		void SendRotationPacket(ROTATION direction);
		void SendAniIndexPacket(int32_t index, float updateTime);
#pragma endregion

		constexpr uint32_t GetID() const { return m_id; }
	private:
		void ProcessThread();

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

		void AddPlayer(int32_t id);
		void MovePlayer(int32_t id);
		void RotatePlayer(int32_t id);
		void PlayAni(int32_t id);

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
		
		int32_t m_id;
		std::shared_ptr<CGameObject> m_myObject;
		std::unordered_map<int32_t, int32_t> m_idMatch;	// Client_ID, 연관된 플레이어 캐릭터 index
	};
}