#pragma once

#include "MonoBehaviour.h"

class PhysxObject_Script : public MonoBehaviour
{
public:
	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

private:
	void ParsePackets();
	void Transform(network::CPacket& packet);
};