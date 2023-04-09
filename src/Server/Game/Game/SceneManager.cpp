#include "pch.h"
#include "Session.h"
#include "TempObject.h"
#include "Scene.h"
#include "SceneManager.h"

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
				player->SendTransformTempPacket(key, ProtocolID::WR_TRANSFORM_REQ, obj);
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

	void CSceneManager::DecodeKeyInput(int32_t id, int32_t objID, uint8_t keyInput, server::KEY_STATE keyState)
	{
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

		auto ks{ static_cast<uint8_t>(keyState) };

		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::LEFT)) != 0x00)
		{
			// 오브젝트 검색 방식
			//session->GetTempObject(objID);
		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::RIGHT)) != 0x00)
		{

		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::UP)) != 0x00)
		{

		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::DOWN)) != 0x00)
		{

		}
		if ((keyInput & static_cast<uint8_t>(server::KEY_TYPE::SPACE)) != 0x00)
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