#include "pch.h"
#include "physx_utils.h"
#include "Server.h"
#include "GameInstance.h"
#include "TimeManager.h"

int32_t main()
{
	std::unique_ptr<game::CServer> game_server{ std::make_unique<game::CServer>() };
	game_server->Run();

	//GameInstance* game = GameInstance::GetInstance();
	//game->Init();
	//while (1)
	//{
	//	double timeDelta = TimeManager::GetInstance()->GetElapsedTime();
	//	game->Update(timeDelta);
	//	game->LateUpdate(timeDelta);
	//}
	//GameInstance::GetInstance()->DestroyInstance();
	//game = nullptr;
}