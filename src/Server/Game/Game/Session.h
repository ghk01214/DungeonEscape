#pragma once

#include "Object.h"
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
		CSession(CObject* obj);
		~CSession();

		void Reset();

		void Recv();
		void Send(network::CPacket& packet);

		void SendLoginPacket(int32_t id, CObject* obj);
		void SendAddPacket(int32_t id, CObject* obj);
		void SendRemovePacket(int32_t id);
		void SendMovePacket(int32_t id, ProtocolID protocol, CObject* obj);
		void SendRotatePacket(int32_t id, ProtocolID protocol, CObject* obj);
		void SendJumpPacket(int32_t id, ProtocolID protocol, CObject* obj);
		void SendAniIndexPacket(int32_t id, ProtocolID protocol, CPlayer* obj);

		const STATE GetState() const { return m_state; }
		const int32_t GetID() const { return m_id; }
		CObject* GetMyObject() { return m_pObject; }

		void SetState(STATE state) { m_state = state; }
		void SetSocket(SOCKET socket) { m_socket = socket; }
		void SetID(int32_t id) { m_id = id; }
		//void SetDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }
		void SetObject(CObject* pObject) { m_pObject = pObject; }
		void SetPos(Pos pos) { m_pObject->SetPos(pos); }
		void SetPos(float x, float y, float z) { m_pObject->SetPos(x, y, z); }

	private:
		SOCKET m_socket;

		network::OVERLAPPEDEX m_recvEx;
		network::OVERLAPPEDEX m_sendEx;

		std::atomic<STATE> m_state;
		std::atomic_int32_t m_id;

		CObject* m_pObject;

		//float m_deltaTime;
	public:
		int32_t m_prevRemain;
	};
}