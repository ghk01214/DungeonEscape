#pragma once
#include <shared_mutex>

template<typename T>
class Singleton
{
protected:
	Singleton() = default;
	virtual ~Singleton() = default;

	Singleton(const Singleton& other) = delete;
	Singleton& operator=(const Singleton& other) = delete;
public:
	virtual void Init() {}

	static std::unique_ptr<T>& inst()
	{
		std::call_once(_once, []{ _inst = std::make_unique<T>(); });

		return _inst;
	}

	static void release()
	{
		if (_inst != nullptr)
			_inst.reset();
	}

	static void swap(T* new_inst)
	{
		_inst.reset(new_inst);
	}

private:
	static std::unique_ptr<T> _inst;
	static std::once_flag _once;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::_inst{ nullptr };

template<typename T>
std::once_flag Singleton<T>::_once{};