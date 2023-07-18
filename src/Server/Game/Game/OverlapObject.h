#pragma once

class Player;
class MonsterAI;

class OverlapObject
{
	friend class MonsterAI;

public:
	OverlapObject(MonsterAI* monsterAI);
	~OverlapObject();

public:
	void Update();
	void Release();

public:
	void Activate(std::string scheduleName);
	void Deactivate();

	void Active();
public:
	bool IsPlayerDuplicate(Player* player);
	void ApplyMonsterSkillToPlayer(Player* player);

private:
	std::vector<Player*> m_duplicates;
	std::string m_currentScheduleName;
	bool m_active = false;
	MonsterAI* m_monsterAI;
};

