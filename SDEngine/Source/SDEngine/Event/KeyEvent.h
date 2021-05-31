#pragma once

#include "Event.h"

class KeyEvent : public Event
{
protected:
	int keyCode;

	KeyEvent(int inKeyCode) :
		keyCode(inKeyCode)
	{
	}

public:
	int GetKeyCode(){ return keyCode; }
	
	EVENT_CLASS_CATEGORY(EventCategoryKeybord) | EventCategoryInput)
};


class KeyPressedEvent : public KeyEvent
{
private:
	int repeatCount;

public:
	KeyPressedEvent(int inKeyCode, int inRepeatCount)
		:KeyEvent(inKeyCode),
		repeatCount(inRepeatCount)
	{
	}

	int GetRepeatCount() { return repeatCount; }

	string ToString() const override
	{
		stringstream ss;
		ss << "KeyPressedEvent: " << keyCode << " (" << repeatCount << " repeats)";
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyPressed)
};

class KeyPressedEvent : public KeyEvent
{

public:
	KeyPressedEvent(int inKeyCode)
		:KeyEvent(inKeyCode)
	{
	}

	string ToString() const override
	{
		stringstream ss;
		ss << "KeyPressedEvent: " << keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyReleased)
};