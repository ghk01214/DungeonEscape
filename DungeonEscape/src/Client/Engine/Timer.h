#pragma once


class CTimer
{
	DECLARE_SINGLE(CTimer);

public:
	void Init();
	void Update();

	uint32 GetFps() { return m_fps; }
	float GetDeltaTime() { return m_deltaTime; }

private:
	uint64	m_frequency = 0;
	uint64	m_prevCount = 0;
	float	m_deltaTime = 0.f;

private:
	uint32	m_frameCount = 0;
	float	m_frameTime = 0.f;
	uint32	m_fps = 0;
};

