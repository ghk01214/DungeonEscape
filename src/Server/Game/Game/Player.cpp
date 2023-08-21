#include "pch.h"
#include "Player.h"
#include "CustomController.h"
#include "MessageHandler.h"
#include "ObjectManager.h"
#include "TimeManager.h"
#include "RigidBody.h"
#include "Transform.h"
#include "SkillObject.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "BoxCollider.h"
#include "TriggerObject.h"
#include "OverlapObject2.h"
#include "EventHandler.h"
#include "MonsterAI.h"
#include "Monster.h"

using namespace std;

Player::Player(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_prevState{ IDLE1 },
	m_currState{ m_prevState },
	m_aniPlayTime{ 0.f },
	m_aniEnd{ false },
	m_hp{ 20 },
	m_mp{ 100 },
	m_firstSingleStrike{ true },
	m_prevOnGround{ false },
	m_sendState{ false },
	m_cameraDistance{ 381.448f }
{
	m_id = playerID;
}

Player::~Player()
{
}

void Player::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController", true);

	std::string test = "Player";
	m_controller->GetBody()->SetName(test);

	auto body = m_controller->GetBody();
	body->SetMass(body->GetMass() * 0.7f);
	SetObjectType(server::OBJECT_TYPE::PLAYER);

	m_controller->GetBody()->GetCollider(0)->ApplyModifiedLayer(PhysicsLayers::PLAYER, PhysicsLayers::SKILLOBJECT_PLAYER);
	m_controller->SetJumpSpeed(70.f);
	m_controller->SetMoveSpeed(40.f);

	for (int32_t i = magic_enum::enum_integer(server::TRIGGER_TYPE::NONE) + 1; i < magic_enum::enum_integer(server::TRIGGER_TYPE::MAX); ++i)
	{
		m_triggerDictionary[magic_enum::enum_value<server::TRIGGER_TYPE>(i)] = false;
	}

	m_overlapObj = new OverlapObject2(this);
	m_overlapObj->Init();

}

void Player::Update(double timeDelta)
{
	//SendChangedStateAgain();

	CalculateCameraDistance();

	KeyboardLimit();					//FSM 0단계 : 금지상태에서는 키보드 클리어

	ChangeStateByKeyInput();			//FSM 1단계	: 키보드 입력에 의한 STATE변경 (이동, 점프, 공격) STATE가 자유롭지 못할 시 진입X

	TriggerZoneStatusUpdate();			//FSM 2단계	: 위부 요인에 의한 STATE변경 (판단 순서 : MP부족 > 피격 > HP부족)


	IsOnGround();						//FSM 3단계	: 공중모션 판단

	SkillAttempt();						//FSM 4단계 : STATE_CHECK직전 state가 공격이라면 마나상태에 따라 ATK or TIRED로 진입.
	DeathCheck();						//			: IDLE상태에서 HP0면 state를 DIE0으로													//최종 STATE

	PlayerMove();						//			: controller를 사용해 이동한다. state가 move/jump가 아닐 시 키보드 정보 삭제
	StunCheck();						//			몬스터에 의한 기절

	State_Check_Enter();				//FSM 5단계	: 최종적으로 정해진 State진입에 대한 1회성 행동처리(공격함수 호출, 사운드 출력 등..)		//최종 STATE에 대한 처리
	Update_Frame_Continuous();			//			: 현재 State에 대한 지속적 처리 (현재는 없음)
	Update_Frame_Once();				//			: 애니메이션 종료에 대한 State재정의 (eg. Damaged > IDLE or DIE0)

	PlayerPattern_ATTACK_ForDebug();

	if (m_overlapObj)
		m_overlapObj->Update();

	

	GameObject::Update(timeDelta);

	ServerSendTransformMessage();
}

void Player::LateUpdate(double timeDelta)
{
	m_controller->ClearControllerCollisionInfo();
	m_transform->ConvertPX(m_controller->GetBody()->GetGlobalPose());
	m_transform->SetRotation(m_qlook);
}

