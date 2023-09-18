#pragma once

#include "Component.h"

// 네트워크
namespace network
{
	class CNetwork : public Component
	{
	public:
		CNetwork(server::OBJECT_TYPE type = server::OBJECT_TYPE::NONE, int32_t id = -1);
		virtual ~CNetwork();

		virtual void Awake() override;
		virtual void FinalUpdate() override;

#pragma region [SEND PACKET]
		void SendAddPlayer(server::FBX_TYPE fbxType);
		void SendAniIndex(server::OBJECT_TYPE type);
		void SendPlayerQuat(const Quat& quat);
		void SendCameraLook(const Vec3& look);
		void SendAddObject(int32_t tempID, server::OBJECT_TYPE type, server::FBX_TYPE fbxType);
		void SendRemoveObject(server::OBJECT_TYPE type);
		void SendAnimationEnd(server::OBJECT_TYPE type, int32_t state);
#pragma endregion

		void InsertPackets(CPacket& packet);
		void ClearRecvQueue(int32_t size);

		constexpr uint32_t GetID() const { return m_networkID; }
		std::deque<CPacket> GetRecvPackets();
		const int32_t GetRecvQueueSize() const;

		void SetID(uint32_t id) { m_networkID = id; }

		constexpr bool IsMyPlayer() const { return m_myPlayer; }

	private:
		int32_t m_networkID;		// 오브젝트 정보를 담고 있는 ID, 초기값을 -1로 설정한다. 이는 네트워크 매니저에서 값을 채우면 변경된다.
		server::OBJECT_TYPE m_objectType;
		std::deque<CPacket> m_recvPackets;
		std::atomic_int32_t m_recvQueueSize;

		bool m_myPlayer;
	};
}