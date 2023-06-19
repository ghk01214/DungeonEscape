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

Player::Player(int32_t playerID, const Vec3& position, const Quat& rotation, const Vec3& scale) :
	GameObject{ position, rotation, scale },
	m_aniIndex{ 12 },
	m_aniFrame{ 0.f },
	m_aniSpeed{ 1.f },
	m_hp{ 20 },
	m_firstSingleStrike{ true }
{
	m_id = playerID;
	m_attack.fill(false);
}

Player::~Player()
{
}

void Player::Init()
{
	m_controller = AddComponent<CustomController>(L"CustomController", true);

	auto body = m_controller->GetBody();
	body->SetMass(body->GetMass() * 0.7f);
	SetObjectType(server::OBJECT_TYPE::PLAYER);


	for (int i = static_cast<int>(server::TRIGGER_TYPE::NONE) + 1; i < static_cast<int>(server::TRIGGER_TYPE::MAX); ++i)
	{
		m_triggerDictionary[static_cast<server::TRIGGER_TYPE>(i)] = false;
	}
}

void Player::Update(double timeDelta)
{
	// 플레이어 사망 시 삭제 로직 추가 필요		
	// AS : 삭제로직 이미 있어. 삭제 호출 함수는 GameObject::SetRemoveReserved()야. 해당 함수를 호출하면 특정 루프 이후 객체를 게임에서 삭제해(layer::late_update참고)
	// AS : layer::Late_Update에서 오브젝트가 실제로 삭제돼. 거기에서 삭제 직전에 messageHandler써서 죽은 놈 아이디를 전 클라에 뿌려. 아마 그거 때문에 불공도 제대로 삭제되지 않는거 아닌가싶음
	m_controller->Move(m_id);

	PlayerPattern_SingleStrike();
	TriggerZoneStatusUpdate();
	// AS : PlayerPatttern_singleStrike(), PlayerPattern_ShootBall() 전부 여기서 호출할꺼야.
	// AS : 해당 함수 진입문에 1.FBX, 2.키보드 따져서 진입할거라고. 제발 컨트롤러에 키보드 정보만 입력해.
	// AS : 근데 컨트롤러 키보드 로직 정확한거 맞냐? 키보드를 3가지로 구분하고, 키보드 종류가 5개라면 변수 15개가 필요한데 변수가 8개밖에 없는데? 잘되는거 확실하면 상관없긴함.

	// AS : 난 AttackType을 무슨 용도로 사용한건지 모르겠다. controller안에 키보드 정보 싹 다 넣어놓으면, 그거에 맞춰서 player_pattern()내부에 키보드 + FBX 조건문 걸어서 진입할텐데.
	
	// AS : 왜 HP를 서버에 저장하는거야? state, hp, animation frame등등 모두 클라에서 관리하잖아?
	// AS : 1번PC 플레이어가 뒤져있으면, 234도 1이 뒤져있는거 알고 있고, 1번에서도 자기 키보드 정보를 서버에게 안보낼거아니야 뒤졌으니깐. 난 도저히 HP는 왜 서버에 있는지 모르겠음.

	GameObject::Update(timeDelta);

	if (m_startSendTransform == true)
	{
		game::Message msg{ m_id, ProtocolID::WR_TRANSFORM_ACK };
		game::MessageHandler::GetInstance()->PushTransformMessage(msg);
		//AS:플레이어 상시로 보내기로 확인한거 아니였냐. 왜 조건문안에 있어?
	}
}

void Player::LateUpdate(double timeDelta)
{
	m_controller->ClearControllerCollisionInfo();
	m_transform->ConvertPX(m_controller->GetBody()->GetGlobalPose());
}

void Player::Release()
{
	m_controller = nullptr;
	GameObject::Release();
}

