
#include "Client_Defines.h"
#include "Scene.h"

class Scene_Logo final : public CScene
{
public:
	explicit Scene_Logo();
	virtual ~Scene_Logo() = default;

public:
	virtual void Awake();
	virtual void Start();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FinalUpdate();
	virtual void Render();

public:
	static shared_ptr<Scene_Logo> Create();
};