﻿#pragma once

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

public:	//LastBossRock전용함수
	void RecordInitialPosition(Vec3 Position);
	bool SinkBelow();
	bool RiseUp();
	void CheckSkip();

	void SkipClear();


	void Gimmik2SoundAlram();

public:
	void ServerMessage_Init(bool scatterRock, bool boulder);
	void ServerMessage_Release();							//SetRemoveReserved()이후 바로 호출해야하는 함수

	void ServerMessage_Transform();

public:
	void ApplyRequestedLayers();

protected:
	RigidBody* m_body = nullptr;

private:
	Vec3 m_initialPosition;
	Vec3 m_riseupPosition;
	bool m_skip = false;

	bool m_gimmik2Alarm = false;
};