void Player::Release()
{
	m_banTriggerApproach = true;

	m_controller = nullptr;
	SafeRelease(m_overlapObj);
	GameObject::Release();
}

void Player::IsOnGround()
{
	switch (m_currState)
	{
		case DAMAGE: case DEAD:
		case DIE0: case DIE1: case DIE2:
		return;
		default:
		break;
	}

	auto distance{ m_controller->GetDistanceFromGround() };

	if (IsEqual(distance, -1.f) == true or distance > 20.f)
	{
		if (m_currState == JUMP_START)
			return;
		m_currState = JUMPING;
	}
}

bool Player::IsDead()
{
	return m_hp <= 0;
}

void Player::TriggerZoneStatusChange(server::TRIGGER_TYPE triggerType, bool status)
{
	//trigger enter/exit에 대한 처리
	if (triggerType != server::TRIGGER_TYPE::NONE)
	{
		m_triggerDictionary[triggerType] = status;
	}


	//마그마블럭 진입/탈출
	if (triggerType == server::TRIGGER_TYPE::MAGMA)
	{
		if (status)
		{
			m_controller->SetMoveSpeed(20.f);		//default:30
			m_controller->SetJumpSpeed(50.f);		//default:70
		}
		else
		{
			m_controller->SetMoveSpeed(30.f);
			m_controller->SetJumpSpeed(70.f);
		}
	}
}

void Player::TriggerZoneStatusUpdate()
{
	//영역 내 지속적인 효과를 주는 트리거 상호작용만 포함
	//1회성 트리거(타격)의 경우 TriggerObject 클래스에서 플레이어의 status를 변경하는 방식으로 사용
	Trigger_Magma();
	Trigger_Wind();
}

void Player::Trigger_Magma()
{
	//특정 시간마다 데미지 부여(클라)
}

void Player::Trigger_Wind()
{
	if (!(m_triggerDictionary[server::TRIGGER_TYPE::WIND_FRONT] || m_triggerDictionary[server::TRIGGER_TYPE::WIND_BACK] ||
		m_triggerDictionary[server::TRIGGER_TYPE::WIND_LEFT] || m_triggerDictionary[server::TRIGGER_TYPE::WIND_RIGHT] ||
		m_triggerDictionary[server::TRIGGER_TYPE::WIND_UP] || m_triggerDictionary[server::TRIGGER_TYPE::WIND_DOWN]))
		return;

	if (m_triggerDictionary[server::TRIGGER_TYPE::WIND_UP])
	{
		m_controller->GetBody()->AddForce(ForceMode::Impulse, physx::PxVec3(0, 1.f, 0) * 1.f);
	}
}

Player::PLAYER_STATE Player::GetState() const
{
	return m_currState;
}

float Player::GetAniPlayTime() const
{
	return m_aniPlayTime;
}

void Player::SetState(PLAYER_STATE state)
{
	m_currState = state;
}

void Player::SetState(int32_t index)
{
	m_currState = magic_enum::enum_value<PLAYER_STATE>(index);
}

void Player::SetAniPlayTime(float time)
{
	m_aniPlayTime = time;
}

void Player::SetAniEndFlag(bool flag)
{
	m_aniEnd = flag;
}

void Player::SetPlayerQuat(Quat& quat)
{
	m_qlook = quat;
}

void Player::GetDamaged(int32_t damage)
{
	m_hp -= damage;
}

void Player::CalculateCameraDistance()
{
	m_cameraDistance = m_controller->CalculateCameraDistance();
}

void Player::SendChangedStateAgain()
{
	if (m_sendState <= 0)
		return;

	game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_STATE_ACK, m_id };
	ev.state = m_currState;
	ev.objType = m_objType;

	game::MessageHandler::GetInstance()->PushSendMessage(ev);

	--m_sendState;
}

void Player::KeyboardLimit()
{
	switch (m_currState)
	{
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		case DAMAGE: case DEAD:
		case DIE0: case DIE1: case DIE2:
		{
			m_controller->Keyboard_ATK_Clear();
			m_controller->Keyboard_Direction_Clear();
			m_controller->Keyboard_SpaceBar_Clear();
		}
		return;
		case JUMP_START: case JUMPING:
		{
			m_controller->Keyboard_ATK_Clear();
		}
		return;
		default:
		break;
	}
}

