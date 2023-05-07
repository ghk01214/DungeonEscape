#pragma once

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
    void Reset();

	void Update();
	constexpr uint32_t GetFPS() const { return m_fps; }

	const bool Is1FrameInVar();
	const bool IsOneSec();

private:
	LONGLONG m_startTime;
	float m_invFrequency;

	float m_deltaTime;
	uint64_t m_prevCount;
	uint32_t m_frameCount;
	uint32_t m_frameCountIn1s;
	float m_frameTime;
	uint32_t m_fps;

	// 임시 변수
	uint32_t m_oneSecFrameCount;
};
