﻿#pragma once

#include "Client_Defines.h"
#include <Scene.h>
#include <MonoBehaviour.h>
#include <unordered_set>

class Texture;
class InfoUI_Script;
class Fade_Script;
class PortalUI_Script;
class Cinematic_Script;
class Magic_Artifact_Script;

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
	void CreateMainCamera(std::shared_ptr<CScene> pScene);
	void CreateUICamera(std::shared_ptr<CScene> pScene);
	void CreateSkyBox(std::shared_ptr<CScene> pScene);
	void CreateUI(std::shared_ptr<CScene> pScene);
	void CreateMRTUI(std::shared_ptr<CScene> pScene);
	void CreatePortalUI(std::shared_ptr<CScene> pScene);
	void CreateLights(std::shared_ptr<CScene> pScene);
	void CreateMap(std::shared_ptr<CScene> pScene);
	void CreatePlayer(std::shared_ptr<CScene> pScene, server::FBX_TYPE player);
	void CreateSphere(std::shared_ptr<CScene> pScene);
	void CreateSkill(std::shared_ptr<CScene> pScene);
	void CreateBillBoard(std::shared_ptr<CScene> pScene);
	void CreateEffect(std::shared_ptr<CScene> pScene);
	void CreateMagicArtifactEffect(std::shared_ptr<CScene> pScene);

private:
	std::shared_ptr<CGameObject> CreateBillBoardBase(std::vector<shared_ptr<Texture>> textures, float fPassingTime);
	std::shared_ptr<CGameObject> CreateEffectBase(std::vector<shared_ptr<Texture>> textures, float fPassingTime);
	std::vector<std::shared_ptr<CGameObject>> CreateSkillBase(const std::wstring& skillName, const std::wstring& fbxPath);
	std::shared_ptr<CGameObject> CreateArtifactBase(std::vector<shared_ptr<Texture>> textures);
	std::vector<std::shared_ptr<CGameObject>> CreateMapObject(ObjectDesc& objectDesc);
	std::vector<std::shared_ptr<CGameObject>> CreateAnimatedObject(ObjectDesc& objectDesc);
	void CreateFade(std::shared_ptr<CScene> pScene);

private:
	void PushMapData(MAP_TYPE eType, std::vector<std::shared_ptr<CGameObject>> objects);

private:
	void CreateHPnSPBar();
	void CreateOneTimeDialogue();

	void CreatePopUp();
	void CreateBlur();
	void CreateCloseButton();
	void CreateBGMButton();
	void CreateBGMSlider();
	void CreateSEButton();
	void CreateSESlider();

private:
	void ChangePopUpVisibility();
	void ChangeVolume();
	void ChangeMuteTexture();

private:
	void SendKeyInput();

	void ChangeNetworkObjectID(network::CPacket& packet);
	std::vector<std::shared_ptr<CGameObject>> AddNetworkToObject(std::vector<std::shared_ptr<CGameObject>> object, server::OBJECT_TYPE objectType, int32_t id = -1);
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

private:
	void CheckMapMove(void);
	void MoveMap(MAP_TYPE eType);

private:
	void Init(std::shared_ptr<Scene_Test> pScene, server::FBX_TYPE eType);

public:
	static std::shared_ptr<CScene> Create(server::FBX_TYPE eType);

private:
	std::unordered_map<int32_t, int32_t> m_objectIDMap;		// temp obj id, new obj id
	std::unordered_set<int32_t> m_overlappedObjects;

private:
	std::vector<int32_t> m_effects;
	std::vector<int32_t> m_billboards;
	std::unordered_map<server::EFFECT_TYPE, EffectInfo> m_billboardInfo;

private:
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_1;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_2;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_3;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_4;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_5;
	std::vector<std::shared_ptr<CGameObject>> m_splitMap_6;

	MAP_TYPE m_eCurrentMapType;
	MAP_TYPE m_eNextMapType;

private:
	std::shared_ptr<InfoUI_Script> m_InfoUIScript;
	std::shared_ptr<Fade_Script> m_fadeScript;
	std::shared_ptr<PortalUI_Script> m_portalUIScript;
	std::shared_ptr<Cinematic_Script> m_cinematicScript;

	std::vector<std::shared_ptr<Magic_Artifact_Script>> m_artifactMagicScript;
	std::unordered_map<std::string, std::shared_ptr<class OneTimeDialogue_Script>> m_oneTimeDialogueScript;

	std::vector<std::shared_ptr<CGameObject>> m_popUp;

	std::shared_ptr<class CloseButton_Script> m_closeButton;
	std::vector<std::shared_ptr<class SliderTip_Script>> m_sliderTip;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSlider;
	std::vector<std::shared_ptr<class VolumeSlider_Script>> m_volumeSliderLeftTip;
	std::vector<std::shared_ptr<class MuteButton_Script>> m_muteButton;
};