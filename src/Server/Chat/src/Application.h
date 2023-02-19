#pragma once

#include <Singleton.h>

namespace chat
{
	SKIP_TCP_NAMESPACE;
	
	class CApplication : public Singleton<CApplication>
	{
	public:
		CApplication(boost::asio::io_context& context, int32_t port);
		~CApplication();

		void WorkerThread();
		void Run();
	private:
		void Accept();

	private:
		boost::asio::io_context* m_pContext;
		tcp::acceptor m_acceptor;
		tcp::socket m_socket;

		std::vector<std::thread> m_workerThreads;
	};
}
