#pragma once

class CGameObject;
class Texture;

class EffectManager
{
	DECLARE_SINGLE(EffectManager);

public:
	void ReserveEffects(uint32 num);	// 한 화면에 그려지는 최대 이펙트 수
	void PushEffectTexture(const std::wstring& effectName, vector<shared_ptr<Texture>> textures);
	void Play(const std::wstring& effectName, Vec3 vPos, Vec3 vScale, Vec3 vRotation, float fAlpha = 1.f, float fPassingTime = 0.001);
	void PlayBillBoard(const std::wstring& effectName, Vec3 vPos, Vec3 vScale, float fAlpha = 1.f, float fPassingTime = 0.001);
	void Reset(void);

	void SetSceneIndex(uint32 index) { m_sceneIndex = index; }

public:
	const vector<shared_ptr<CGameObject>>& GetEffectObject(void) { return m_renderObjects; }
	const vector<shared_ptr<CGameObject>>& GetBillBoardObject(void) { return m_billboardReserveObjects; }

	uint32_t GetBillboardCount() { return m_billboardCount; }

private:
	vector<shared_ptr<CGameObject>> m_effectObjects;
	map<std::wstring, vector<shared_ptr<Texture>>> m_effectTextures;

	vector<shared_ptr<CGameObject>> m_effectReserveObjects;
	vector<shared_ptr<CGameObject>> m_billboardReserveObjects;
	vector<shared_ptr<CGameObject>> m_renderObjects;
	uint32 m_effectCount = 0;
	uint32 m_billboardCount = 0;
	uint32 m_maxCount = 0;
	uint32 m_sceneIndex = 0;
};