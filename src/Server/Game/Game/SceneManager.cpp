#include "pch.h"
#include "Session.h"
#include "TempObject.h"
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
			auto sessionObj{ player->GetTempObject() };

			for (auto& [key, obj] : sessionObj)
			{
				player->SendTransformTempPacket(key, ProtocolID::WR_TRANSFORM_ACK, obj);
			}
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
		m_sceneLock.lock();

		m_currentScene = m_scene[id];

		m_sceneLock.unlock();
	}

	void CSceneManager::CreateObject(const std::wstring& fbxName, TempTrans trans)
	{
		auto objID{ NewObjectID() };
		CTempObject* tempObject{ new CTempObject{ fbxName, trans } };
		tempObject->SetID(objID);

		Accessor<int32_t, CTempObject*> access;
		m_objects.insert(access, objID);

		access->second = tempObject;
	}

	void CSceneManager::RemoveObject(int32_t id)
	{

	}

	int32_t CSceneManager::NewObjectID()
	{
		bool issueNewID{ m_reusableObjectID.empty() };

		// 재사용 가능 id가 없으면 최고 숫자 발급
		if (issueNewID == true)
			return m_objectsNum++;

		int32_t newID{ -1 };

		// 재사용 가능한 id가 있으면 재사용 가능한 id 중 가장 낮은 id 발급
		while (true)
		{
			bool issueReuseID{ m_reusableObjectID.try_pop(newID) };

			if (issueReuseID == true)
				return newID;
		}
	}

	void CSceneManager::DecodeKeyInput(int32_t id, int32_t objID, unsigned long keyInput)
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

		if (input[static_cast<int32_t>(server::KEY_TYPE::LEFT)] == true)
		{

		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::RIGHT)] == true)
		{

		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::UP)] == true)
		{

		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::DOWN)] == true)
		{

		}
		if (input[static_cast<int32_t>(server::KEY_TYPE::SPACE)] == true)
		{

		}
	}

	void CSceneManager::SendTransformPacket(int32_t objID)
	{
		for (auto& player : m_session)
		{
			if (player->GetState() != STATE::INGAME)
				continue;

			player->SendTransformTempPacket(objID, ProtocolID::WR_TRANSFORM_ACK, player->GetTempObject(objID));
		}
	}
}