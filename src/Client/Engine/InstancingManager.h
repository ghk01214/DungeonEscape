﻿#pragma once
#include "InstancingBuffer.h"

class CGameObject;

class InstancingManager
{
	DECLARE_SINGLE(InstancingManager);

public:
	void Render(vector<shared_ptr<CGameObject>>& gameObjects);

	void ClearBuffer();
	void Clear() { m_buffers.clear(); }

private:
	void AddParam(uint64 instanceId, InstancingParams& data);

private:
	unordered_map<uint64/*instanceId*/, shared_ptr<InstancingBuffer>> m_buffers;
};
