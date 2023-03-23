#pragma once

class CGameObject;

class NetworkManager
{
	DECLARE_SINGLE(NetworkManager);

public:
	void Update();	// 주기적으로 처리해야 하는 일 처리

public:
	void RegiserObject(std::shared_ptr<CGameObject> object);

private:
	map<uint32, std::shared_ptr<CGameObject>> m_networkObject;		// 변수명은 원하는데로 변경
																	// map<objectID, 게임오브젝트> VS map<게임오브젝트, objectID> -> 변경을 원하면 알려줄것, 현재는 전자
};

