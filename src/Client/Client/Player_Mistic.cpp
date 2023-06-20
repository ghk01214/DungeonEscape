#include "pch.h"
#include "Player_Mistic.h"

#include "Animator.h"
#include "Input.h"

#include "Transform.h"
#include "Timer.h"

#include <SceneManager.h>
#include <Scene.h>
#include <GameObject.h>
#include <Transform.h>

#include <NetworkManager.h>
#include <Network.h>

#include "Camera.h"

void Player_Mistic::Start()
{
	m_prevState = IDLE_A;
	m_currState = IDLE_A;

	m_hp = 30;

	std::cout << std::format("PLAYER {} HP : {}", GetNetwork()->GetID(), m_hp) << std::endl;

	if (GetNetwork()->IsMyPlayer() == false)
		return;

	GetNetwork()->SendAddPlayer(server::FBX_TYPE::MISTIC);

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld *= Matrix::CreateRotationY(XMConvertToRadians(180.f));
	GetTransform()->SetWorldMatrix(matWorld);
}

void Player_Mistic::Update(void)
{
	if (GetNetwork()->IsMyPlayer() == false)
		return;

	DecidePlayerDeath();
	KeyInputStateChange();
}

void Player_Mistic::LateUpdate()
{
	// 카메라가 바라보고 있는 방향으로 플레이어를 움직임.
	MovePlayerCameraLook();

	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();

	ParsePackets();
}

void Player_Mistic::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
		case AERT:
		{

		}
		break;
		case ATK0:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case ATK1:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case ATK2:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case ATK3:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case ATK4:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case BLOCK:
		{

		}
		break;
		case DAMAGE:
		{
			GetAnimator()->Play(m_currState);

			m_hp -= 5;
			std::cout << std::format("PLAYER {} HP : {}", GetNetwork()->GetID(), m_hp) << std::endl;
		}
		break;
		case DASH:
		{

		}
		break;
		case DIE0:
		{
			GetAnimator()->Play(m_currState);

			if (GetNetwork()->IsMyPlayer() == true)
				GetNetwork()->SendDie();
		}
		break;
		case DIE1:
		{
			GetAnimator()->Play(m_currState);

			if (GetNetwork()->IsMyPlayer() == true)
				GetNetwork()->SendDie();
		}
		break;
		case DIE2:
		{
			GetAnimator()->Play(m_currState);

			if (GetNetwork()->IsMyPlayer() == true)
				GetNetwork()->SendDie();
		}
		break;
		case IDLE_A:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case IDLE_B:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case IDLE_C:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case JUMP_START:
		{
			// 물리 시뮬레이터의 점프 속도가 클라이언트에서 재생되는 점프 속도보다 빨라서
			// 점프 애니메이션의 속도를 2배로 조정
			// 점프 애니메이션의 길이 : 1.16
			GetAnimator()->Play(m_currState, 1.f);
		}
		break;
		case JUMPING:
		{
			auto ani{ GetAnimator() };
			auto updateTime{ 16.f / ani->GetFramePerSecond() };

			ani->PlayFrame(JUMP_START, updateTime, 0.f);
		}
		break;
		case JUMP_END:
		{
			GetAnimator()->PlayFrame(JUMP_START, GetAnimator()->GetUpdateTime(), 1.f);
		}
		break;
		case MOVE:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case MOVE_L:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
		case MOVE_R:
		{
			GetAnimator()->Play(m_currState);
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
		case SHOOT:
		{
			GetAnimator()->Play(m_currState);
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
		case VICTORY_A:
		{

		}
		break;
		case VICTORY_B:
		{

		}
		break;
		default:
		break;
	}

	m_prevState = m_currState;
}

void Player_Mistic::UpdateFrameRepeat()
{
	switch (m_currState)
	{
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		case DAMAGE: case DIE0: case DIE1: case DIE2: case DEAD:
		case JUMP_START: case JUMPING: case JUMP_END:
		case SHOOT: case SLEEP: case SWOON: case TIRED:
		return;
		default:
		break;
	}

	auto anim{ GetAnimator() };

	GetAnimator()->RepeatPlay();
}

