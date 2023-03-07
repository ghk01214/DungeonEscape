﻿#pragma once

#include <iostream>
#include <format>
#include <chrono>
#include <algorithm>
#include <random>

#include <array>
#include <vector>
#include <set>
#include <deque>
#include <queue>

#include <boost/asio.hpp>

#include <thread>
#include <atomic>

#include <Define.h>
#include <protocol.hpp>

#define SKIP_STEADY_CLOCK_NAMESPACE using std::chrono::steady_clock
#define SKIP_TCP_NAMESPACE using boost::asio::ip::tcp
#define SKIP_UDP_NAMESPACE using boost::asio::ip::tcp

using uchar_t = unsigned char;

extern std::default_random_engine dre;