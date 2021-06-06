#pragma once
#include "EventBase.h"

class MousePosEvent : public Event
{
protected:
	float mouseX, mouseY;

public:
	MousePosEvent(float inMouseX, float inMouseY):
		mouseX(inMouseX),
		mouseY(inMouseY)
	{
	}

	float GetMouseX() { return mouseX; }
	float GetMouseY() { return mouseY; }

	EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
};


class MouseMovedEvent : public MousePosEvent
{
public:
	MouseMovedEvent(float inMouseX, float inMouseY) :
		MousePosEvent(inMouseX, inMouseY)
	{

	}

	string ToString() const override
	{
		stringstream ss;
		ss << "MouseMovedEvent: " << mouseX << ", " << mouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseMove)
};

class MouseScrollEvent : public MousePosEvent
{
public:
	MouseScrollEvent(float inMouseX, float inMouseY) :
		MousePosEvent(inMouseX, inMouseY)
	{

	}

	string ToString() const override
	{
		stringstream ss;
		ss << "MouseScrollEvent: " << mouseX << ", " << mouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(MouseScroll)
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
