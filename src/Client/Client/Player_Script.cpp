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

void Player_Mistic::Start()
{
	if (GetNetwork()->IsMyPlayer())
		GetNetwork()->SendAddPlayer();
}

void Player_Mistic::Update(void)
{
	if (GetNetwork()->IsMyPlayer())
	{
		int32 count = GetAnimator()->GetAnimCount();
		int32 currentIndex = GetAnimator()->GetCurrentClipIndex();
		int32 index = 0;

		if (INPUT->GetButtonDown(KEY_TYPE::KEY_1))
		{
			index = (currentIndex + 1) % count;

			//GetNetwork()->SendAniIndexPacket(index);
			GetAnimator()->Play(index);
		}

		if (INPUT->GetButtonDown(KEY_TYPE::KEY_2))
		{
			index = (currentIndex - 1 + count) % count;

			//GetNetwork()->SendAniIndexPacket(index);
			GetAnimator()->Play(index);
		}
	}
}

void Player_Mistic::LateUpdate()
{
	// 카메라가 바라보고 있는 방향으로 플레이어를 움직임.
	MovePlayerCameraLook();

	ParsePackets();
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

		Vec3 pos = GetTransform()->GetWorldVec3Position();
		Matrix matWorld = GetTransform()->GetWorldMatrix();
		matWorld.Translation(pos);
		GetTransform()->SetWorldMatrix(matWorld);

		auto look{ camera->GetTransform()->GetLook() };
		GetNetwork()->SendCameraLook(camera->GetTransform()->GetLook());

		std::cout << look.x << ", " << look.y << ", " << look.z << "\n";
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
			default:
			break;
		}
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
		auto mat{ Matrix::CreateTranslation(pos) };
		GetTransform()->SetWorldMatrix(mat);
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

	GetTransform()->SetWorldVec3Position(pos);
	auto mat{ Matrix::CreateTranslation(pos) };
	GetTransform()->SetWorldMatrix(mat);
}

void Player_Mistic::ChangeAnimation(network::CPacket& packet)
{
	int32_t index{ packet.Read<int32_t>() };
	float frame{ packet.Read<float>() };

	GetAnimator()->Play(index, frame);
}
