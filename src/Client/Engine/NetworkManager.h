#pragma once

#include <bitset>

class CGameObject;
class MonoBehaviour;

namespace network
{
	class NetworkManager
	{
		DECLARE_SINGLE(NetworkManager);

	public:
		void Init();
		void RegisterObject(std::shared_ptr<CGameObject> object);
		void RegisterObject(std::vector<std::shared_ptr<CGameObject>> object);

		void Connect();
		void EndThreadProcess();
		void CloseThread();

		void Recv();
		void Send(CPacket& packet);

		void SendLoginPacket();
		void SendKeyInputPacket();

		constexpr int32_t GetID() const { return m_id; }

		void AddScript(server::SCRIPT_TYPE scriptType, std::shared_ptr<MonoBehaviour> script);
		constexpr bool IsSuccessfullyLoggedIn() const { return m_login; }
	private:
		void ProcessThread();

		void Recv(DWORD bytes, OVERLAPPEDEX* pOverEx);
		void Send(DWORD bytes, OVERLAPPEDEX* pOverEx);

		void SendLogoutPacket();

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
		CPacket m_packet;
		int32_t m_remainSize;

		int32_t m_id;

		bool m_login;

		std::unordered_map<int32_t, std::vector<std::shared_ptr<CGameObject>>> m_objects;

		std::unordered_map<int8_t, std::shared_ptr<MonoBehaviour>> m_scripts;
	};
}