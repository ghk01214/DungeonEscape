﻿#pragma once

#include "Object.h"
#include "TempObject.h"
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
		void SendAddTempPacket(int32_t id, CTempObject* obj);
		void SendRemovePacket(int32_t id);
		void SendTransformPacket(int32_t id, ProtocolID protocol, CObject* obj);
		void SendTransformTempPacket(int32_t id, ProtocolID protocol, CTempObject* obj);
		void SendAniIndexPacket(int32_t id, ProtocolID protocol, CPlayer* obj);

		void CreateObject(int32_t objID, network::CPacket& packet);
		void AddObject(int32_t id, const std::wstring& name, TempTrans& trans);

		const STATE GetState() const { return m_state; }
		const int32_t GetID() const { return m_id; }
		//CObject* GetMyObject() { return m_pObject; }
		CTempObject* GetTempObject(int32_t id);
		HashMap<int32_t, CTempObject*>& GetTempObject() { return m_pTempObject; }

		void SetState(STATE state) { m_state = state; }
		void SetSocket(SOCKET socket) { m_socket = socket; }
		void SetID(int32_t id) { m_id = id; }
		//void SetObject(CObject* pObject) { m_pObject = pObject; }
		//void SetPos(Pos pos) { m_pObject->SetPos(pos); }
		//void SetPos(float x, float y, float z) { m_pObject->SetPos(x, y, z); }
		void SetKeyInput(uint8_t input) { m_keyInput = input; }
		void SetKeyState(server::KEY_STATE state) { m_keyState = state; }

	private:
		SOCKET m_socket;

		network::OVERLAPPEDEX m_recvEx;
		network::OVERLAPPEDEX m_sendEx;

		std::atomic<STATE> m_state;
		std::atomic_int32_t m_id;

		//HashMap<int32_t, std::vector<CObject*>> m_pObject;
		HashMap<int32_t, CTempObject*> m_pTempObject;

		uint8_t m_keyInput;
		server::KEY_STATE m_keyState;
	public:
		int32_t m_prevRemain;
	};
}