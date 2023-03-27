#pragma once
#include "Object.h"

class Mesh;
class Material;
class CGameObject;

struct MeshRenderInfo
{
	shared_ptr<Mesh>				mesh;
	vector<shared_ptr<Material>>	materials;
};

class MeshData : public Object
{
public:
	MeshData();
	virtual ~MeshData();

public:
	static shared_ptr<MeshData> LoadFromFBX(const wstring& path);

	virtual void Load(const wstring& path);
	virtual void Save(const wstring& path);

	vector<shared_ptr<CGameObject>> Instantiate();

private:
	HANDLE CreateFileWrite(const wstring& path);
	HANDLE CreateFileRead(const wstring& path);

private:
	vector<MeshRenderInfo> m_meshRenders;
};
