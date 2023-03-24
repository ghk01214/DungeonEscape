#pragma once

class CGameObject;

namespace network
{
	class NetworkManager
	{
		DECLARE_SINGLE(NetworkManager);

	public:
		void Update();	// 주기적으로 처리해야 하는 일 처리

	public:
		void RegisterObject(std::shared_ptr<CGameObject> object);

	private:
		std::unordered_map<uint32_t, std::shared_ptr<CGameObject>> m_networkObject;		// 변수명은 원하는데로 변경																	// map<objectID, 게임오브젝트> VS map<게임오브젝트, objectID> -> 변경을 원하면 알려줄것, 현재는 전자
	};
}