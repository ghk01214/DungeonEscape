#pragma once

#include "GameObject.h"

class CustomController;

class Player : public GameObject
{
public:
	Player(const Vec3& position = Vec3(), const Quat& rotation = Quat(), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~Player() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	uint32_t GetPlayerID() { return m_playerID; }
	//최초 아이디 배정만 true값을 리턴한다.
	bool SetPlayerID(uint32_t playerID);

public:
	CustomController* m_controller = nullptr;

	uint32_t m_playerID = 4294967295U;
};