void Player::ChangeStateByKeyInput()
{
	switch (m_currState)
	{
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		case JUMP_START: case JUMPING: case DAMAGE: case DEAD:
		case DIE0: case DIE1: case DIE2:
		return;
		default:
		break;
	}

	auto key{ m_controller->GetKeyInput() };

#pragma region 방향처리
	if (key[W].press or key[A].press or key[S].press or key[D].press)
	{
		m_currState = MOVE;
	}
	else if (!key[W].press and !key[S].press and !key[A].press and !key[D].press
		and !key[W].down and !key[S].down and !key[A].down and !key[D].down)
	{
		m_currState = IDLE1;
	}
#pragma endregion

#pragma region [점프처리]
	if (key[SPACE].down and m_controller->IsOnGround())
	{
		m_currState = JUMP_START;
		return;			// 점프면 바로 함수 종료
	}
#pragma endregion

#pragma region [스킬처리]
	if (key[KEY_1].down)
	{
		m_currState = ATK0;
		m_controller->Keyboard_Direction_Clear();
		m_controller->Keyboard_SpaceBar_Clear();
	}
	else if (key[KEY_2].down)
	{
		m_currState = ATK1;
		m_controller->Keyboard_Direction_Clear();
		m_controller->Keyboard_SpaceBar_Clear();
	}
	else if (key[KEY_3].down)
	{
		m_currState = ATK2;
		m_controller->Keyboard_Direction_Clear();
		m_controller->Keyboard_SpaceBar_Clear();
	}
	else if (key[KEY_4].down)
	{
		m_currState = ATK3;
		m_controller->Keyboard_Direction_Clear();
		m_controller->Keyboard_SpaceBar_Clear();
	}
#pragma endregion
}

void Player::State_Check_Enter()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)				//state최초진입 행동정의
	{
		case AERT:
		{

		}
		break;
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		{
			if (m_fbxType == server::FBX_TYPE::NANA)
				PlayerPattern_SingleStrike(m_currState);
			else if (m_fbxType == server::FBX_TYPE::MISTIC)
				PlayerPattern_ShootBall();
			else if (m_fbxType == server::FBX_TYPE::CARMEL)
				std::cout << "카르멜도 패턴 추가해야함" << std::endl;
		}
		break;
		case BLOCK:
		{

		}
		break;
		case DAMAGE:
		{

		}
		break;
		case DASH:
		{

		}
		break;
		case DIE0:
		{

		}
		break;
		case DIE1:
		{

		}
		break;
		case DIE2:
		{

		}
		break;
		case IDLE1:
		{
		}
		break;
		case IDLE2:
		{

		}
		break;
		case IDLE3:
		{

		}
		break;
		case JUMP_START:
		{
		}
		break;
		case MOVE:
		{

		}
		break;
		case MOVE_LEFT:
		{

		}
		break;
		case MOVE_RIGHT:
		{

		}
		break;
		case REST:
		{

		}
		break;
		case RUN:
		{

		}
		break;
		case RUN_LEFT:
		{

		}
		break;
		case RUN_RIGHT:
		{

		}
		break;
		case SHOOT:
		{

		}
		break;
		case SLEEP:
		{

		}
		break;
		case SWOON:
		{

		}
		break;
		case TIRED:
		{

		}
		break;
		case VICTORY1:
		{

		}
		break;
		case VICTORY2:
		{

		}
		break;
		case WALK:
		{

		}
		break;
		case WALK_LEFT:
		{

		}
		break;
		case WALK_RIGHT:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;
	m_sendState = SEND_AGAIN;

	for (int32_t i = 0; i < SEND_AGAIN; ++i)
	{
		game::TIMER_EVENT ev{ ProtocolID::WR_CHANGE_STATE_ACK, m_id };
		ev.state = m_currState;
		ev.objType = m_objType;

		game::MessageHandler::GetInstance()->PushSendMessage(ev);
	}
}

