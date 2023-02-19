#include "pch.h"
#include <Packet.h>
#include "Application.h"

namespace chat
{
	CApplication::CApplication(boost::asio::io_context& context) :
		m_pContext{ &context },
		m_socket{ context }
	{
		Connect();
	}

	CApplication::~CApplication()
	{
	}

	void CApplication::Connect()
	{
		tcp::endpoint serverAddr{ boost::asio::ip::address::from_string("127.0.0.1"), CHAT_SERVER_PORT };
		m_socket.connect(serverAddr);
	}

	void CApplication::SyncWrite()
	{
		while (true)
		{
			std::wstring message;

			std::cout << std::format("Enter message : ");
			std::getline(std::wcin, message);

			if (message.length() == 0)
			{
				std::cout << std::format("Message length 0\n");
				std::cout << std::format("Terminate Program!\n");

				m_socket.close();
				return;
			}

			boost::system::error_code errorCode;
			packet::CPacket sendPacket{};
			sendPacket.SetData(0);
			sendPacket.WriteWString(message);

			m_socket.write_some(boost::asio::buffer(sendPacket.GetPacketData(), sendPacket.GetTotalPacketSize()), errorCode);

			if (errorCode == boost::asio::error::eof)
			{
				std::cout << std::format("Terminate Program!\n");

				m_socket.close();
				return;
			}
			else if (errorCode)
			{
				std::cout << errorCode << "\n";
				std::cout << std::format("Terminate Program!\n");

				m_socket.close();
				return;
			}

			packet::CPacket recvPacket{};
			size_t length{ m_socket.read_some(boost::asio::buffer(recvPacket.GetPacketData(), packet::CPacket::BUFF_SIZE), errorCode)};

			if (errorCode == boost::asio::error::eof)
			{
				std::cout << std::format("Terminate Program!\n");

				m_socket.close();
				return;
			}
			else if (errorCode)
			{
				std::cout << errorCode << "\n";
				std::cout << std::format("Terminate Program!\n");

				m_socket.close();
				return;
			}

			std::wstring wstr{};
			recvPacket.ReadWString(wstr);

			std::wcout << std::format(L"{} bytes received : {}\n\n", length, wstr);
		}
	}

	void CApplication::AsyncWrite()
	{
		//while (true)
		//{
			std::wstring message;

			std::cout << std::format("Enter message : ");
			std::getline(std::wcin, message);

			if (message.length() == 0)
			{
				std::cout << std::format("Message length 0\n");
				std::cout << std::format("Terminate Program!\n");

				m_socket.close();
				return;
			}

			packet::CPacket sendPacket{};
			sendPacket.SetData(0);
			sendPacket.WriteWString(message);
			uint16_t size{ sendPacket.GetTotalPacketSize() };

			m_socket.async_write_some(boost::asio::buffer(sendPacket.GetPacketData(), size),
				[this, size](boost::system::error_code errorCode, std::size_t bytesTransferred)
				{
					if (errorCode == boost::asio::error::eof)
					{
						std::cout << std::format("Terminate Program!\n");

						m_socket.close();
						return;
					}
					else if (errorCode)
					{
						std::cout << errorCode << "\n";
						std::cout << std::format("Terminate Program!\n");

						m_socket.close();
						return;
					}

					if (size != bytesTransferred)
					{
						std::cout << std::format("Incomplete Send occured. Terminate Program!\n");
						
						m_socket.close();
						return;
					}

					AsyncWrite();
				});
		//}
	}

	void CApplication::AsyncRead()
	{
		packet::CPacket recvPacket{};

		m_socket.async_read_some(boost::asio::buffer(recvPacket.GetPacketData(), packet::CPacket::BUFF_SIZE), [this, &recvPacket](boost::system::error_code errorCode, size_t length)
			{
				if (errorCode == boost::asio::error::eof)
				{
					std::cout << std::format("Terminate Program!\n");

					m_socket.close();
					return;
				}
				else if (errorCode)
				{
					std::cout << errorCode << "\n";
					std::cout << std::format("Terminate Program!\n");

					m_socket.close();
					return;
				}

				recvPacket.SetData(0);
				
				std::wstring wstr{};
				recvPacket.ReadWString(wstr);

				std::wcout << std::format(L"{} bytes received : {}", length, wstr);

				AsyncRead();
			});
	}
}