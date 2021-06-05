#pragma once

#include "CoreMinimal.h"
#include "Log.h"

#define BIT(x) (1 << x)

enum class EventType
{
	None = 0,

	//Wiondow
	WindowClose, WindowResize, WindowMove,

	//Game
	GameTick, GameUpdate, GameRender,

	//keyboard
	KeyPressed, KeyReleased,

	//mouse
	MousePressed, MouseReleased, MouseMove,
};

enum EventCategory
{
	None = 0,
	EventCategoryGame = BIT(0),
	EventCategoryInput = BIT(1),
	EventCategoryKeybord = BIT(2),
	EventCategoryMouse = BIT(3),
	EventCategoryMouseButton = BIT(4),
};

enum class MouseKey
{
	None = 0,
	LeftButton = 1,
	MiddleButton = 2,
	RightButton = 3
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
							   virtual EventType GetEventType() override { return GetStaticType(); } \
							   virtual char* GetName() const override {return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() override { return category; }

class Event
{
public:
	virtual ~Event() = default;
	virtual EventType GetEventType() = 0;
	virtual char* GetName() const = 0;
	virtual int GetCategoryFlags() = 0;
	virtual string ToString() const { return GetName(); }

	bool bHandled = false;

	//一个事件可能属于多个事件类别
	bool IsInCategory(EventCategory category)
	{
		return GetCategoryFlags() & (int)category;
	}
};

class EventDispatcher
{
private:
	Event& event;

public:
	EventDispatcher(Event& inEvent) : event(inEvent)
	{
	}

	template<typename T, typename F>
	bool Dispath(const F& func)
	{
		if (event.GetEventType() == T::GetStaticType())
		{
			event.bHandled = func(static_cast<T&>(event));
			return true;
		}

		return false;
	}
};


inline ostream& operator<<(ostream& os, const Event& event)
{
	return os << event.ToString();
}