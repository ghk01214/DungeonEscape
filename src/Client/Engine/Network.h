#pragma once

#include "Component.h"

// 네트워크
namespace network
{
	class CNetwork : public Component
	{
	public:
		CNetwork(OBJECT_TYPE type = OBJECT_TYPE::NONE, int32_t id = -1);
		virtual ~CNetwork();

		virtual void Awake() override;
		virtual void FinalUpdate() override;

#pragma region [SEND PACKET]
		void SendAddPlayer();
		void SendAniIndexPacket(int32_t index);
		void SendCameraLook(const Vec3& look);
#pragma endregion

		void InsertPackets(CPacket& packet);
		void ClearRecvQueue(int32_t size);

		constexpr uint32_t GetID() const { return m_networkID; }
		const std::deque<CPacket> GetRecvPackets() const;
		const int32_t GetRecvQueueSize() const;

		void SetID(uint32_t id) { m_networkID = id; }

		constexpr bool IsMyPlayer() const { return m_myPlayer; }

	private:
		int32_t m_networkID;		// 오브젝트 정보를 담고 있는 ID, 초기값을 -1로 설정한다. 이는 네트워크 매니저에서 값을 채우면 변경된다.
		OBJECT_TYPE m_objectType;
		std::deque<CPacket> m_recvPackets;
		std::atomic_int32_t m_recvQueueSize;

		bool m_myPlayer;
	};
}