#pragma once

#include "GameObject.h"

class CustomController;

class Player : public GameObject
{
public:
	Player(int32_t playerID = -1, const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Player() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	constexpr int32_t GetPlayerID() const { return m_playerID; }
	constexpr int32_t GetAniIndex() const { return m_aniIndex; }
	constexpr float GetAniFrame() const { return m_aniFrame; }

	void SetAniIndex(int32_t aniIndex);
	void SetAniFrame(float aniFrame);
	void ClientRequestStateChange(int32_t clientStateRequest, float clientStateFrame);
	void ClientRequestExcuted();

public:
	void SetControllerMoveSpeed(float value);
	float GetControllerMoveSpeed();
	void SetControllerJumpSpeed(float value);
	float GetControllerJumpSpeed();
public:
	CustomController* m_controller = nullptr;

	int32_t m_playerID = 4294967295U;

	int32_t m_aniIndex;
	float m_aniFrame;

	// 클라에서 스테이트 변경 요청이 옴
	int32_t m_clientRequestState;
	float m_clientStateFrame;
	bool m_clientStateChangeRequest;
};
