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

protected:
	friend class Resources;
	virtual void Load(const wstring& path) override;
	virtual void Save(const wstring& path) override;

public:
	const vector<MeshRenderInfo>& GetMeshRenderInfo(void) { return m_meshRenders; }

public:
	static shared_ptr<MeshData> LoadFromFBX(const wstring& path);

	vector<shared_ptr<CGameObject>> Instantiate();
	vector<shared_ptr<CGameObject>> Instantiate_Player();

public:
	static SimpleMath::Quaternion ConvertFbxToDirectQuaternion(const FbxQuaternion& q);
	static SimpleMath::Vector3 ConvertFbxDouble3ToVector3(const FbxDouble3& v);

//private:
//	friend class Resources;
//	friend class FBXMapLoader;

public:
	static HANDLE CreateFileWrite(const wstring& path);
	static HANDLE CreateFileRead(const wstring& path);

private:
	vector<MeshRenderInfo> m_meshRenders;
};