void Player::Update_Frame_Continuous()
{
	switch (m_currState)				//연속된 애니메이션의 주기마다의 처리
	{
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		case DAMAGE: case DIE0: case DIE1: case DIE2: case DEAD:
		case JUMP_START: case JUMPING: case JUMP_END:
		case SHOOT: case SLEEP: case SWOON: case TIRED:
		return;
		default:
		break;
	}
}

void Player::Update_Frame_Once()
{
	switch (m_currState)
	{
		case IDLE1: case IDLE2: case IDLE3:
		case MOVE: case MOVE_LEFT: case MOVE_RIGHT:
		case RUN: case RUN_LEFT: case RUN_RIGHT:
		case WALK: case WALK_LEFT: case WALK_RIGHT:
		case VICTORY1: case VICTORY2: case DEAD:
		return;
		default:
		break;
	}

	if (m_controller->IsOnGroundByDistance() == true)
	{
		if (m_currState == JUMP_START || m_currState == JUMPING)
		{
			if (m_controller->Falling())
			{
				m_currState = IDLE1;
			}
		}
	}

	if (m_aniEnd == false)
		return;

	switch (m_currState)
	{
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		{
			std::cout << "공격 종료\n";
			m_currState = IDLE1;
		}
		break;
		case DAMAGE:
		{
			m_currState = IDLE1;
		}
		break;
		case DIE0: case DIE1: case DIE2:
		{
			m_currState = DEAD;
		}
		break;
		case JUMP_START:
		{
			m_currState = JUMPING;
		}
		case JUMP_END:
		{
			m_currState = IDLE1;
		}
		break;
		case SHOOT:
		{
			m_currState = IDLE1;
		}
		break;
		case SLEEP:
		{
			m_currState = IDLE1;
		}
		break;
		case SWOON:
		{
			m_currState = IDLE1;
		}
		break;
		case TIRED:
		{
			m_currState = IDLE1;
		}
		break;
		default:
		break;
	}

	m_aniEnd = false;
}

void Player::PlayerMove()
{
	if (m_currState != MOVE and m_currState != JUMP_START and m_currState != JUMPING)
		m_controller->Keyboard_Direction_Clear();

	if (m_stun)
	{
		m_controller->Keyboard_Direction_Clear();
		m_controller->Keyboard_SpaceBar_Clear();
	}

	m_controller->PlayerMove();
}

void Player::SkillAttempt()
{
	std::vector<int32_t> requiredMana;
	requiredMana.resize(4);

	//직업별 필요 마나 설정
	if (m_fbxType == server::FBX_TYPE::NANA)				//전사
	{
		requiredMana[0] = 10;
		requiredMana[1] = 20;
		requiredMana[2] = 30;
		requiredMana[3] = 40;
	}
	else if (m_fbxType == server::FBX_TYPE::MISTIC)				//법사
	{
		requiredMana[0] = 10;
		requiredMana[1] = 20;
		requiredMana[2] = 30;
		requiredMana[3] = 40;
	}
	else if (m_fbxType == server::FBX_TYPE::CARMEL)				//힐러
	{
		requiredMana[0] = 10;
		requiredMana[1] = 20;
		requiredMana[2] = 30;
		requiredMana[3] = 40;
	}

	// state(스킬)과 마나 비교
	if (m_currState == ATK0 and m_mp < requiredMana[0])
	{
		m_currState = TIRED;
	}
	if (m_currState == ATK1 and m_mp < requiredMana[1])
	{
		m_currState = TIRED;
	}
	if (m_currState == ATK2 and m_mp < requiredMana[2])
	{
		m_currState = TIRED;
	}
	if (m_currState == ATK3 and m_mp < requiredMana[3])
	{
		m_currState = TIRED;
	}
}

void Player::DeathCheck()
{
	if (m_currState == DIE0)					//반복호출을 막는다. 계속 DIE0이면 DEAD로 진행이 불가능하다.
		return;

	if (m_currState == IDLE1 and m_hp <= 0)		//IDLE상태일때만 죽음으로 보내버린다.
		m_currState = DIE0;
}

