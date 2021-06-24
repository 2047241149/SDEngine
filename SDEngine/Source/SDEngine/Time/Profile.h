#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <memory>
using namespace std;
using namespace std::chrono;

struct ProfileResult
{
	string name;
	float time;
};

class Profile
{
private:
	vector<ProfileResult> profiles;
	static shared_ptr<Profile> single;

public:
	static shared_ptr<Profile> Get();
	static const vector<ProfileResult>& GetProfileResults();
	static void ClearData();
	static void AddProfileData(const ProfileResult& data);
};


class Timer
{
private:
	string name;
	bool bStop;
	time_point<steady_clock> startTimePoint;

public:
	Timer(string inName) :
		name(inName),
		bStop(false)
	{
		startTimePoint = high_resolution_clock::now();
	}

	~Timer()
	{
		if (!bStop)
			Stop();
	}

	void Stop()
	{
		auto endTimePoint = high_resolution_clock::now();
		long long start = time_point_cast<microseconds>(startTimePoint).time_since_epoch().count();
		long long end = time_point_cast<microseconds>(endTimePoint).time_since_epoch().count();

		bStop = true;
		float duration = (end - start) * 0.001f;
		Profile::AddProfileData({ name, duration });
	}
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name)
#define PROFILE_FUNC() PROFILE_SCOPE(__FUNCTION__)