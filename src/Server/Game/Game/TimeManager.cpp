#include "pch.h"
#include "TimeManager.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ImplementSingletone(TimeManager);

TimeManager::TimeManager()
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

double TimeManager::GetElapsedTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return (currentTime.QuadPart - m_startTime) * m_invFrequency;
}

void TimeManager::Reset()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    m_startTime = currentTime.QuadPart;
}





