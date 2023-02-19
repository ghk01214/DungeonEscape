#include <tbb/concurrent_queue.h>
#include <Packet.h>
#include "Player.h"
#include "Room.h"
#include "Session.h"

namespace chat
{
	CSession::CSession(tcp::socket& socket, CRoom& room) :
		m_socket{ socket },
		m_room{ room }
	{
	}
	
	CSession::~CSession()
	{
	}

	void CSession::Start()
	{
	}

	void CSession::Broadcast(const packet::CPacket message)
	{
	}

	void CSession::ReadHeader()
	{
	}

	void CSession::ReadData()
	{
	}

	void CSession::Write()
	{
	}


}