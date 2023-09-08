#pragma once

#include "MapObject.h"

//용도 : 다음 맵 이동 포탈 및 게임 가이드 진행 위치 표시용
//1회성은 앞에 있는 기믹 소개용으로 1회만 발동 (카메라 이동시키며 기믹 소개)
//다회성은 특정 위치에 가면 UI표시용으로 사용하던가 포탈용으로 사용한다.

class Player;

class TriggerObject2 : public MapObject
{
public:
	enum class TRIGGERATTRIBUTE
	{
		NONE = 0,

		PORTAL1,
		PORTAL2,
		PORTAL3,
		PORTAL4,
		PORTAL5,
		PORTAL6,
		PORTAL7,
		PORTAL8,

		GUIDELINE1,			// 아티팩트가 부서지지 않은 상태에서 돌기둥에 다가갔을 때의 UI
		GUIDELINE2,			// Weeper의 광역기 공격의 UI 알림
		GUIDELINE3,			// Golem의 SPELL 공격의 UI 알림
		GUIDELINE4,
		GUIDELINE5,

		CUTSCENE1,			// 아티팩트 파괴 후 보호막 사라질 때의 컷신
		CUTSCENE2,			// 돌기둥을 처음 봤을 때의 컷신
		CUTSCENE3,			// 기둥이 메테오에 의해 넘어지는 컷신
		CUTSCENE4,			// 돌 굴러가는 장면을 보여주는 컷신
		CUTSCENE5,			// Weeper 등장 컷신
		CUTSCENE6,			// Golem 등장 컷신

		END
	};
public:
	TriggerObject2(const Vec3& position, const Quat& rotation, const Vec3& scale , bool once);
	~TriggerObject2() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:			//초기화 관련 함수
	void SetTriggerAttribute(TRIGGERATTRIBUTE attrib);
	void GeometryInit(Vec3 Scale);
	void Handle_Overlap();
	void SetPortalDestination();

public:
	void AttributePortal(double timeDelta);
	void SendPlayers();
	void ThrowGimmik2Ball();
private:
	std::vector<Player*> OverlapCheck_Player();
	void Handle_OverlapOut(const std::vector<Player*>& validptr);
	bool IsPlayerDuplicate(GameObject* obj);
	bool Apply(Player* player);
	void ApplyOut(Player* player);				//apply, applyout : 진입, 진출 시 attribute에 따른 행동 서술 (기능과 서버메시지는 여기서)

private:
	void ServerInit();
	void ServerRelease();			//생성 삭제 명령만 존재.
	void ServerSendInMessage(Player* player);
	void ServerSendTriggerInMessage(Player* player, server::TRIGGER_INTERACTION_TYPE triggerType);
	void ServerSendPortalOutMessage();
	void ServerSendInteractionCountMessage();
	void ServerSendCutSceneMessage(server::CUT_SCENE_TYPE cutSceneType);

	bool CheckArtifactDestoryed();

	void AttributeGimmik2();


private:
	physx::PxBoxGeometry* m_box = nullptr;
	TRIGGERATTRIBUTE m_attribute = TRIGGERATTRIBUTE::NONE;
	bool m_oneTimeOnly = false;				//1회만 실행되는가?
	bool m_deactivate = false;				//update진입 방지 코드

	std::vector<Player*> m_duplicates;

private:
	float m_contactTime = 0.f;				//포탈용 변수. 플레이어와의 접촉 시간을 판단후 일정 시간 후 모든 플레이어를 이동시킨다.
	float m_requestedContactTime = 3.f;		//위에서 설명한 '일정 시간'.

	int m_currentCnt = 0;					//현재 겹친 인원수
	int m_beforeCnt = 0;					//위를 찾기 위한 보조변수

private:
	std::vector<Vec3> m_portalDestination;	//포탈이 이동시켜줄 위치
};

