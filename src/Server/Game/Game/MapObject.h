#pragma once

#include "GameObject.h"

class RigidBody;

class MapObject : public GameObject
{
public:
	MapObject(const Vec3& position = Vec3(0,0,0), const Quat& rotation = Quat(0,0,0,1), const Vec3& scale = Vec3(1.f, 1.f, 1.f));
	~MapObject() override;

public:
	virtual void Init();
	virtual void Update(double timeDelta) override;
	virtual void LateUpdate(double timeDelta) override;
	virtual void Release();

public:		//server
	bool GetRequireFlagTransmit();
	void SetRequireFlagTransmit(bool value);

public:
	void ApplyRequestedLayers();

private:
	RigidBody* m_body = nullptr;

private:	//server
	bool m_requiresPacketTransmit = false;
};