void Player::SetStun(bool value)
{
	m_stun = value;
	if (m_stun)
		cout << "stun TRUE" << endl;
	else
		cout << "stun FALSE" << endl;
}

void Player::StunCheck()
{
	if (m_currState == DEAD || m_currState == JUMPING || m_currState == JUMP_START || m_currState == JUMP_END)
		return;
	else
	{
		if (m_stun)
		{
			m_currState = TIRED;
			m_controller->Keyboard_ATK_Clear();
			m_controller->Keyboard_Direction_Clear();
			m_controller->Keyboard_SpaceBar_Clear();
		}
	}
}

Vec3 Player::LocationForBilboard_VictimPlayer(MonsterAI* overlapOwner)
{
	auto monster = overlapOwner->GetMonster();

	physx::PxVec3 fakeVariable(0);
	physx::PxVec3 monsterPos = TO_PX3(monster->GetControllerPosition());
	physx::PxVec3 playerPos = TO_PX3(GetControllerPosition());

	physx::PxVec3 dir = playerPos - monsterPos;

	dir.normalize();


	playerPos += (dir * 100.f);
	fakeVariable = playerPos;

	Vec3 tempPos;
	tempPos.x = playerPos.x;
	tempPos.y = playerPos.y;
	tempPos.z = playerPos.z;


	return tempPos;
}

Vec3 Player::LocationForBilboard_VictimMonster(Monster* monster)
{
	physx::PxVec3 fakeVariable(0);
	physx::PxVec3 monsterPos = TO_PX3(monster->GetControllerPosition());
	physx::PxVec3 playerPos = TO_PX3(GetControllerPosition());

	physx::PxVec3 dir = monsterPos - playerPos;

	dir.normalize();


	monsterPos += (dir * -200.f);
	fakeVariable = playerPos;

	Vec3 tempPos;
	tempPos.x = monsterPos.x;
	tempPos.y = monsterPos.y;
	tempPos.z = monsterPos.z;


	return tempPos;
}



void Player::SetControllerMoveSpeed(float value)
{
	m_controller->SetMoveSpeed(value);
}

void Player::SetControllerPosition(Vec3 pos)
{
	m_controller->GetBody()->SetPosition(pos, true);
}

Vec3 Player::GetControllerPosition()
{
	return FROM_PX3(m_controller->GetBody()->GetPosition());
}

Quat Player::GetControllerRotation()
{
	return FROM_PXQUAT(m_controller->GetBody()->GetRotation());
}

float Player::GetControllerMoveSpeed()
{
	return m_controller->GetMoveSpeed();
}

void Player::SetControllerJumpSpeed(float value)
{
	m_controller->SetJumpSpeed(value);
}

float Player::GetControllerJumpSpeed()
{
	return m_controller->GetJumpSpeed();
}

float Player::GetCameraDistance()
{
	return m_cameraDistance;
}

CustomController* Player::GetController()
{
	return m_controller;
}

void Player::SetControllerCameraLook(Vec3& value)
{
	m_controller->CameraLookReceive(value);
}

