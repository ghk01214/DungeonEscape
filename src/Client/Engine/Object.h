#pragma once

enum class OBJECT_TYPE : uint8
{
	NONE,
	GAMEOBJECT, // PREFAB
	COMPONENT,
	MATERIAL,
	MESH,
	MESH_DATA,
	SHADER,
	TEXTURE,

	END
};

enum
{
	OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};

class Object
{
public:
	Object(OBJECT_TYPE type);
	virtual ~Object();

	OBJECT_TYPE GetType() { return m_gameObjectType; }

	void SetName(const wstring& name) { m_name = name; }
	const wstring& GetName() { return m_name; }

	uint32 GetID() { return m_id; }
	int32_t GetClientID() { return m_clientID; }

	void SetClientID(int32_t id) { m_clientID = id; }

	// TODO : Instantiate

protected:
	friend class Resources;
	virtual void Load(const wstring& path) { }
	virtual void Save(const wstring& path) { }

protected:
	OBJECT_TYPE m_gameObjectType = OBJECT_TYPE::NONE;
	wstring m_name;

protected:
	uint32 m_id = 0;
	int32_t m_clientID;
};

