#pragma once

class TimeDevice
{
public:
	DeclareSingletone(TimeDevice);

private:
    TimeDevice();
	~TimeDevice();
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