void Player::PlayerPattern_ShootBall()
{
	SkillObject::SKILLOBJECTTYPE skilltype;
	switch (m_currState)
	{
		case ATK0:
		{
			skilltype = SkillObject::SKILLOBJECTTYPE::PLAYER_FIREBALL;
		}
		break;
		case ATK1:
		{
			skilltype = SkillObject::SKILLOBJECTTYPE::PLAYER_ICEBALL;
		}
		break;
		case ATK2:
		{
			skilltype = SkillObject::SKILLOBJECTTYPE::PLAYER_THUNDERBALL;
		}
		break;
		case ATK3:
		{
			if (m_meteorAvailable)
			{
				m_meteorAvailable = false;
				PlayerPattern_ShootMeteor();
				EventHandler::GetInstance()->AddEvent("METEOR_AVAILABLE_SET_TRUE", 5.f, this);
				m_controller->Keyboard_ATK_Clear();
				return;
			}

			skilltype = SkillObject::SKILLOBJECTTYPE::PLAYER_METEOR;
		}
		break;
		default:
		break;
	}

	//투사체 위치 선정
	physx::PxVec3 playerPos = m_controller->GetBody()->GetGlobalPose().p;
	physx::PxVec3 playerCameraLook = m_controller->GetCameraLook().getNormalized();

	float playerRadius = m_controller->GetCollider()->GetRadius();
	float skillBallHalfExtent = 50.f;


	// 07/23 추가한 코드. 방향수정이 잘되면 기존 위의 코드는 정리한다.
	playerCameraLook = -GetForwardVec();


	physx::PxVec3 skillBallPosition = playerPos + playerCameraLook * (playerRadius + skillBallHalfExtent + 10);
	Vec3 ballPos = FROM_PX3(skillBallPosition);

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");
	auto skillObject = objmgr->AddGameObjectToLayer<SkillObject>
		(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent), skilltype, nullptr, this);
	skillObject->PlayerSkillFire(playerCameraLook);

	// KeyUp 상태가 전달되기까지의 딜레이가 있어서 로직 종료 시 key 상태 변경
	m_controller->Keyboard_ATK_Clear();
}

void Player::PlayerPattern_ShootMeteor()
{
	auto pillarlist = ObjectManager::GetInstance()->GetLayer(L"Layer_Gimmik_Pillar")->GetGameObjects();
	GameObject* pillar =nullptr;
	for (auto& t : pillarlist)
	{
		pillar = t;
	}

	SkillObject::SKILLOBJECTTYPE skilltype = SkillObject::SKILLOBJECTTYPE::PLAYER_METEOR;

	Vec3 ballPos(3645.371, 800.000, 21171.279);
	bool randX = rand() % 2 == 0;
	if (randX)
		ballPos.z += 400.f;
	else
		ballPos.z -= 400.f;

	//투사체 생성
	auto objmgr = ObjectManager::GetInstance();
	auto layer = objmgr->GetLayer(L"Layer_SkillObject");
	auto skillObject = objmgr->AddGameObjectToLayer<SkillObject>
		(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(300,300,300), skilltype, nullptr, this);
	skillObject->SetAttribute(SkillObject::SKILLATTRIBUTE::GUIDED_METEOR, true);
	skillObject->SetAttribute(SkillObject::SKILLATTRIBUTE::LEVITATE, true);
	if(pillar)
		skillObject->m_target = pillar;
	auto meteorBody = skillObject->GetComponent<RigidBody>(L"RigidBody");
	auto meteorCollider = meteorBody->GetCollider(0);
	meteorCollider->ApplyModifiedLayer(PhysicsLayers::SKILLOBJECT_PLAYER, PhysicsLayers::MAP);

	meteorCollider->SetRestitution(0.3f);
	meteorCollider->SetRestitutionCombineMode(PhysicsCombineMode::Max);


	// KeyUp 상태가 전달되기까지의 딜레이가 있어서 로직 종료 시 key 상태 변경
	m_controller->Keyboard_ATK_Clear();
}

void Player::PlayerPattern_ATTACK_ForDebug()
{
	static bool wasKDown = false; // static 변수 추가. 이전에 'K'키가 눌려져 있었는지를 기억합니다.

	bool isKDown = GetAsyncKeyState('K') & 0x8000; // 'K'키가 현재 눌려져 있는지 확인합니다.

	bool ball = true;


	if (wasKDown && !isKDown) // 이전에 'K'키가 눌려져 있었는데, 지금은 떼어져 있다면.
	{
		if (ball)
		{
			SkillObject::SKILLOBJECTTYPE skilltype;

			skilltype = SkillObject::SKILLOBJECTTYPE::PLAYER_FIREBALL;

			//투사체 위치 선정
			physx::PxVec3 playerPos = m_controller->GetBody()->GetGlobalPose().p;
			physx::PxVec3 playerCameraLook = m_controller->GetCameraLook().getNormalized();

			float playerRadius = m_controller->GetCollider()->GetRadius();
			float skillBallHalfExtent = 50.f;

			physx::PxVec3 skillBallPosition = playerPos + playerCameraLook * (playerRadius + skillBallHalfExtent + 10);
			Vec3 ballPos = FROM_PX3(skillBallPosition);

			//투사체 생성
			auto objmgr = ObjectManager::GetInstance();
			auto layer = objmgr->GetLayer(L"Layer_SkillObject");
			auto skillObject = objmgr->AddGameObjectToLayer<SkillObject>
				(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent), skilltype, nullptr, this);
			skillObject->PlayerSkillFire(playerCameraLook);

			//PlayerPattern_ShootMeteor();
			//PlayerPattern_SingleStrike(ATK0);
		}
		else
		{
			PlayerPattern_SingleStrike(ATK0);
		}

		//PlayerPattern_ShootMeteor();
	}

	wasKDown = isKDown; // 'K'키가 눌려져 있는지의 상태를 업데이트합니다.
}

