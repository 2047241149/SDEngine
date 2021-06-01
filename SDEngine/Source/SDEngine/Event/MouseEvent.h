#pragma once
#include "EventBase.h"

class MouseMovedEvent : public Event
{
private:
	float mouseX, mouseY;

public:
	MouseMovedEvent(float inMouseX, float inMouseY):
		mouseX(inMouseX),
		mouseY(inMouseY)
	{
	}

	float GetMouseX() { return mouseX; }
	float GetMouseY() { return mouseY; }

	string ToString() const override
	{
		stringstream ss;
		ss << "MouseMovedEvent: " << mouseX << ", " << mouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMove)
	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
};

class MouseButtonEvent : public Event 
{
protected:
	MouseKey mousekey;

	MouseButtonEvent(MouseKey inMousekey):
		mousekey(inMousekey)
	{
	}

public:
	MouseKey GetMouseKey() { return mousekey; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
};


class MouseButtonPressedEvent : public MouseButtonEvent
{
private:
	float mouseX, mouseY;

public:
	MouseButtonPressedEvent(MouseKey inMousekey, float inMouseX, float inMouseY) :
		MouseButtonEvent(inMousekey),
		mouseX(inMouseX),
		mouseY(inMouseY)
	{
	}

	float GetMouseX() { return mouseX; }

	float GetMouseY() { return mouseY; }

	string ToString() const override
	{
		stringstream ss;
		ss << "MouseButtonPressedEvent: " << (int)mousekey;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MousePressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(MouseKey inMousekey) :
		MouseButtonEvent(inMousekey)
	{

	}

	string ToString() const override
	{
		stringstream ss;
		ss << "MouseButtonReleasedEvent: " << (int)mousekey;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseReleased)
};
