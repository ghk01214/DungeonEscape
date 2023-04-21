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
	unsigned int GetPlayerID();
	//최초 아이디 배정만 true값을 리턴한다.
	bool SetPlayerID(unsigned int playerID);

public:
	CustomController* m_controller = nullptr;

	unsigned int m_playerID = 4294967295U;
};
