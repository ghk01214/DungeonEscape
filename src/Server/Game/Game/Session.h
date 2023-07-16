﻿#pragma once

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
		CSession(Player* obj);
		~CSession();

		void Reset();

		void Recv();
		void Send(network::CPacket& packet);

		void SendLoginPacket(Player* obj);
		void SendPlayerIDIssuePacket(int32_t id, ProtocolID protocol);
		void SendAddAnimateObjPacket(GameObject* obj);
		void SendAddObjPacket(GameObject* obj);
		void SendObjectIDPacket(int32_t objID, int32_t oldObjID);
		void SendRemovePacket(int32_t id, server::OBJECT_TYPE type);
		void SendPlayerTransformPacket(GameObject* obj);
		void SendTransformPacket(GameObject* obj);
		void SendStatePacket(int32_t id, int32_t stateIndex);

		const STATE GetState() const { return m_state; }
		const int32_t GetID() const { return m_id; }
		GameObject* GetMyObject() { return m_pObject; }
		constexpr int32_t GetRoomID() const { return m_roomID; }

		void SetState(STATE state) { m_state = state; }
		void SetSocket(SOCKET socket) { m_socket = socket; }
		void SetID(int32_t id) { m_id = id; }
		void SetPlayer(Player* pObject) { m_pObject = pObject; }
		void SetRoomID(int32_t id) { m_roomID = id; }

	private:
		SOCKET m_socket;

		network::OVERLAPPEDEX m_recvEx;
		network::OVERLAPPEDEX m_sendEx;

		std::atomic<STATE> m_state;
		std::atomic_int32_t m_id;
		int32_t m_roomID;

		Player* m_pObject;
	public:
		int32_t m_prevRemain;
	};
}