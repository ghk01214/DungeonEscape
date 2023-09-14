#pragma once

#include "GameObject.h"

class RigidBody;

class ParticleObject : public GameObject
{
public:
	ParticleObject(const Vec3& position = Vec3(0, 0, 0), const Quat& rotation = Quat(0, 0, 0, 1), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~ParticleObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:
	void ServerMessage_Init();
	void ServerMessage_Release();							//SetRemoveReserved()이후 바로 호출해야하는 함수

public:
	void ApplyRequestedLayers();


public:
	static void Summon(physx::PxVec3 initialPos, float xzInterval, float xzStrength, float yStrength, float deleteTime);

protected:
	RigidBody* m_body = nullptr;

private:
	Vec3 m_spawnPlace;
	Vec3 m_flyDirection;
};
