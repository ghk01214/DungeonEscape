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

	//물리보간 관련
public:
	void SetAccumulated(float value);
	float GetAccumulated();
	int HandleAccumulated();				//accumulate값을 조정하고, 남은 값을 accumulate값에 저장. 나눗셈값을 리턴.
private:
	float m_fixedTimeScale = 0.0017f;
	float m_accumulateTime = 0.f;
};
