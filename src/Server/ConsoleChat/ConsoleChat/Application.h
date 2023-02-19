#pragma once

namespace chat
{
	SKIP_TCP_NAMESPACE;

	class CApplication
	{
	public:
		CApplication() = default;
		CApplication(boost::asio::io_context& context);
		~CApplication();

		void Connect();
		void SyncWrite();

		void AsyncRead();
		void AsyncWrite();
	private:
		boost::asio::io_context* m_pContext;
		tcp::socket m_socket;
	};
}