void Player_Mistic::UpdateFrameOnce()
{
	switch (m_currState)
	{
		case IDLE_A: case IDLE_B: case IDLE_C:
		case MOVE: case MOVE_L: case MOVE_R:
		case RUN: case RUN_L: case RUN_R:
		case WALK: case WALK_L: case WALK_R:
		case VICTORY_A: case VICTORY_B:
		case DEAD:
		return;
		default:
		break;
	}

	auto anim{ GetAnimator() };

	anim->CalculateUpdateTime();

	if (anim->IsAnimationEnd() == true)
	{
		switch (m_currState)
		{
			case ATK0:
			case ATK1:
			case ATK2:
			case ATK3:
			case ATK4:
			{
				m_currState = IDLE_A;
			}
			break;
			case DAMAGE:
			{
				m_currState = IDLE_A;
			}
			break;
			case DIE0:
			case DIE1:
			case DIE2:
			{
				m_currState = DEAD;
				anim->SetAniSpeed(0.f);
			}
			break;
			case JUMP_END:
			{
				m_currState = IDLE_A;
			}
			break;
			case SHOOT:
			{
				m_currState = IDLE_A;
			}
			break;
			case SLEEP:
			{
				m_currState = IDLE_A;
			}
			break;
			case SWOON:
			{
				m_currState = IDLE_A;
			}
			break;
			case TIRED:
			{
				m_currState = IDLE_A;
			}
			break;
		}
	}

	anim->PlayNextFrame();

	if (m_currState == JUMP_START)
	{
		if (anim->GetAnimFrame() == 16)
		{
			m_currState = JUMPING;
		}
	}
}

void Player_Mistic::KeyInputStateChange()
{
	switch (m_currState)
	{
		case ATK0: case ATK1: case ATK2: case ATK3: case ATK4:
		case JUMP_START: case JUMPING: case JUMP_END:
		case DIE0: case DIE1: case DIE2: case DEAD:
		case DAMAGE:
		break;
		default:
		{
			if (INPUT->GetButtonDown(KEY_TYPE::W) == true
				or INPUT->GetButton(KEY_TYPE::W) == true)
			{
				m_currState = MOVE;
			}
			else if (INPUT->GetButtonDown(KEY_TYPE::A) == true
				or INPUT->GetButton(KEY_TYPE::A) == true)
			{
				m_currState = MOVE_L;
			}
			else if (INPUT->GetButtonDown(KEY_TYPE::S) == true
				or INPUT->GetButton(KEY_TYPE::S) == true)
			{
				m_currState = MOVE;
			}
			else if (INPUT->GetButtonDown(KEY_TYPE::D) == true
				or INPUT->GetButton(KEY_TYPE::D) == true)
			{
				m_currState = MOVE_R;
			}
			else if (INPUT->GetButtonUp(KEY_TYPE::W) == true
				or INPUT->GetButtonUp(KEY_TYPE::A) == true
				or INPUT->GetButtonUp(KEY_TYPE::S) == true
				or INPUT->GetButtonUp(KEY_TYPE::D) == true)
			{
				m_currState = IDLE_A;
			}
			else if (INPUT->GetButtonDown(KEY_TYPE::SPACE) == true)
			{
				m_currState = JUMP_START;
			}

			if (m_currState != ATK0 or m_currState != ATK1 or m_currState != ATK2
				or m_currState != ATK3 or m_currState != ATK4)
			{
				if (INPUT->GetButtonDown(KEY_TYPE::KEY_1) == true)
				{
					m_currState = ATK0;
				}
				else if (INPUT->GetButtonDown(KEY_TYPE::KEY_2) == true)
				{
					m_currState = ATK1;
				}
				else if (INPUT->GetButtonDown(KEY_TYPE::KEY_3) == true)
				{
					m_currState = ATK2;
				}
				else if (INPUT->GetButtonDown(KEY_TYPE::KEY_4) == true)
				{
					m_currState = ATK3;
				}
			}
		}
		break;
	}
}

void Player_Mistic::DecidePlayerDeath()
{
	if (m_hp <= 0)
		m_currState = DIE0;
}

float Player_Mistic::GetAngleBetweenVector(const XMVECTOR& vector1, const XMVECTOR& vector2)
{
	XMVECTOR v = XMVector3AngleBetweenVectors(vector1, vector2);

	return XMVectorGetX(v);
}

