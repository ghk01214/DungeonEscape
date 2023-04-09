#pragma once

namespace game
{
	class CSession;
	class CScene;
	class CTempObject;

	class CSceneManager
	{
		SINGLE(CSceneManager);

	public:
		void Init();

		void Update();
		void LateUpdate();
		void FinalUpdate();

		void EnterScene(CSession* session);
		void ChangeScene(int8_t id);

		void CreateObject(const std::wstring& fbxName, TempTrans trans);
		void RemoveObject(int32_t id);
		int32_t NewObjectID();

		void DecodeKeyInput(int32_t id, int32_t objID, uint8_t keyInput, server::KEY_STATE keyState);

		void SendTransformPacket(int32_t objID);

	private:
		std::array<CScene*, MAX_SCENE> m_scene;
		CScene* m_currentScene;
		std::shared_mutex m_sceneLock;

		std::array<CSession*, 3> m_session;
		std::atomic_int32_t m_activeSessionNum;

		HashMap<int32_t, CTempObject*> m_objects;
		std::atomic_int32_t m_objectsNum;
		tbb::concurrent_priority_queue<int32_t, std::greater<int32_t>> m_reusableObjectID;

		std::shared_mutex m_objectLock;
	};
}