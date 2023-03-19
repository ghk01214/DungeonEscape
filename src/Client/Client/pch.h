#pragma once

#pragma comment(lib, "Engine.lib")

#include "EnginePch.h"

// server 관련
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <Packet.h>
#include <Define.h>
#include <protocol.hpp>
#include <OVERLAPPEDEX.h>
#include "Network.h"

#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

namespace network
{
	extern std::unique_ptr<CNetwork> pNetwork;

	void ErrorQuit(const std::wstring& msg);
#if _DEBUG
	void ErrorDisplay(const std::wstring& msg);
#endif
}