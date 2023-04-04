#pragma once

class CGameObject;

namespace network
{
	class NetworkManager
	{
	private:
		using Object = std::vector<std::shared_ptr<CGameObject>>;

		DECLARE_SINGLE(NetworkManager);

	public:
		void Init();
		void RegisterObject(std::shared_ptr<CGameObject> object);
		void RegisterObject(Object object);

		void Connect();
		void EndThreadProcess();
		void CloseThread();

		void Recv();
		void Send(network::CPacket& packet);

		void SendLoginPacket();

		constexpr uint32_t GetID() const { return m_id; }
	private:
		void ProcessThread();

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
		void RemovePlayer(int32_t id);
		void TransformPlayer(int32_t id);
		void PlayAni(int32_t id);

	private:
		HANDLE m_iocp;
		SOCKET m_socket;
		int32_t m_serverKey;

		std::thread m_networkThread;

		OVERLAPPEDEX m_recvEx;
		OVERLAPPEDEX m_sendEx;

		int8* m_pRecvPacket;
		network::CPacket m_packet;
		int32_t m_remainSize;

		int32_t m_id;
		std::unordered_map<int32_t, Object> m_objects;
	};
}