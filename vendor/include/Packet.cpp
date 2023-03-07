#include "Packet.h"

namespace network
{
	CPacket::CPacket() :
		m_size{ 0 },
		m_type{ TYPE::NONE },
		m_id{ 0 },
		m_readOffset{ 0 },
		m_writeOffset{ 0 }
	{
		// 버퍼 0으로 초기화
		m_data.fill(0);
	}

	CPacket::~CPacket()
	{
	}

	//bool CPacket::ValidWriteSize(uint32_t size)
	//{
	//	if (HEADER_SIZE + m_writeOffset + size > m_totalSize)
	//	{
	//		return true;	// 추후 false로 수정
	//	}

	//	return true;
	//}

	//bool CPacket::ValidReadSize(uint32_t size)
	//{
	//	if (HEADER_SIZE + m_readOffset + size > m_totalSize)
	//	{
	//		return true;	// 추후 false로 수정
	//	}

	//	return true;
	//}

	// 전송 클라이언트 id 작성
	void CPacket::WriteID(uint32_t id)
	{
		uint16_t size{ sizeof(id) };

		std::memcpy(m_data.data(), &id, size);
	}

	// 패킷을 전송한 클라이언트 id 읽기
	uint32_t CPacket::ReadID()
	{
		uint32_t id;

		std::memcpy(&id, m_data.data(), sizeof(uint32_t));

		return id;
	}

	// 헤더 포함 패킷 전체 사이즈 작성
	void CPacket::WriteSize()
	{
		uint16_t totalSize{ static_cast<uint16_t>(m_size + HEADER_SIZE) };

		std::memcpy(m_data.data() + PACKET_SIZE_OFFSET, &totalSize, sizeof(totalSize));
	}

	// 패킷 사이즈 읽기
	const uint16_t CPacket::ReadSize()
	{
		uint16_t size;

		std::memcpy(&size, m_data.data() + PACKET_SIZE_OFFSET, sizeof(size));

		return size;
	}

	// 아직 사용X
	void CPacket::SetData(uint32_t id)
	{
		m_type = TYPE::WRITE;
		m_size = 0;

		m_id = id;
		WriteID(id);

		m_readOffset = 0;
		m_writeOffset = 0;
	}

	// 전송받은 데이터를 CPacket 클래스에 저장
	void CPacket::SetData(char* data)
	{
		std::memcpy(m_data.data(), data, BUFF_SIZE);

		m_type = TYPE::READ;
		m_id = ReadID();
		m_size = ReadSize();
		m_readOffset = 0;
		m_writeOffset = 0;
	}

	// Deprecated
	void CPacket::SetData(std::array<char, BUFF_SIZE>::iterator recvIter, int32_t recvDataSize, int32_t prevSize)
	{
		std::ranges::copy(recvIter, recvIter + recvDataSize, m_data.begin() + prevSize);

		m_type = TYPE::READ;
		m_id = ReadID();
		m_size = ReadSize();
		m_readOffset = 0;
		m_writeOffset = 0; 
	}

	// string 작성 메서드 별도 작성(채팅 프로그램 구현 시 사용)
	void CPacket::WriteString(const std::string& data)
	{
		uint16_t length{ static_cast<uint16_t>(data.length()) };

		if (length > 0)
		{
			//if (ValidWriteSize(length) == false)
			//	return;

			Write(length);

			std::memcpy(m_data.data() + (HEADER_SIZE + m_writeOffset), data.c_str(), length);
			m_writeOffset += length;
			m_size += length;

			WriteSize();
		}
	}

	// wstring 작성 메서드 별도 작성(채팅 프로그램 구현 시 사용)
	void CPacket::WriteWString(const std::wstring& data)
	{
		uint16_t length{ static_cast<uint16_t>(data.length() * 2) };

		if (length > 0)
		{
			//if (ValidWriteSize(length) == false)
			//	return;

			Write(length);

			std::memcpy(m_data.data() + (HEADER_SIZE + m_writeOffset), data.c_str(), length);
			m_writeOffset += length;
			m_size += length;

			WriteSize();
		}
	}

	// string 읽기 메서드 별도 작성(채팅 프로그램 구현 시 사용)
	void CPacket::ReadString(std::string& str)
	{
		uint32_t length{ Read<uint16_t>() };

		//if (ValidReadSize(length) == false)
		//	return;

		if (length == 0)
			return;

		str.resize(length, 0);
		std::memcpy(str.data(), m_data.data() + (HEADER_SIZE + m_readOffset), length);

		m_readOffset += length;
	}

	// wstring 읽기 메서드 별도 작성(채팅 프로그램 구현 시 사용)
	void CPacket::ReadWString(std::wstring& wstr)
	{
		uint32_t length{ Read<uint16_t>() };

		//if (ValidReadSize(length) == false)
		//	return;

		if (length == 0)
			return;

		wstr.resize(length / 2, 0);
		std::memcpy(wstr.data(), m_data.data() + (HEADER_SIZE + m_readOffset), length);

		m_readOffset += length;
	}
}