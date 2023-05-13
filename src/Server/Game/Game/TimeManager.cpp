#include "pch.h"
#include "TimeManager.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ImplementSingletone(TimeManager);

TimeManager::TimeManager() :
    m_frameCount{ 0 },
    m_frameCountIn1s{ 0 },
    m_frameTime{0.f},
    m_prevCount{0},
    m_deltaTime{0.f},
    m_fps{0}
{
}

TimeManager::~TimeManager()
{
	Release();
}

void TimeManager::Init()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    m_invFrequency = 1.0 / frequency.QuadPart;

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    m_startTime = currentTime.QuadPart;
}

void TimeManager::Release()
{
}

void TimeManager::Reset()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    m_startTime = currentTime.QuadPart;
}

void TimeManager::Update()
{
    uint64_t currentCount;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

    m_deltaTime = (currentCount - m_prevCount) * m_invFrequency;
    m_prevCount = currentCount;

    ++m_frameCount;
    ++m_frameCountIn1s;
    m_frameTime += m_deltaTime;
    m_deltaTimeInVar += m_deltaTime;

    if (m_frameTime > 1.f)
    {
        m_fps = static_cast<uint32_t>(m_frameCountIn1s / m_frameTime);

        m_frameTime = 0.f;
        m_frameCount = 0;
        m_frameCountIn1s = 0;
    }
}

const bool TimeManager::Is1FrameInVar()
{
    if (m_fps > 0.f and m_frameCount > m_fps / 60)
    {
        m_frameCount = 0;
        return true;
    }

    return false;
}
