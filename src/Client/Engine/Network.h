#pragma once

#include "Component.h"

// 네트워크 

namespace network
{
//	class CNetwork : public Component
//	{
//	public:
//		CNetwork();
//		virtual ~CNetwork();
//
//		virtual void Awake() override;
//		virtual void FinalUpdate() override;
//
//#pragma region [SEND PACKET]
//		void SendLoginPacket();
//		void SendMovePacket(DIRECTION direction);
//		void SendRotationPacket(ROTATION direction);
//#pragma endregion
//
//		uint32_t GetObjectID(void) { return m_objectID; }
//
//		void SetObjectID(uint32_t id) { m_objectID = id; }
//
//	private:
//		uint32_t m_objectID;		// 오브젝트 정보를 담고 있는 ID, 초기값을 -1로 설정한다. 이는 네트워크 매니저에서 값을 채우면 변경된다.
//	};
}