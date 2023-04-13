#pragma once
template <class T>
inline void SafeDelete(T*& pointer)
{
	if (pointer)
	{
		delete pointer;
		pointer = nullptr;
	}
}

template <class T>
inline void SafeDeleteArray(T*& arrayBegin)
{
	if (arrayBegin)
	{
		delete[] arrayBegin;
		arrayBegin = nullptr;
	}
}

template<typename T>
void SafeRelease(T*& pInstance)
{
	if (nullptr != pInstance)
	{
		pInstance->Release();
		delete pInstance;
		pInstance = nullptr;
	}
}

//class CTagFinder
//{
//public:
//	explicit CTagFinder(const wchar_t* pTag)
//		: m_pTargetTag(pTag) {}
//	~CTagFinder() {}
//
//public:
//	template<typename T>
//	bool operator()(const T& Pair)
//	{
//		if (0 == lstrcmp(m_pTargetTag, Pair.first))
//			return true;
//
//		return false;
//	}
//private:
//	const wchar_t* m_pTargetTag;
//};
