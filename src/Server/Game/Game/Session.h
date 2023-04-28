#pragma once

#include "GameObject.h"
#include "Player.h"

namespace network
{
	class OVERLAPPEDEX;
}

namespace game
{
	enum class STATE
	{
		NONE = 0,
		FREE,
		ACCEPTED,
		INGAME,
		MAX
	};

	class CSession
	{
	public:
		CSession();
		CSession(GameObject* obj);
		~CSession();

		void Reset();

		void Recv();
		void Send(network::CPacket& packet);

		void SendLoginPacket(GameObject* obj);
		void SendAddPacket(int32_t id, GameObject* obj);
		void SendRemovePacket(int32_t id);
		void SendTransformPacket(int32_t id, ProtocolID protocol, GameObject* obj);
		//void SendAniIndexPacket(int32_t id, ProtocolID protocol, CPlayer_OLD* obj);

		const STATE GetState() const { return m_state; }
		const int32_t GetID() const { return m_id; }
		GameObject* GetMyObject() { return m_pObject; }

		void SetState(STATE state) { m_state = state; }
		void SetSocket(SOCKET socket) { m_socket = socket; }
		void SetID(int32_t id) { m_id = id; }
		void SetObject(GameObject* pObject) { m_pObject = pObject; }

	private:
		SOCKET m_socket;

		network::OVERLAPPEDEX m_recvEx;
		network::OVERLAPPEDEX m_sendEx;

		std::atomic<STATE> m_state;
		std::atomic_int32_t m_id;

		GameObject* m_pObject;
	public:
		int32_t m_prevRemain;
	};
}