#pragma once

#include "Component.h"

// 네트워크 

class CNetwork : public Component
{
public:
	CNetwork();
	virtual ~CNetwork();

public:
	virtual void Awake() override;
	virtual void FinalUpdate() override;

public:
	void SetObjectID(uint32 id) { m_objectID = id; }
	uint32 GetObjectID(void) { return m_objectID; }


public:
	// 필요한 함수들 추가





private:
	uint32 m_objectID = -1;		// 오브젝트 정보를 담고 있는 ID, 초기값을 -1로 설정한다. 이는 네트워크 매니저에서 값을 채우면 변경된다.

private:
	// 필요한 변수들 추가
};

