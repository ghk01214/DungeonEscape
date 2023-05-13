﻿#pragma once

class TimeManager
{
public:
	DeclareSingletone(TimeManager);

private:
    TimeManager();
	~TimeManager();
public:
	void Init();
	void Release();
public:
	float GetDeltaTime() { return m_deltaTime; }
	float GetDeltaTimeInVar() { return m_deltaTimeInVar; }
    void Reset();

	void Update();

	void ClearDeltaTimeInVar() { m_deltaTimeInVar = 0.f; }

	constexpr uint32_t GetFPS() const { return m_fps; }

	const bool Is1FrameInVar();

private:
	LONGLONG m_startTime;
	float m_invFrequency;

	float m_deltaTime;
	uint64_t m_prevCount;
	uint32_t m_frameCount;
	uint32_t m_frameCountIn1s;
	float m_frameTime;
	uint32_t m_fps;

	float m_deltaTimeInVar;
};