void Player::PlayerPattern_SingleStrike(PLAYER_STATE state)
{
#pragma region 옛코드
	//if (m_banTriggerApproach)
	//	return;
	//
	//float attackTime[2] = {};
	//
	//switch (state)
	//{
	//	case ATK0:
	//	{
	//		attackTime[0] = 0.2f;
	//		attackTime[1] = 0.43f;
	//	}
	//	break;
	//	case ATK1:
	//	{
	//		attackTime[0] = 0.2f;
	//		attackTime[1] = 0.39f;
	//	}
	//	break;
	//	case ATK2:
	//	{
	//		attackTime[0] = 0.08f;
	//		attackTime[1] = 0.19f;
	//	}
	//	break;
	//	case ATK3:
	//	{
	//		attackTime[0] = 0.6f;
	//		attackTime[1] = 0.75f;
	//	}
	//	break;
	//	case ATK4:
	//	{
	//		attackTime[0] = 0.08f;
	//		attackTime[1] = 0.27f;
	//	}
	//	break;
	//	default:
	//	break;
	//}
	////공격 활성화 시간
	//
	//auto objmgr = ObjectManager::GetInstance();
	//physx::PxVec3 playerPos = TO_PX3(GetControllerPosition());				//플레이어 위치
	//physx::PxVec3 cameraDir = m_controller->GetCameraLook();
	//cameraDir.y = 0;  // y 컴포넌트를 0으로 설정
	//cameraDir.normalize();  // 벡터를 정규화
	//float angle = physx::PxAtan2(cameraDir.z, cameraDir.x);
	//physx::PxQuat cameraRot(angle, physx::PxVec3(0, 1, 0));					//카메라 회전정보



	//// 07/23 추가한 코드. 방향수정이 잘되면 기존 위의 코드는 정리한다.
	//cameraDir = GetForwardVec();



	//float radius = m_controller->GetCollider()->GetRadius();
	//float extent = 100.f;
	//physx::PxVec3 triggerPos = playerPos + cameraDir * (radius + extent);	//트리거 위치

	//if (m_firstSingleStrike == true)
	//{
	//	m_attackTrigger = objmgr->AddGameObjectToLayer<TriggerObject>(L"Layer_TriggerObject", FROM_PX3(triggerPos), FROM_PXQUAT(cameraRot), Vec3(extent, extent, extent));
	//	m_attackTrigger->SetTriggerType(server::TRIGGER_TYPE::SINGLE_STRIKE, attackTime[0], attackTime[1]);
	//	auto triggerBody = m_attackTrigger->GetComponent<RigidBody>(L"RigidBody");
	//	triggerBody->AddCollider<BoxCollider>(m_attackTrigger->GetTransform()->GetScale());
	//	triggerBody->GetCollider(0)->SetTrigger(true);
	//	m_firstSingleStrike = false;
	//}
	//else
	//{
	//	m_attackTrigger->RestoreOneTimeEffect();
	//	auto triggerBody = m_attackTrigger->GetComponent<RigidBody>(L"RigidBody");
	//	triggerBody->SetPosition(FROM_PX3(triggerPos), true);
	//}

#pragma endregion

	//투사체 위치 선정
	physx::PxVec3 playerPos = m_controller->GetBody()->GetGlobalPose().p;
	physx::PxVec3 xzDir = m_controller->GetCameraLook().getNormalized();

	float playerRadius = m_controller->GetCollider()->GetRadius();
	float skillHalfExtent = 100.f;

	xzDir = -GetForwardVec();

	physx::PxVec3 pos = playerPos + xzDir * (playerRadius + skillHalfExtent + 10);
	physx::PxQuat rot = GetRotation_For_Overlap(xzDir);


	m_overlapObj->UpdateOverlapPosition(pos, rot);

	float start = 0.f;
	float end = 0.f;
	switch (state)
	{
		case ATK0:
		{
			start = 0.2f;
			end = 0.43f;
		}
		break;
		case ATK1:
		{
			start = 0.2f;
			end = 0.39f;
		}
		break;
		case ATK2:
		{
			start = 0.08f;
			end = 0.19f;
		}
		break;
		case ATK3:
		{
			start = 0.6f;
			end = 0.75f;
		}
		break;
		case ATK4:
		{
			start = 0.08f;
			end = 0.27f;
		}
	}

	EventHandler::GetInstance()->AddEvent("PLAYER_OVERLAPOBJ_ACTIVATE", start, this);
	EventHandler::GetInstance()->AddEvent("PLAYER_OVERLAPOBJ_DEACTIVATE", end, this);

	if(m_currState == PLAYER_STATE::ATK0)
		m_overlapObj->UpdateOverlapType("ATTACK0");
	else if (m_currState == PLAYER_STATE::ATK1)
		m_overlapObj->UpdateOverlapType("ATTACK1");
	else if (m_currState == PLAYER_STATE::ATK2)
		m_overlapObj->UpdateOverlapType("ATTACK2");
	else if (m_currState == PLAYER_STATE::ATK3)
		m_overlapObj->UpdateOverlapType("ATTACK3");

	m_controller->Keyboard_ATK_Clear();
}

