#pragma once

#include <bitset>

class CGameObject;
class MonoBehaviour;

namespace network
{
	using NetworkGameObject = std::vector<std::shared_ptr<CGameObject>>;

	struct NetworkComponent
	{
		server::OBJECT_TYPE type;
		NetworkGameObject object;

		NetworkComponent(server::OBJECT_TYPE type, NetworkGameObject object) :
			type{ type }, object{ object } {}
	};

	class NetworkManager
	{
		DECLARE_SINGLE(NetworkManager);

	public:
		void Init(const std::wstring& serverAddr);

		void Connect();
		void EndThreadProcess();
		void CloseThread();

		void Recv();
		void Send(CPacket& packet);

		void SendLoginPacket(const std::wstring& ID, const std::wstring& pwd);
		void SendKeyInputPacket();
		void SendLogoutPacket();

		void AddNetworkComponent(NetworkGameObject& object);
		void AddNetworkObject(int32_t id, NetworkGameObject& object);
		void RemoveNetworkObject(int32_t id);

		constexpr int32_t GetID() const { return m_id; }
		NetworkGameObject GetNetworkObject(int32_t id) const { return m_objects.at(id); }
		const std::wstring& GetName() const { return m_name; }

		void SetName(const std::wstring& name);

		constexpr bool IsSuccessfullyLoggedIn() const { return m_login; }
	private:
		void ProcessThread();

		void Recv(DWORD bytes, OVERLAPPEDEX* pOverEx);
		void Send(DWORD bytes, OVERLAPPEDEX* pOverEx);

#pragma region [PROCESS PACKET]
		void ProcessPacket(network::CPacket& packet);
		void ProcessAUPacket(ProtocolID type, network::CPacket& packet);
		void ProcessMYPacket(ProtocolID type, network::CPacket& packet);
		void ProcessWRPacket(ProtocolID type, network::CPacket& packet);
		void ProcessBTPacket(ProtocolID type, network::CPacket& packet);
		void ProcessIFPacket(ProtocolID type, network::CPacket& packet);
		void ProcessITPacket(ProtocolID type, network::CPacket& packet);
		void ProcessCMPacket(ProtocolID type, network::CPacket& packet);
		void ProcessECPacket(ProtocolID type, network::CPacket& packet);
		void ProcessGMPacket(ProtocolID type, network::CPacket& packet);
		void ProcessTTPacket(ProtocolID type, network::CPacket& packet);
#pragma endregion

		void RemovePlayer(int32_t id);

	private:
		HANDLE m_iocp;
		SOCKET m_socket;
		int32_t m_serverKey;
		std::wstring m_serverAddr;

		std::thread m_networkThread;

		OVERLAPPEDEX m_recvEx;
		//OVERLAPPEDEX m_sendEx;

		//int8* m_pRecvPacket;
		//CPacket m_packet;
		int32_t m_remainSize;

		int32_t m_id;
		std::wstring m_name;

		bool m_login;
		bool m_alreadyAdded;

		std::unordered_map<int32_t, NetworkGameObject> m_objects;
	};
}