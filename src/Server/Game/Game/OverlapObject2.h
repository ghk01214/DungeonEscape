#pragma once

class GameObject;
class Monster;
class PillarObject;

class OverlapObject2
{
	friend class Player;

public:
	OverlapObject2(Player* player);
	~OverlapObject2();

public:
	void Init();
	void Update();
	void Release();

public:
	void UpdateOverlapPosition(physx::PxVec3 pos, physx::PxQuat quat);
	void UpdateOverlapType(string name);
	void Activate();
	void Deactivate();

	void Active();
public:
	std::vector<Monster*> OverlapCheck_Monster();
	std::vector<PillarObject*> OverlapCheck_Pillar();
	bool IsObjectDuplicate(GameObject* obj);
	bool ApplySkillToMonster(Monster* monster);

	physx::PxQuat GetRotation_For_Overlap(physx::PxVec3 xzDir);

	void ServerMessage_RenderEffect(Monster* monster);

private:
	std::vector<GameObject*> m_duplicates;
	physx::PxVec3 m_pos;
	physx::PxQuat m_rot;
	bool m_active = false;
	physx::PxBoxGeometry* m_box = nullptr;
	string m_attackType;

	Player* m_player;
};

