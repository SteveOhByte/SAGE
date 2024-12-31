#ifndef H_TIMER
#define H_TIMER

class TimerType
{
public:
	TimerType();
	void CheckTime();
	double GetTimeDeltaTime() const;
	int GetFps() const { return framesPerSecond; }

private:
	double secondsPerTick;
	double deltaTime;
	int framesPerSecond;
	__int64 ticksPerSecond;
	__int64 previousTime;
	__int64 currentTime;
};

#endif