int32_t Player::IsAttackKeyDown()
{
	for (int32_t i = KEY_1; i <= KEY_4; ++i)
	{
		if (m_controller->GetKeyStatus(i).down == true)
			return i;
	}

	return -1;
}

void Player::ServerSendTransformMessage()
{
	if (m_startSendTransform == false)
		return;

	game::TIMER_EVENT ev{ ProtocolID::WR_TRANSFORM_ACK, m_id };
	ev.objType = m_objType;
	game::MessageHandler::GetInstance()->PushTransformMessage(ev);

	//auto p{ GetTransform()->GetPosition() };
	//std::cout << p.x << ", " << p.y << ", " << p.z << "\n";
}

physx::PxVec3 Player::GetForwardVec()
{
	physx::PxQuat quat = TO_PXQUAT(m_qlook);
	physx::PxVec3 forward(0, 0, 1);

	physx::PxVec3 lookVec = quat.rotate(forward);
	lookVec.y = 0;
	lookVec.normalize();
	return lookVec;
}

void Player::SetMeteorAttackAvailable(bool value)
{
	m_meteorAvailable = value;
}

void Player::SetCameraDistance(float value)
{
	m_cameraDistance = value;
}

void Player::Set_OverlapObject(bool activate)
{
	if (!m_overlapObj)
		return;

	if (activate)
	{
		m_overlapObj->Activate();
		m_overlapObj->ServerMessage_RenderEffect_SingleStrike();		

	}
	else
		m_overlapObj->Deactivate();
}

physx::PxQuat Player::GetRotation_For_Overlap(physx::PxVec3 xzDir)
{
	physx::PxVec3 initialDir(0, 0, 1);
	physx::PxVec3 rotAxis = initialDir.cross(xzDir).getNormalized();
	physx::PxReal rotAngle = physx::PxAtan2(physx::PxSqrt(1 - physx::PxPow(xzDir.dot(initialDir), 2)), xzDir.dot(initialDir));
	physx::PxQuat rotation(rotAngle, rotAxis);

	return rotation;
}
