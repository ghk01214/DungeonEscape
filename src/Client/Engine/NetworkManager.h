#pragma once

class CGameObject;

class NetworkManager
{
	DECLARE_SINGLE(NetworkManager);

public:
	void Update();	// 주기적으로 처리해야 하는 일 처리

public:
	void RegisterObject(std::shared_ptr<CGameObject> object);

private:
	map<uint32, std::shared_ptr<CGameObject>> m_networkObject;
};