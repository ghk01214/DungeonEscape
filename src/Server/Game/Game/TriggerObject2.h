#pragma once

#include "MapObject.h"

//용도 : 다음 맵 이동 포탈 및 게임 가이드 진행 위치 표시용
class Player;

class TriggerObject2 : public MapObject
{
public:
	enum class TRIGGERATTRIBUTE
	{
		NONE, 
		PORTAL1,
		PORTAL2,
		PORTAL3,
		PORTAL4,
		PORTAL5,
		PORTAL6,
		PORTAL7,
		PORTAL8,

		GUIDELINE1,
		GUIDELINE2,
		GUIDELINE3,
		GUIDELINE4,
		GUIDELINE5
	};
public:
	TriggerObject2(const Vec3& position, const Quat& rotation, const Vec3& scale , bool);
	~TriggerObject2() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void GeometryInit(Vec3 Scale);
	void Handle_Overlap();

private:
	std::vector<Player*> OverlapCheck_Player();
	void Handle_OverlapOut(const std::vector<Player*>& validptr);
	bool IsPlayerDuplicate(GameObject* obj);
	bool Apply(Player* player);

private:
	void ServerInit();
	void ServerRelease();			//생성 삭제 명령만 존재.
	void ServerSendMessage();

private:
	physx::PxBoxGeometry* m_box = nullptr;
	TRIGGERATTRIBUTE m_attribute = TRIGGERATTRIBUTE::NONE;
	bool m_oneTimeOnly = false;
	std::vector<Player*> m_duplicates;
};

