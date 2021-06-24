#include "Profile.h"

shared_ptr<Profile> Profile::single = nullptr;

shared_ptr<Profile> Profile::Get()
{
	if (nullptr == single)
		single = make_shared<Profile>();

	return single;
}

const vector<ProfileResult>& Profile::GetProfileResults()
{
	shared_ptr<Profile> single = Get();
	return single->profiles;
}

void Profile::ClearData()
{
	shared_ptr<Profile> single = Get();
	single->profiles.clear();
}

void Profile::AddProfileData(const ProfileResult& data)
{
	shared_ptr<Profile> single = Get();
	single->profiles.push_back(data);
}