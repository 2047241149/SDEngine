#pragma once

#include "EventBase.h"

//TODO: refactor, VK_XXX should map to EkeyCode, use EkeyCode uniformly
class KeyEvent : public Event
{
protected:
	UINT keyCode;

	KeyEvent(UINT inKeyCode) :
		keyCode(inKeyCode)
	{
	}

public:
	UINT GetKeyCode(){ return keyCode; }
	
	EVENT_CLASS_CATEGORY(EventCategoryKeybord | EventCategoryInput)
};

class CharEvent : public KeyEvent
{
public:
	CharEvent(UINT inKeyCode)
		:KeyEvent(inKeyCode)
	{
	}

	string ToString() const override
	{
		stringstream ss;
		ss << "CharEvent: " << keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(Char)
};

class KeyPressedEvent : public KeyEvent
{
private:
	int repeatCount;

public:
	KeyPressedEvent(UINT inKeyCode, int inRepeatCount)
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

class KeyReleasedEvent : public KeyEvent
{

public:
	KeyReleasedEvent(UINT inKeyCode)
		:KeyEvent(inKeyCode)
	{
	}

	string ToString() const override
	{
		stringstream ss;
		ss << "KeyReleasedEvent: " << keyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE(KeyReleased)
};