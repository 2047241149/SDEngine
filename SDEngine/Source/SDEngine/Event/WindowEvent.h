#pragma once
#include "EventBase.h"

class WindowResizeEvent : public Event
{
private:
	int width, height;

public:
	WindowResizeEvent(int inWidth, int inHeight) :
		width(inWidth),
		height(inHeight)
	{
	}

	int GetWidth() { return width; }
	int GeiHeight() { return height; }

	string ToString() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << width << ", " << height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(WindowResize)
	EVENT_CLASS_CATEGORY(EventCategory::EventCategoryGame)
};


class WindoowCloseEvent : public Event
{
public:
	WindoowCloseEvent() = default;

	EVENT_CLASS_TYPE(WindowClose)
	EVENT_CLASS_CATEGORY(EventCategory::EventCategoryGame)
};


class GameTickEvent : public Event
{
public:
	GameTickEvent() = default;

	EVENT_CLASS_TYPE(GameTick)
	EVENT_CLASS_CATEGORY(EventCategoryGame)
};

class GameUpdateEvent : public Event
{
public:
	GameUpdateEvent() = default;

	EVENT_CLASS_TYPE(GameUpdate)
	EVENT_CLASS_CATEGORY(EventCategoryGame)
};

class GameRenderEvent : public Event
{
public:
	GameRenderEvent() = default;

	EVENT_CLASS_TYPE(GameRender)
	EVENT_CLASS_CATEGORY(EventCategory::EventCategoryGame)
};