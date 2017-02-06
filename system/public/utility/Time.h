#ifndef _TIME_H
#define _TIME_H

#include "DemoTypes.h"
#include "DemoCommon.h"

#define MILLISECONDS_2_SECONDS 0.001f

class Time
{
public:

	// initializes the time data, should only be called by DemoSystem, not by the user
	static void Start()
	{
		SetDeltaTime(0.0f);
		SetTime(0.0f);
		SetFrameCount(0);
		SetLastTime(MillisecondsNow());
	}

	// updates the time, should only be called by DemoSystem once per frame, not by the user
	static void UpdateTime()
	{
		// calculate time since last update, and update the total time
		int64 now = MillisecondsNow();
		SetDeltaTime(GetElapsedTime(now));
		SetTime(GetTime() + GetDeltaTime());

		// increment frame counter and set current time for next update
		SetFrameCount(GetFrameCount() + 1);
		SetLastTime(now);
	}

	// returns the real time since the application started in seconds
	static float GetRealTime()
	{
		return GetTime() + GetElapsedTime(MillisecondsNow());
	}

	// amount of time passed since the last update in seconds
	static float deltaTime() { return GetDeltaTime(); }

	// time at the start of the last update in seconds
	static float time() { return GetTime(); }

	// number of updates since the application started
	static uint32 frameCount() { return GetFrameCount(); }

private:

	// amount of time passed since the last update in seconds
	static float& GetDeltaTime() { static float dt; return dt; }
	static void SetDeltaTime(float v) { float& dt = GetDeltaTime(); dt = v; }

	// time at the start of the last update in seconds
	static float& GetTime() { static float t; return t; }
	static void SetTime(float v) { float& t = GetTime(); t = v; }

	// number of updates since the application started
	static uint32& GetFrameCount() { static uint32 fc; return fc; }
	static void SetFrameCount(uint32 v) { static uint32& fc = GetFrameCount(); fc = v; }

	// the time at which the last UpdateTime method was called
	static int64& GetLastTime() { static int64 lt; return lt; };
	static void SetLastTime(int64 v) { int64& lt = GetLastTime(); lt = v; }

	static float GetElapsedTime(int64 now)
	{
		return static_cast<float>(now - GetLastTime()) * MILLISECONDS_2_SECONDS;
	}

	// #if define(_PLATFORM_WIN32)
	// #elif define(_PLATFORM_LINUX)
	static int64 MillisecondsNow()
	{
		static LARGE_INTEGER frequency;
		static BOOL useQPC = QueryPerformanceFrequency(&frequency);

		if (useQPC)
		{
			LARGE_INTEGER now;
			QueryPerformanceCounter(&now);
			return (1000LL * now.QuadPart) / frequency.QuadPart;
		}
		else
		{
			return GetTickCount();
		}
	}
};

#endif // _TIME_H