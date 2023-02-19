#include "Server.h"

int32_t main()
{
	try
	{
		boost::asio::io_context ioContext;
		game::CServer logic{ &ioContext, GAME_SERVER_PORT };
		
		logic.Run();
	}
	catch (const std::exception& error)
	{
		std::format("Error : {}", error.what());
	}
}