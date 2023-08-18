#pragma once

#include "Client_Defines.h"
#include <Scene.h>
#include <MonoBehaviour.h>

class InfoUI_Script;
class Fade_Script;
class PortalUI_Script;
class Scenematic_Script;
class Magic_Artifact_Script;

#include <unordered_set>

struct EffectInfo
{
	int32_t index;
	Vec3 pos;
	Vec3 scale;
	float speed;
	float alpha = 1.f;
};

enum class MAP_TYPE
{
	StartRoom,
	FirstBoss,
	Cave,
	SecondRoom_Bridge_SecondBoss,
	ThirdRoom_RockRolling,
	LastBoss_TreasureRoom,
	END
};

class Scene_Test final : public CScene
{
public:
	explicit Scene_Test();
	virtual ~Scene_Test() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

private:
	void CreateLayer(void);
	void CreateComputeShader(void);
	void CreateMainCamera(shared_ptr<CScene> pScene);
	void CreateUICamera(shared_ptr<CScene> pScene);
	void CreateSkyBox(shared_ptr<CScene> pScene);
	void CreateUI(shared_ptr<CScene> pScene);
	void CreateMRTUI(shared_ptr<CScene> pScene);
	void CreatePortalUI(shared_ptr<CScene> pScene);
	void CreateLights(shared_ptr<CScene> pScene);
	void CreateMap(shared_ptr<CScene> pScene);
	void CreatePlayer(shared_ptr<CScene> pScene, server::FBX_TYPE player);
	void CreateSphere(shared_ptr<CScene> pScene);
	void CreateSkill(shared_ptr<CScene> pScene);
	void CreateBillBoard(shared_ptr<CScene> pScene);
	void CreateEffect(shared_ptr<CScene> pScene);
	void CreateMagicArtifactEffect(shared_ptr<CScene> pScene);

private:
	std::shared_ptr<CGameObject> CreateBillBoardBase(vector<shared_ptr<class Texture>> textures, float fPassingTime);
	std::shared_ptr<CGameObject> CreateEffectBase(vector<shared_ptr<class Texture>> textures, float fPassingTime);
	std::vector<std::shared_ptr<CGameObject>> CreateSkillBase(const std::wstring& skillName, const std::wstring& fbxPath);
	std::shared_ptr<CGameObject> CreateArtifactBase(vector<shared_ptr<class Texture>> textures);

private:
	void CreateHPnSPBar();
	void CreateFade(shared_ptr<CScene> pScene);

private:
	void SendKeyInput();

	void ChangeNetworkObjectID(network::CPacket& packet);
	void CreateAnimatedRemoteObject(network::CPacket& packet);
	void CreateRemoteObject(network::CPacket& packet);
	void RemoveObject(network::CPacket& packet);
	void RemoveNonAnimatedObject(int32_t id);
	void PlayEffect(network::CPacket& packet);
	void ChangeSound(network::CPacket& packet);
	void TriggerBehaviour(network::CPacket& packet);
	void TriggerInteractionCount(network::CPacket& packet);
	void PlayCutScene(network::CPacket& packet);

	void ClassifyObject(server::FBX_TYPE type, ObjectDesc& objectDesc, int32_t stateIndex = -1);
	void AddObjectToScene(server::OBJECT_TYPE type, std::vector<std::shared_ptr<CGameObject>>& gameObjects);
	void AddObjectEffectScript(std::shared_ptr<CGameObject>& gameObject, server::FBX_TYPE type);
	void AddEffectTextures();

public:
	std::vector<std::shared_ptr<CGameObject>> CreateMapObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> CreateAnimatedObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> AddNetworkToObject(std::vector<std::shared_ptr<CGameObject>> object, server::OBJECT_TYPE objectType, int32_t id = -1);

public:
	void PushMapData(MAP_TYPE eType, std::vector<std::shared_ptr<CGameObject>> objects);

private:
	void Init(shared_ptr<Scene_Test> pScene, server::FBX_TYPE eType);

private:
	std::unordered_map<int32_t, int32_t> m_objectIDMap;		// temp obj id, new obj id

	std::unordered_set<int32_t> m_overlappedObjects;

public:
	static shared_ptr<CScene> Create(server::FBX_TYPE eType);

private:
	std::vector<int32_t> m_effects;
	std::vector<int32_t> m_billboards;
	std::unordered_map<server::EFFECT_TYPE, EffectInfo> m_billboardInfo;

private:
	std::shared_ptr<InfoUI_Script> m_InfoUIScript = make_shared<InfoUI_Script>();

public:
	void CheckMapMove(void);
	void MoveMap(MAP_TYPE eType);

private:
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_1;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_2;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_3;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_4;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_5;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_6;

	MAP_TYPE m_eCurrentMapType = MAP_TYPE::END;
	MAP_TYPE m_eNextMapType = MAP_TYPE::END;

private:
	std::shared_ptr<Fade_Script> m_fadeScript = nullptr;
	std::shared_ptr<PortalUI_Script> m_portalUIScript = nullptr;
	std::shared_ptr<Scenematic_Script> m_scenematicScript = nullptr;

private:
	std::vector<std::shared_ptr<Magic_Artifact_Script>> m_artifactMagicScript;
};