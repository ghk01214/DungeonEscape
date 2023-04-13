#include "pch.h"
#include "TimeDevice.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

ImplementSingletone(TimeDevice);

TimeDevice::TimeDevice()
{
}

TimeDevice::~TimeDevice()
{
	Release();
}

void TimeDevice::Init()
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    m_invFrequency = 1.0 / frequency.QuadPart;

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    m_startTime = currentTime.QuadPart;
}

void TimeDevice::Release()
{
}

double TimeDevice::GetElapsedTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return (currentTime.QuadPart - m_startTime) * m_invFrequency;
}

void TimeDevice::Reset()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    m_startTime = currentTime.QuadPart;
}





