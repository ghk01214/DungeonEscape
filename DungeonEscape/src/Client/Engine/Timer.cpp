#include "pch.h"
#include "Timer.h"

void CTimer::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&m_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&m_prevCount)); // CPU 클럭
}

void CTimer::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	m_deltaTime = (currentCount - m_prevCount) / static_cast<float>(m_frequency);	// (currentCount - m_prevCount) -> 경과된 시간(cpu 클럭), m_frequency -> 경과된 시간(초당 움직이는 cpu 클럭 수)
	m_prevCount = currentCount;

	m_frameCount++;
	m_frameTime += m_deltaTime;

	if (m_frameTime > 1.f)
	{
		m_fps = static_cast<uint32>(m_frameCount / m_frameTime);

		m_frameTime = 0.f;
		m_frameCount = 0;
	}
}