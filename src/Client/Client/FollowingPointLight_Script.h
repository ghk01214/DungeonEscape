#pragma once

#include "MonoBehaviour.h"

class FollowingPointLight_Script : public MonoBehaviour
{
public:
	FollowingPointLight_Script();
	~FollowingPointLight_Script();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void LateUpdate() override;

};