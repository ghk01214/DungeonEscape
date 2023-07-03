#include "pch.h"
#include "physx_utils.h"
#include "Server.h"
#include "GameInstance.h"
#include "TimeManager.h"

int32_t main()
{
	std::setlocale(LC_ALL, "korean");

	std::unique_ptr<game::CServer> game_server{ std::make_unique<game::CServer>() };
	game_server->Run();
}