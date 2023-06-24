#pragma once

#define GET_INSTANCE(CLASSNAME)	[](){					\
	CLASSNAME*	instance = CLASSNAME::GetInstance();

#define NO_COPY(CLASSNAME)								\
	private:											\
	CLASSNAME(const CLASSNAME&);						\
	CLASSNAME& operator = (const CLASSNAME&);


#define DeclareSingletone(CLASSNAME)					\
	NO_COPY(CLASSNAME)									\
	private:											\
	static CLASSNAME*	instance;						\
	public:												\
	static CLASSNAME*	GetInstance();					\
	void DestroyInstance();


#define ImplementSingletone(CLASSNAME)					\
	CLASSNAME*	CLASSNAME::instance = nullptr;			\
	CLASSNAME*	CLASSNAME::GetInstance()				\
	{													\
		if(nullptr == instance)							\
		{												\
			instance = new CLASSNAME;					\
		}												\
		return instance;								\
	}													\
	void CLASSNAME::DestroyInstance()					\
	{													\
		if(nullptr != instance)							\
		{												\
			instance->Release();						\
			delete instance;							\
			instance = nullptr;							\
		}												\
	}