#pragma once

#include <iostream>
#include <format>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>

#include <array>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include <thread>
#include <atomic>

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_priority_queue.h>

#include <WS2tcpip.h>
#include <MSWSock.h>

#include <Define.h>
#include <protocol.hpp>
#include <Packet.h>
#include <OVERLAPPEDEX.h>

#pragma comment(lib, "WS2_32")
#pragma comment(lib, "MSWSock")

using int8 = char;
using uchar_t = unsigned char;

template<typename T, typename U>
using Accessor = tbb::concurrent_hash_map<T, U>::accessor;

template<typename T, typename U>
using HashMap = tbb::concurrent_hash_map<T, U>;

extern std::default_random_engine dre;

void ErrorQuit(const std::wstring_view& msg);
void ErrorDisplay(const std::wstring_view& msg);