#pragma once

#include <array>
#include <string>
#include <typeinfo>

namespace network
{
	// 패킷의 종류(아직 미사용)
	enum class TYPE
	{
		NONE = 0,
		READ,
		WRITE,
		MAX
	};

	// 스트링 제외 일반 변수들만 받아들이도록 하는 concept 조건
	template<typename T>
	concept IsSingleVariable = requires
	{
		std::is_arithmetic_v<T>;	// 변수형
		std::is_enum_v<T>;			// enum형
		// string/string_view, wstring/wstring_view 제외
		typeid(T).name() != typeid(std::string).name();
		typeid(T).name() != typeid(std::string_view).name();
		typeid(T).name() != typeid(std::wstring).name();
		typeid(T).name() != typeid(std::wstring_view).name();
	};

	class CPacket
	{
	public:
		enum : uint16_t
		{
			// 버퍼의 최대 크기는 4MB로 고정
			BUFF_SIZE = 4096,
			// 헤더 크기(클라이언트 id 4바이트 + 패킷 사이즈 2바이트)
			HEADER_SIZE = 6,
			// 패킷 사이즈를 작성하는 오프셋(클라이언트 id가 4바이트이니까 4로 설정)
			PACKET_SIZE_OFFSET = 4
		};
	public:
		CPacket();
		~CPacket();

		//bool ValidWriteSize(uint32_t size);
		//bool ValidReadSize(uint32_t size);

		void WriteID(int32_t id);
		template<typename T>
		void Write(T data) requires IsSingleVariable<T>;
		void WriteString(const std::string& data);
		void WriteWString(const std::wstring& data);
		void WriteSize();

		int32_t ReadID();
		template<typename T>
		T Read() requires IsSingleVariable<T>;
		void ReadString(std::string& str);
		void ReadWString(std::wstring& wstr);

		constexpr int32_t GetID() const { return m_id; }
		constexpr uint16_t GetDataSize() const { return m_size; }
		constexpr uint16_t GetPacketSize() const { return m_size + HEADER_SIZE; }
		constexpr uint32_t GetRemainDataSize() const { return m_size - m_readOffset; }
		constexpr std::array<char, BUFF_SIZE>& GetPacketData() { return m_data; }
		char* GetPacketAddr() { return m_data.data(); }

		void SetData(int32_t id);
		void SetData(char* data);
		void SetData(std::array<char, BUFF_SIZE>::iterator recvIter, int32_t recvDataSize, int32_t prevSize);
	private:
		const uint16_t ReadSize();

	private:
		uint16_t m_size;		// Header를 제외한 데이터만의 크기
		TYPE m_type;
		int32_t m_id;

		std::array<char, BUFF_SIZE> m_data;
		uint32_t m_readOffset;
		uint32_t m_writeOffset;
	};

	// 템플릿으로 작성해서 각 자료형에 따른 함수 자동생성
	// requires절 사용으로 일반 변수형만 이용 가능
	template<typename T>
	void CPacket::Write(T data) requires IsSingleVariable<T>
	{
		int32_t size{ sizeof(data) };

		//if (ValidWriteSize(size) == false)
		//	return;

		// 기본 헤더 다음 위치 + 쓰기 오프셋의 위치에 작성
		std::memcpy(m_data.data() + (HEADER_SIZE + m_writeOffset), &data, size);
		// 작성한 변수 크기만큼 오프셋 이동
		m_writeOffset += size;
		// 패킷 데이터 사이즈 추가
		m_size += size;

		// 패킷 데이터 사이즈 작성
		WriteSize();
	}

	// 템플릿으로 작성해서 각 자료형에 따른 함수 자동생성
// requires절 사용으로 일반 변수형만 이용 가능
	template<typename T>
	T CPacket::Read() requires IsSingleVariable<T>
	{
		int32_t size{ sizeof(T) };

		//if (ValidReadSize(size) == false)
		//	return T{};

		T read{};
		// 기본 헤더 다음 위치 + 읽기 오프셋의 위치에서 읽기
		std::memcpy(&read, m_data.data() + (HEADER_SIZE + m_readOffset), size);
		// 읽은 변수 크기만큼 오프셋 이동
		m_readOffset += size;

		// 데이터 반환
		return read;
	}
}