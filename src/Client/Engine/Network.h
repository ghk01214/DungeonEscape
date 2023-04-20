#pragma once

#include "Component.h"

// 네트워크
namespace network
{
	class CNetwork : public Component
	{
	public:
		CNetwork();
		virtual ~CNetwork();

		virtual void Awake() override;
		virtual void FinalUpdate() override;

#pragma region [SEND PACKET]
		void SendAddObjectPacket();
		void SendTransformPacket(uint8_t key, server::KEY_STATE state);
		void SendAniIndexPacket(int32_t index, float updateTime);
#pragma endregion

		constexpr uint32_t GetID(void) const { return m_networkID; }

		void SetID(uint32_t id) { m_networkID = id; }

	private:
		int32_t m_networkID;		// 오브젝트 정보를 담고 있는 ID, 초기값을 -1로 설정한다. 이는 네트워크 매니저에서 값을 채우면 변경된다.
	};
}