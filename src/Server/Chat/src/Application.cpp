#include "Application.h"
#include "Server.h"

namespace chat
{
	CApplication::CApplication(boost::asio::io_context& context, int32_t port) :
		m_pContext{ &context },
		m_acceptor{ context, tcp::endpoint(tcp::v4(), port) },
		m_socket{ context }
	{
		std::setlocale(LC_ALL, "UTF-8");

		Accept();
	}

	CApplication::~CApplication()
	{
	}

	void CApplication::WorkerThread()
	{
		m_pContext->run();
	}

	void CApplication::Run()
	{
		for (int32_t i = 0; i < 4; ++i)
		{
			m_workerThreads.push_back(std::thread{ &chat::CApplication::WorkerThread, this });
		}

		std::ranges::for_each(m_workerThreads, [](std::thread& thread) { thread.join(); });
	}

	void CApplication::Accept()
	{
		m_acceptor.async_accept(m_socket,
			[this](boost::system::error_code errorCode)
			{
				if (!errorCode)
				{
					std::cout << "Accepted" << "\n";
					std::make_shared<CServer>(std::move(m_socket))->Run();
				}

				Accept();
			});
	}
}