void Player_Mistic::TurnPlayer(Vec3 from, Vec3 to)
{
	float crossProduct = from.x * to.z - from.z * to.x;

	XMVECTOR vNormal1 = XMVector3Normalize(from);
	XMVECTOR vNormal2 = XMVector3Normalize(to);

	float dot = XMVectorGetX(XMVector3Dot(vNormal1, vNormal2));
	float angle = acosf(dot);

	if (XMConvertToDegrees(angle) < 10)
		return;

	if (crossProduct < 0)
	{
		// 시계방향
		GetTransform()->TurnAxisY(true);
	}
	else
	{
		// 반시계방향
		GetTransform()->TurnAxisY(false);
	}
}

void Player_Mistic::MovePlayerCameraLook(void)
{
	const auto& transform = GetTransform();

	std::shared_ptr<CScene> activeScene = GET_SINGLE(SceneManager)->GetActiveScene();
	const auto& camera = activeScene->GetMainCamera();

	// 카메라가 존재할 경우
	if (nullptr != camera)
	{
		//Vec3 pos = GetTransform()->GetWorldPosition();

		//const shared_ptr<Transform>& cameraTransform = camera->GetTransform();

		//// 초당 이동 속도
		//if (INPUT->GetButton(KEY_TYPE::W))
		//{
		//	pos.x += cameraTransform->GetLook().x * _speed * DELTA_TIME;
		//	pos.z += cameraTransform->GetLook().z * _speed * DELTA_TIME;
		//	TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Forward());
		//}
		//if (INPUT->GetButton(KEY_TYPE::S))
		//{
		//	pos.x -= cameraTransform->GetLook().x * _speed * DELTA_TIME;
		//	pos.z -= cameraTransform->GetLook().z * _speed * DELTA_TIME;
		//	TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Backward());
		//}
		//if (INPUT->GetButton(KEY_TYPE::A))
		//{
		//	pos.x -= cameraTransform->GetRight().x * _speed * DELTA_TIME;
		//	pos.z -= cameraTransform->GetRight().z * _speed * DELTA_TIME;
		//	TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Right());
		//}
		//if (INPUT->GetButton(KEY_TYPE::D))
		//{
		//	pos.x += cameraTransform->GetRight().x * _speed * DELTA_TIME;
		//	pos.z += cameraTransform->GetRight().z * _speed * DELTA_TIME;
		//	TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Left());
		//}
		//Matrix matWorld = GetTransform()->GetWorldMatrix();
		//matWorld.Translation(pos);
		//GetTransform()->SetWorldMatrix(matWorld);

		//TurnPlayer(GetTransform()->GetWorldMatrix().Forward(), camera->GetTransform()->GetLook());

		Vec3 pos{ GetTransform()->GetWorldVec3Position() };
		Matrix matWorld{ GetTransform()->GetWorldMatrix() };
		matWorld.Translation(pos);
		GetTransform()->SetWorldMatrix(matWorld);

		GetNetwork()->SendCameraLook(camera->GetTransform()->GetLook());
	}
}

