#pragma once

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,			// 이동 관련
	MESH_RENDERER,		// 메쉬 렌더
	CAMERA,				// 카메라
	// ...
	MONO_BEHAVIOUR,		// 스크립트용
	END
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1		// MONO_BEHAVIOUR을 제외한 컴포넌트의 갯수
};

class CGameObject;
class CTransform;

// 상속용 class
class CComponent
{
public:
	CComponent(COMPONENT_TYPE type);
	virtual ~CComponent();

public:
	virtual void Awake() { }	// 초기화 관련 함수 - 컴포넌트 생성 시 참조와 변수를 초기화하는것들 처리
	virtual void Start() { }	// 시작 - 초기화 된 컴포넌트들의 데이터에 접근 가능
	virtual void Update() { }	// Update
	virtual void LateUpdate() { }	// LateUpdate
	virtual void FinalUpdate() { }	// 진짜 마지막 Update 함수...

public:
	COMPONENT_TYPE GetType() { return m_type; }
	bool IsValid() { return m_gameObject.expired() == false; }	// 소멸 여부 확인

	std::shared_ptr<CGameObject> GetGameObject();
	std::shared_ptr<CTransform> GetTransform();

private:
	// GameObject에게만 컴포넌트 class의 접근 권한을 줌
	friend class CGameObject;
	void SetGameObject(std::shared_ptr<CGameObject> gameObject) { m_gameObject = gameObject; }

protected:
	COMPONENT_TYPE m_type;
	std::weak_ptr<CGameObject> m_gameObject;	// 게임오브젝트와 서로 참조하고 있어야 하므로, 참조카운터 관련 문제로 weak_ptr 사용
};



