#pragma once

#include "FBXLoader.h"

struct objectLocationInfo
{
	objectLocationInfo()
		: Position{ 1.f, 1.f, 1.f, 0.f }, Rotation{ 0.f, 0.f, 0.f, 0.f }, Scale{ 1.f, 1.f, 1.f, 1.f }
	{

	}

	FBXVec4 Position;
	FBXVec4 Rotation;
	FBXVec4 Scale;
	FbxQuaternion qLocal;
	FbxQuaternion qWorld;
};

// FBXLoader를 사용해, 맵의 정보와 관련된 정보를 입력
class FBXMapLoader
{
public:
	FBXMapLoader();
	~FBXMapLoader();

public:
	void AddBasicObject(std::wstring path);	// fbx 파일들이 존재하는 경로(.fbx), 해당 경로에 있는 fbx 파일 안이 있는 모든 mesh 정보를 가져온다.
	void ExtractMapInfo(std::wstring path);	// 맵 파일이 존재하는 경로(.txt)
	void MakeMap(void);
	auto& GetMapObjectInfo(void) { return m_MapObjectInfo; }

	const FBXMeshInfomation& FindVertexIndicesInfo(std::wstring statisMeshName);

public:
	static DirectX::XMFLOAT4 ConvertFbxToDirectQuaternion(const FbxQuaternion& q);
	static Vec3 ConvertFbxDouble3ToVector3(const FbxDouble3& v);

public:
	void SetDebugMode(bool value);

private:
	void ExtractObjectInfoFromFBX(std::wstring path);


private:
	std::unordered_map<std::wstring, FBXMeshInfomation> m_assetInfo;	// 맵에서 사용되는 staticMesh 오브젝트들에 대한 정보
																		// staticMesh이름, 메쉬 정보

	std::unordered_map<std::wstring, std::vector<objectLocationInfo>> m_mapDeployedObjectInfo;	// 맵 fbx 파일에서 추출한 fbx파일 이름, 위치정보

	std::unordered_map<std::wstring, std::wstring> m_objectStaticMesh;	// 맵 fbx 파일에서 추출한 fbx파일 이름, 해당 fbx파일이 사용하는 staticMesh 오브젝트 이름

	std::vector<std::pair<std::wstring, objectLocationInfo>> m_MapObjectInfo;	// 실제 맵 오브젝트 정보


	bool m_debugMode = false;
};

