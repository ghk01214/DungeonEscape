#include "pch.h"
#include "Player_Script.h"

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

Player_Script::Player_Script(server::FBX_TYPE type) :
	m_playerType{ type },
	m_prevState{ IDLE1 },
	m_currState{ m_prevState },
	m_radius{ 50.f },
	m_halfHeight{ 50.f }
{
}

Player_Script::~Player_Script()
{
}

void Player_Script::Start()
{
	if (GetNetwork()->IsMyPlayer() == true)
		GetNetwork()->SendAddPlayer(m_playerType);

	GetAnimator()->Play(m_currState);

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld *= Matrix::CreateRotationY(XMConvertToRadians(180.f));
	GetTransform()->SetWorldMatrix(matWorld);
}

void Player_Script::Update(void)
{
	ParsePackets();
}

void Player_Script::LateUpdate()
{
	// 카메라가 바라보고 있는 방향으로 플레이어를 움직임.
	if (GetNetwork()->IsMyPlayer() == true)
		MovePlayerCameraLook();

	CheckState();
	UpdateFrameRepeat();
	UpdateFrameOnce();
}

void Player_Script::CheckState()
{
	if (m_prevState == m_currState)
		return;

	switch (m_currState)
	{
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
		case DEAD:
		break;
		default:
		{
			GetAnimator()->Play(m_currState);
		}
		break;
	}

	m_prevState = m_currState;

//	std::cout << magic_enum::enum_name(m_currState) << std::endl;
}

void Player_Script::UpdateFrameRepeat()
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

	GetAnimator()->RepeatPlay();
}

void Player_Script::UpdateFrameOnce()
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

	auto anim{ GetAnimator() };

	anim->CalculateUpdateTime();

	if (anim->IsAnimationEnd() == true)
		return;

	anim->PlayNextFrame();

	if (m_currState == JUMP_START)
	{
		if (anim->GetAnimFrame() == 16)
		{
			m_currState = JUMPING;
		}
	}
}

float Player_Script::GetAngleBetweenVector(const XMVECTOR& vector1, const XMVECTOR& vector2)
{
	XMVECTOR v = XMVector3AngleBetweenVectors(vector1, vector2);

	return XMVectorGetX(v);
}

void Player_Script::TurnPlayer(Vec3 from, Vec3 to)
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

void Player_Script::MovePlayerCameraLook(void)
{
	const auto& transform = GetTransform();

	std::shared_ptr<CScene> activeScene = GET_SINGLE(SceneManager)->GetActiveScene();
	const auto& camera = activeScene->GetMainCamera();
	const auto& cameraTransform = camera->GetTransform();

	// 카메라가 존재할 경우
	if (nullptr != camera)
	{
		// 방향키를 누르면 카메라가 바라보는 방향을 기준으로 회전

		// W(앞쪽 방향키를 누르면)
		if (INPUT->GetButton(KEY_TYPE::W))
		{
			TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Forward());
		}

		// W(앞쪽 방향키를 누르면)
		if (INPUT->GetButton(KEY_TYPE::S))
		{
			TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Backward());
		}

		// W(앞쪽 방향키를 누르면)
		if (INPUT->GetButton(KEY_TYPE::A))
		{
			TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Right());
		}

		// W(앞쪽 방향키를 누르면)
		if (INPUT->GetButton(KEY_TYPE::D))
		{
			TurnPlayer(GetTransform()->GetWorldMatrix().Backward(), cameraTransform->GetWorldMatrix().Left());
		}

		// 서버에 카메라 Look 방향 전송
		GetNetwork()->SendCameraLook(camera->GetTransform()->GetLook());
	}
}

void Player_Script::ParsePackets()
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
			case ProtocolID::WR_CHANGE_STATE_ACK:
			{
				m_currState = magic_enum::enum_value<PLAYER_STATE>(packet.Read<int32_t>());
			}
			break;
			default:
			break;
		}

		packets.pop_front();
	}
}

void Player_Script::StartRender(network::CPacket& packet)
{
	int32_t id{ packet.ReadID() };

	if (GetNetwork()->GetID() == -1)
		GetNetwork()->SetID(id);

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

	int32_t currState{ packet.Read<int32_t>() };
	float updateTime{ packet.Read<float>() };

	std::cout << std::format("ID : {}\n", id);
	std::cout << std::format("pos : {}, {}, {}\n", pos.x, pos.y, pos.z);
	//std::cout << std::format("quat : {}, {}, {}, {}\n", quat.x, quat.y, quat.z, quat.w);
	//std::cout << std::format("scale : {}, {}, {}\n\n", scale.x, scale.y, scale.z);

	m_currState = magic_enum::enum_value<PLAYER_STATE>(currState)	;

	pos.y -= (m_radius + m_halfHeight);

	Matrix matWorld{ Matrix::CreateRotationY(180.f) };
	matWorld *= Matrix::CreateTranslation(pos);
	GetTransform()->SetWorldMatrix(matWorld);
}

void Player_Script::Transform(network::CPacket& packet)
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

	pos.y -= (m_radius + m_halfHeight);

	Matrix matWorld{ GetTransform()->GetWorldMatrix() };
	matWorld.Translation(pos);
	GetTransform()->SetWorldMatrix(matWorld);

#pragma region [FOR DEBUGGING]
	//auto t{ GetTransform()->GetWorldPosition() };
	//std::cout << std::format("player id - {}, pos : {}, {}, {}", id, t.x, t.y, t.z) << std::endl;
#pragma endregion
}
