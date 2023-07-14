#pragma once

class CGameObject;
class MonoBehaviour;

typedef struct _tagObjectDesc {
	std::wstring strName;
	std::wstring strPath;
	Vec3 vPostion;
	Vec3 vScale;
	std::shared_ptr<MonoBehaviour> script;
} ObjectDesc;

typedef struct _tagLightDesc {
	Vec3 vPosition;
	Vec3 vDirection;
	Vec3 vDiffuse;
	Vec3 vAmbient;
	Vec3 vSpecular;
	float lightRange;
	float lightAngle;
} LightDesc;

class CScene
{
public:
	virtual void Awake(void);
	virtual void Start(void);
	virtual void Update(void);
	virtual void LateUpdate(void);
	virtual void FinalUpdate(void);
	virtual void Render();

	shared_ptr<class Camera> GetMainCamera();

	void ClearRTV();

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

	void PushServerRequest(network::CPacket& packet);
	void PopRequestQueue(int32_t size);

private:
	void PushLightData();

public:
	void AddDirectionalLight(LightDesc& lightDesc);
	void AddPointLight(LightDesc& lightDesc);
	void AddSpotLight(LightDesc& lightDesc);

public:
	void AddGameObject(shared_ptr<CGameObject> gameObject);
	void AddGameObject(std::vector<std::shared_ptr<CGameObject>> gameObjects);
	void RemoveGameObject(shared_ptr<CGameObject> gameObject);
	void RemoveGameObject(std::vector<std::shared_ptr<CGameObject>> gameObjects);

public:
	void AddPlayer(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void AddMonster(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void AddBoss(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void AddSceneObject(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void AddNetworkObject(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void SetSkyBox(std::wstring key, std::wstring texture_path);

	void RemovePlayer(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void RemoveMonster(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void RemoveBoss(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void RemoveSceneObject(std::vector<std::shared_ptr<CGameObject>> gameObject);
	void RemoveNetworkObject(std::vector<std::shared_ptr<CGameObject>> gameObject);

public:
	const vector<shared_ptr<CGameObject>>& GetGameObjects(void) { return m_gameObjects; }
	const vector<shared_ptr<CGameObject>>& GetPlayer(void) { return m_player; }
	const vector<shared_ptr<CGameObject>>& GetMonster(void) { return m_monster; }
	const vector<shared_ptr<CGameObject>>& GetBoss(void) { return m_boss; }
	const vector<shared_ptr<CGameObject>>& GetSceneObject(void) { return m_sceneObject; }
	const vector<shared_ptr<CGameObject>>& GetNetworkObject(void) { return m_networkObject; }
	const shared_ptr<CGameObject>& GetSkyBoxObject(void) { return m_skyBox; }

	const std::deque<network::CPacket>& GetServerRequest() const { return m_requestQueue; }
	const int32_t GetServerRequestQueueSize() const { return m_requestQueueSize.load(); }

private:
	vector<shared_ptr<CGameObject>>		m_gameObjects;
	vector<shared_ptr<class Camera>>	m_cameras;
	vector<shared_ptr<class Light>>		m_lights;

private:
	// 필요한 오브젝트(UI, Camera 등)가 추가적으로 존재하면 그때 추가
	vector<shared_ptr<CGameObject>>		m_player;
	vector<shared_ptr<CGameObject>>		m_monster;
	vector<shared_ptr<CGameObject>>		m_boss;
	vector<shared_ptr<CGameObject>>		m_sceneObject;
	vector<shared_ptr<CGameObject>>		m_networkObject;
	shared_ptr<CGameObject>				m_skyBox;

private:
	std::deque<network::CPacket>		m_requestQueue;
	std::atomic_int32_t					m_requestQueueSize;
};

