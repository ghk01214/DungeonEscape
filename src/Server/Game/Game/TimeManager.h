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
    double GetElapsedTime();
    void Reset();



private:
	LONGLONG m_startTime;
	double m_invFrequency;
};