void Player_Mistic::ParsePackets()
{
	auto size{ GetNetwork()->GetRecvQueueSize() };

	if (size == 0)
		return;

	auto packets{ GetNetwork()->GetRecvPackets() };

	if (packets.empty() == true)
		return;

	GetNetwork()->ClearRecvQueue(size);

	for (int32_t i = 0; i < size; ++i)
	{
		if (packets.empty() == true)
			return;

		auto packet{ packets.front() };

		switch (packet.ReadProtocol())
		{
			case ProtocolID::WR_ADD_ANIMATE_OBJ_ACK:
			{
				StartRender(packet);
			}
			break;
			case ProtocolID::WR_TRANSFORM_ACK:
			{
				Transform(packet);
			}
			break;
			case ProtocolID::WR_ANI_ACK:
			{
				ChangeAnimation(packet);
			}
			break;
			case ProtocolID::WR_JUMP_START_ACK:
			{
				m_currState = JUMP_START;
			}
			break;
			case ProtocolID::WR_ATTACK_ACK:
			{
				m_currState = magic_enum::enum_cast<PLAYER_STATE>(packet.Read<int32_t>()).value();
			}
			break;
			case ProtocolID::WR_HIT_ACK:
			{
				if (m_currState != DEAD)
					m_currState = DAMAGE;
			}
			break;
			case ProtocolID::WR_DIE_ACK:
			{
				if (m_currState != DEAD)
					m_currState = DIE0;
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}

void Player_Mistic::StartRender(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	if (GetNetwork()->GetID() == -1)
		GetNetwork()->SetID(id);

	if (GetNetwork()->GetID() != -1)
	{
		Vec3 pos;
		pos.x = packet.Read<float>();
		pos.y = packet.Read<float>();
		pos.z = packet.Read<float>();

		Vec4 quat;
		quat.x = packet.Read<float>();
		quat.y = packet.Read<float>();
		quat.z = packet.Read<float>();
		quat.w = packet.Read<float>();

		Vec3 scale;
		scale.x = packet.Read<float>();
		scale.y = packet.Read<float>();
		scale.z = packet.Read<float>();

		int32_t aniIndex{ packet.Read<int32_t>() };
		float aniFrame{ packet.Read<float>() };

		std::cout << std::format("ID : {}\n", id);
		std::cout << std::format("pos : {}, {}, {}\n", pos.x, pos.y, pos.z);
		std::cout << std::format("quat : {}, {}, {}, {}\n", quat.x, quat.y, quat.z, quat.w);
		std::cout << std::format("scale : {}, {}, {}\n\n", scale.x, scale.y, scale.z);

		GetTransform()->SetWorldVec3Position(pos);
		Matrix matWorld{ GetTransform()->GetWorldMatrix() };
		matWorld.Translation(pos);
		GetTransform()->SetWorldMatrix(matWorld);
		GetAnimator()->Play(aniIndex, aniFrame);
	}
}

void Player_Mistic::Transform(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	Vec3 pos;
	pos.x = packet.Read<float>();
	pos.y = packet.Read<float>();
	pos.z = packet.Read<float>();

	Vec4 quat;
	quat.x = packet.Read<float>();
	quat.y = packet.Read<float>();
	quat.z = packet.Read<float>();
	quat.w = packet.Read<float>();

	Vec3 scale;
	scale.x = packet.Read<float>();
	scale.y = packet.Read<float>();
	scale.z = packet.Read<float>();

	bool onGround{ packet.Read<bool>() };

	if (onGround == true)
	{
		if (m_currState == JUMPING)
		{
			m_currState = JUMP_END;
		}
	}
	else
	{
		switch (m_currState)
		{
			case IDLE_A:
			case IDLE_B:
			case IDLE_C:
			//case MOVE:
			//case MOVE_L:
			//case MOVE_R:
			{
				m_currState = JUMPING;
			}
			break;
		}
	}

	GetTransform()->SetWorldVec3Position(pos);
	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld.Translation(pos);
	GetTransform()->SetWorldMatrix(matWorld);

	//auto t{ GetTransform()->GetWorldPosition() };
	//std::cout << std::format("id - {}, pos : {}, {}, {}", id, t.x, t.y, t.z) << std::endl;
}

void Player_Mistic::ChangeAnimation(network::CPacket& packet)
{
	if (m_currState != DEAD)
	{
		int32_t index{ packet.Read<int32_t>() };
		float frame{ packet.Read<float>() };
		float speed{ packet.Read<float>() };

		m_currState = magic_enum::enum_cast<PLAYER_STATE>(index).value();
		m_prevState = m_currState;

		GetAnimator()->PlayFrame(index, frame, speed);
	}
}

void Player_Mistic::AddColliderToObject(network::CPacket& packet)
{
	int32_t objID{ packet.Read<int32_t>() };
	int32_t colliderID{ packet.Read<int32_t>() };
	int32_t tempColliderID{ packet.Read<int32_t>() };

	// 충돌체 추가
	/*for (auto& obj : m_collisionObjects[objID])
	{
		m_tempColliderCont[tempColliderID].id = colliderID;
		obj->SetCollider(colliderID, m_tempColliderCont[tempColliderID]);
	}*/
}
