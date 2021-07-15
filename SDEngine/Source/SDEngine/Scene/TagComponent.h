#pragma once
#include<string>
using std::string;

class TagComponent
{
public:
	TagComponent() = default;
	TagComponent(const TagComponent& other) = default;
	TagComponent(string inTag) :
		tag(inTag)
	{

	}

public:
	string tag;
};