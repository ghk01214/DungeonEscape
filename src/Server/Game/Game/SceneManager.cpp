#include "pch.h"
#include "Session.h"
#include "Scene.h"
#include "SceneManager.h"

#include <bitset>

namespace game
{
	void CSceneManager::Init()
	{
		m_activeSessionNum = 0;
	}

	void CSceneManager::Update()
	{
	}

	void CSceneManager::LateUpdate()
	{
		// 매 프레임마다 Transform packet을 클라이언트로 전송
		//m_objectLock.lock_shared();

		for (auto& [key, obj] : m_objects)
		{
			SendTransformPacket(key);
		}

		for (auto& player : m_session)
		{

		}

		//m_objectLock.unlock_shared();
	}

	void CSceneManager::FinalUpdate()
	{
	}

	void CSceneManager::EnterScene(CSession* session)
	{
		if (m_activeSessionNum >= 3)
			return;

		m_session[m_activeSessionNum++] = session;
	}

	void CSceneManager::ChangeScene(int8_t id)
	{
		/*m_sceneLock.lock();

		m_currentScene = m_scene[id];

		m_sceneLock.unlock();*/
	}

	void CSceneManager::CreateObject(const std::wstring& fbxName, TempTrans trans)
	{
		//auto objID{ NewObjectID() };
		//CTempObject* tempObject{ new CTempObject{ fbxName, trans } };
		//tempObject->SetID(objID);
		//
		//Accessor<int32_t, CTempObject*> access;
		//m_objects.insert(access, objID);
		//
		//access->second = tempObject;
		//
		//SendAddTempPacket(tempObject);
	}

	void CSceneManager::RemoveObject(int32_t id)
	{
		/*m_objects.erase(id);
		m_reusableObjectID.push(id);
		--m_objectsNum;*/
	}

	int32_t CSceneManager::NewObjectID()
	{
		//bool issueNewID{ m_reusableObjectID.empty() };

		//// 재사용 가능 id가 없으면 최고 숫자 발급
		//if (issueNewID == true)
		//	return m_objectsNum++;

		//int32_t newID{ -1 };

		//// 재사용 가능한 id가 있으면 재사용 가능한 id 중 가장 낮은 id 발급
		//while (true)
		//{
		//	bool issueReuseID{ m_reusableObjectID.try_pop(newID) };

		//	if (issueReuseID == true)
		//		return newID;
		//}

		return 0;
	}

	void CSceneManager::DecodeKeyInput(int32_t id, int32_t objID, unsigned long long keyInput)
	{
		// 매 프레임마다 클라이언트로부터 키 정보를 받아옴
		//m_currentScene->DecodeKeyInput(keyInput, keyState);
		CSession* session{ nullptr };

		for (auto& client : m_session)
		{
			if (client->GetID() == id)
			{
				session = client;
				break;
			}
		}

		const int32_t useKeyCount{ static_cast<int32_t>(server::KEY_TYPE::MAX) };

		std::bitset<useKeyCount> input{ keyInput };
		bool press{ false };

		if (input[static_cast<int32_t>(server::KEY_TYPE::LEFT)] == true)
		{
			std::cout << std::format("Left, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::RIGHT)] == true)
		{
			std::cout << std::format("Right, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::UP)] == true)
		{
			std::cout << std::format("Up, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::DOWN)] == true)
		{
			std::cout << std::format("Down, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::SPACE)] == true)
		{
			std::cout << std::format("SPace, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::W)] == true)
		{
			std::cout << std::format("W, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::A)] == true)
		{
			std::cout << std::format("A, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::S)] == true)
		{
			std::cout << std::format("S, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::D)] == true)
		{
			std::cout << std::format("D, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::Q)] == true)
		{
			std::cout << std::format("Q, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::E)] == true)
		{
			std::cout << std::format("E, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::Z)] == true)
		{
			std::cout << std::format("Z, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::C)] == true)
		{
			std::cout << std::format("C, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::KEY_1)] == true)
		{
			std::cout << std::format("KEY_1, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::KEY_2)] == true)
		{
			std::cout << std::format("KEY_2, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::KEY_3)] == true)
		{
			std::cout << std::format("KEY_3, ");
			press = true;
		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::KEY_4)] == true)
		{
			std::cout << std::format("KEY_4, ");
			press = true;
		}

		if (press == true)
			std::cout << std::format("\n");
	}

	void CSceneManager::SendAddTempPacket(CTempObject* obj)
	{
		for (auto& player : m_session)
		{
			if (player->GetState() != STATE::INGAME)
				continue;


		}
	}

	void CSceneManager::SendTransformPacket(int32_t objID)
	{
		for (auto& player : m_session)
		{
			if (player->GetState() != STATE::INGAME)
				continue;


		}
	}
}