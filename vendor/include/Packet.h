#pragma once

namespace packet
{
	enum class TYPE
	{
		NONE = 0,
		READ,
		WRITE,
		MAX
	};

	template<typename T>
	concept IsSingleVariable = requires
	{
		std::is_arithmetic_v<T>;
		std::is_enum_v<T>;
		typeid(T).name() != typeid(std::string).name();
	};

	class CPacket
	{
	public:
		enum : uint16_t
		{
			BUFF_SIZE = 4096,
			HEADER_SIZE = 6,
			PACKET_SIZE_OFFSET = 4
		};
	public:
		CPacket();
		~CPacket();

		//bool ValidWriteSize(uint32_t size);
		//bool ValidReadSize(uint32_t size);

		void WriteID(uint32_t id);
		template<typename T>
		void Write(T data) requires IsSingleVariable<T>;
		void WriteString(const std::string& data);
		void WriteWString(const std::wstring& data);
		void WriteSize();

		uint32_t ReadID();
		template<typename T>
		T Read() requires IsSingleVariable<T>;
		void ReadString(std::string& str);
		void ReadWString(std::wstring& wstr);

		constexpr uint32_t GetID() { return m_id; }
		constexpr uint16_t GetPacketSize() { return m_size; }
		constexpr uint16_t GetTotalPacketSize() { return m_size + HEADER_SIZE; }
		constexpr uint32_t GetRemainDataSize() { return m_size - m_readOffset; }
		constexpr std::array<uint8_t, BUFF_SIZE>& GetPacketData() { return m_data; }

		void SetData(uint32_t id);
		void SetData(std::array<uint8_t, BUFF_SIZE>::iterator recvIter, int32_t recvDataSize, int32_t prevSize);
	private:
		const uint16_t ReadSize();

	private:
		uint16_t m_size;		// Header를 제외한 데이터만의 크기
		TYPE m_type;
		uint32_t m_id;

		std::array<uint8_t, BUFF_SIZE> m_data;
		uint32_t m_readOffset;
		uint32_t m_writeOffset;
	};

	template<typename T>
	void CPacket::Write(T data) requires IsSingleVariable<T>
	{
		int32_t size{ sizeof(data) };

		//if (ValidWriteSize(size) == false)
		//	return;

		std::memcpy(m_data.data() + (HEADER_SIZE + m_writeOffset), &data, size);
		m_writeOffset += size;
		m_size += size;

		WriteSize();
	}

	template<typename T>
	T CPacket::Read() requires IsSingleVariable<T>
	{
		int32_t size{ sizeof(T) };

		//if (ValidReadSize(size) == false)
		//	return T{};

		T read{};
		std::memcpy(&read, m_data.data() + (HEADER_SIZE + m_readOffset), size);
		m_readOffset += size;

		return read;
	}
}