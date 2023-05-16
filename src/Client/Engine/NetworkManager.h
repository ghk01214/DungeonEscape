#pragma once

#include <bitset>

class CGameObject;
class MonoBehaviour;

namespace network
{
	using NetworkGameObject = std::vector<std::shared_ptr<CGameObject>>;

	enum class OBJECT_TYPE
	{
		NONE = 0,

		PLAYER,
		REMOTE_PLAYER,
		OBJECT,

		MAX
	};

	struct NetworkComponent
	{
		OBJECT_TYPE type;
		NetworkGameObject object;

		NetworkComponent(OBJECT_TYPE type, NetworkGameObject object) :
			type{ type }, object{ object } {}
	};

	class NetworkManager
	{
		DECLARE_SINGLE(NetworkManager);

	public:
		void Init(const std::wstring& serverAddr);
		void RegisterObject(OBJECT_TYPE type, std::shared_ptr<CGameObject> object);
		void RegisterObject(OBJECT_TYPE type, NetworkGameObject object);

		void Connect();
		void EndThreadProcess();
		void CloseThread();

		void Recv();
		void Send(CPacket& packet);

		void SendLoginPacket();
		void SendKeyInputPacket();
		void SendLogoutPacket();
		void SendIDIssueRequest();

		void AddRemoteObject(int32_t id, NetworkGameObject& object);

		constexpr int32_t GetID() const { return m_id; }

		constexpr bool IsSuccessfullyLoggedIn() const { return m_login; }
	private:
		void ProcessThread();

		void Recv(DWORD bytes, OVERLAPPEDEX* pOverEx);
		void Send(DWORD bytes, OVERLAPPEDEX* pOverEx);

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
		std::wstring m_serverAddr;

		std::thread m_networkThread;

		OVERLAPPEDEX m_recvEx;
		OVERLAPPEDEX m_sendEx;

		int8* m_pRecvPacket;
		CPacket m_packet;
		int32_t m_remainSize;

		int32_t m_id;

		bool m_login;

		std::unordered_map<int32_t, NetworkGameObject> m_objects;
		std::list<NetworkComponent> m_unregisterdObjects;
	};
}