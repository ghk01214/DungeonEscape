#pragma once

#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <iostream>
#include <format>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>
#include <memory.h>
#include <assert.h>

#include <array>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <algorithm>

#include <thread>
#include <atomic>
#include <shared_mutex>

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_priority_queue.h>

#include <WS2tcpip.h>
#include <MSWSock.h>

#include <Define.h>
#include <protocol.hpp>
#include <Packet.h>
#include <OVERLAPPEDEX.h>

#include <PxPhysicsAPI.h>

#include "game_macro.h"
#include "game_functions.h"
#include "transform_utils.h"

#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

#define SINGLE(type)				\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\

#define GET_SINGLE(type)	type::GetInstance()
#define GET_SCENE			GET_SINGLE(game::CSceneManager)

using int8 = char;
using uchar_t = unsigned char;

template<typename T, typename U>
using Accessor = tbb::concurrent_hash_map<T, U>::accessor;

template<typename T, typename U>
using ConstAccessor = tbb::concurrent_hash_map<T, U>::const_accessor;

template<typename T, typename U>
using HashMap = tbb::concurrent_hash_map<T, U>;

extern std::default_random_engine dre;

void ErrorQuit(const std::wstring_view& msg);
void ErrorDisplay(const std::wstring_view& msg);