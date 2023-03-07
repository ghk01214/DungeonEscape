#pragma once

namespace network
{
	class OVERLAPPEDEX;
}

namespace game
{
	class CObject;

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

		void SendLoginPacket();
		void SendMovePacket(int32_t id, uint16_t targetNum, CObject* obj);
		void SendRotatePacket(int32_t id, uint16_t targetNum, CObject* obj);

		const STATE GetState() const { return m_state; }
		constexpr uint16_t GetTargetNum() { return m_targetNum; }

		void SetSocket(SOCKET socket) { m_socket = socket; }
		void SetID(int32_t id) { m_id.store(id, std::memory_order_seq_cst); }
		void SetTargetNum(uint16_t targetNum) { m_targetNum = targetNum; }

	private:
		SOCKET m_socket;

		network::OVERLAPPEDEX m_recvEx;
		network::OVERLAPPEDEX m_sendEx;

		std::atomic<STATE> m_state;
		std::atomic_int32_t m_id;
		CObject* m_pObject;

		uint16_t m_targetNum;
	public:
		int32_t m_prevRemain;
	};
}