bool Player::IsOnGound()
{
	return m_controller->IsOnGround();
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

void Player::SetAniInfo(int32_t aniIndex, float aniFrame, float aniSpeed)
{
	m_aniIndex = aniIndex;
	m_aniFrame = aniFrame;
	m_aniSpeed = aniSpeed;
}

void Player::GetDamaged(int32_t damage)
{
	m_hp -= damage;
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

CustomController* Player::GetController()
{
	return m_controller;
}

void Player::SetControllerCameraLook(Vec3& value)
{
	m_controller->CameraLookReceive(value);
}

void Player::SetAttackTypeFlag(server::ATTACK_TYPE attack, bool flag)
{
	m_attack[magic_enum::enum_integer(attack) - 1] = flag;
}

bool Player::GetAttackType(server::ATTACK_TYPE& type)
{
	for (int32_t i = 0; i < m_attack.size(); ++i)
	{
		if (m_attack[i] == true)
		{
			type = magic_enum::enum_value<server::ATTACK_TYPE>(i + 1);
			return true;
		}
	}

	return false;
}

void Player::PlayerPattern_ShootBall(server::OBJECT_TYPE type, int32_t objID, float power)
{
	if (m_fbxType != server::FBX_TYPE::MISTIC)
		return;

	server::ATTACK_TYPE attackType{ server::ATTACK_TYPE::NONE };

	if (GetAttackType(attackType) == false)
		return;

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

	// TODO : 오브젝트 id 발급은 MessageHandler에서 이루어지는데 PlayerPattern_ShootBall을 직접 호출하지 못 하면 오브젝트 id를 어떻게 건내면 좋을까?
	auto ballObject = objmgr->AddGameObjectToLayer<SkillObject>(L"Layer_SkillObject", ballPos, Quat(0, 0, 0, 1), Vec3(skillBallHalfExtent, skillBallHalfExtent, skillBallHalfExtent));
	auto ballBody = ballObject->GetComponent<RigidBody>(L"RigidBody");
	ballBody->SetMass(1.f);
	ballBody->AddCollider<SphereCollider>(ballObject->GetTransform()->GetScale());
	ballObject->SetID(objID);
	ballObject->SetName(L"Sphere");
	ballObject->SetFBXType(server::FBX_TYPE::SPHERE);
	ballObject->SetObjectType(type);

	ballBody->AddForce(ForceMode::Impulse, playerCameraLook * power);

	SetAttackTypeFlag(attackType, false);
}

void Player::PlayerPattern_SingleStrike()
{
	if (m_fbxType != server::FBX_TYPE::NANA)
		return;

	server::ATTACK_TYPE attackType{ server::ATTACK_TYPE::NONE };

	//	AS  
	// if (fbx != 전사 || key != (기술1에 해당되는 키보드))
	//		return; 이런 방식으로 쓸거야. 모든 키보드 반응 함수들은 그렇게 호출할거야. controller가 move에서 사용하듯이. 이건 ShootBall도 공통으로 적용되는거

	if (GetAttackType(attackType) == false)
		return;

	enum { start, end };
	std::array<float, 2> attackTime;

	switch (attackType)
	{
		case server::ATTACK_TYPE::ATK0:
		{
			attackTime[start] = 0.2f;
			attackTime[end] = 0.43f;
		}
		break;
		case server::ATTACK_TYPE::ATK1:
		{
			attackTime[start] = 0.2f;
			attackTime[end] = 0.39f;
		}
		break;
		case server::ATTACK_TYPE::ATK2:
		{
			attackTime[start] = 0.08f;
			attackTime[end] = 0.19f;
		}
		break;
		case server::ATTACK_TYPE::ATK3:
		{
			attackTime[start] = 0.6f;
			attackTime[end] = 0.75f;
		}
		break;
		case server::ATTACK_TYPE::ATK4:
		{
			attackTime[start] = 0.08f;
			attackTime[end] = 0.27f;
		}
		break;
	}

	auto objmgr = ObjectManager::GetInstance();
	physx::PxVec3 playerPos = TO_PX3(GetControllerPosition());				//플레이어 위치
	physx::PxVec3 cameraDir = m_controller->GetCameraLook();
	cameraDir.y = 0;  // y 컴포넌트를 0으로 설정
	cameraDir.normalize();  // 벡터를 정규화
	float angle = physx::PxAtan2(cameraDir.z, cameraDir.x);
	physx::PxQuat cameraRot(angle, physx::PxVec3(0, 1, 0));					//카메라 회전정보

	float radius = m_controller->GetCollider()->GetRadius();
	float extent = 100.f;
	physx::PxVec3 triggerPos = playerPos + cameraDir * (radius + extent);	//트리거 위치

	if (m_firstSingleStrike == true)
	{
		m_attackTrigger = objmgr->AddGameObjectToLayer<TriggerObject>(L"Trigger", FROM_PX3(triggerPos), FROM_PXQUAT(cameraRot), Vec3(extent, extent, extent));
		m_attackTrigger->SetTriggerType(server::TRIGGER_TYPE::SINGLE_STRIKE, attackTime[start], attackTime[end]);
		auto triggerBody = m_attackTrigger->GetComponent<RigidBody>(L"RigidBody");
		triggerBody->AddCollider<BoxCollider>(m_attackTrigger->GetTransform()->GetScale());
		triggerBody->GetCollider(0)->SetTrigger(true);
		m_firstSingleStrike = false;
	}
	else
	{
		m_attackTrigger->RestoreOneTimeEffect();
		auto triggerBody = m_attackTrigger->GetComponent<RigidBody>(L"RigidBody");
		triggerBody->SetPosition(FROM_PX3(triggerPos), true);
	}

	SetAttackTypeFlag(attackType, false);
}
