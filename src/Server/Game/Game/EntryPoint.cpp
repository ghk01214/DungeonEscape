#include "pch.h"
#include "Server.h"

int32_t main()
{
	std::unique_ptr<game::CServer> game_server{ std::make_unique<game::CServer>() };

	game_server->Run();
}