#pragma once
#include <vector>
#include <string>
#include <map>
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Event/EventProc.hpp"

/*class EventSubscription
{
	friend class EventSystem;
public:
	bool operator==(const EventSubscription& compareWith) const;
	const EventSubscription& operator=(const EventSubscription& copy);
private:
	EventSubscription(EventCallback callback);
private:
	EventCallback m_callback = nullptr;
};*/
using EventSubscription = EventProc;

using EventSubscriptionList = std::vector<EventSubscription>;

class EventSystem
{
public:
	EventSystem() = default;
	~EventSystem();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallback(const std::string& event, EventCallback callback);
	template<typename T>
	void SubscribeEventCallback(const std::string& event, T* object, bool (T::*method)(EventParam&))
	{
		m_events[event].push_back(EventSubscription(object, method));
	}
	void UnsubscribeEventCallback(const std::string& event, EventCallback callback);
	template<typename T>
	void UnsubscribeEventCallback(const std::string& event, T* object, bool (T::*method)(EventParam&))
	{
		const EventSubscription tmp(object, method);
		const auto& eventFound = m_events.find(event);
		if (eventFound != m_events.end()) {
			EventSubscriptionList& subscribers = eventFound->second;
			for (auto subscriptionIter = subscribers.begin(); subscriptionIter != subscribers.end(); ++subscriptionIter) {
				if (*subscriptionIter == tmp) {
					subscribers.erase(subscriptionIter);
					break;
				}
			}
		}
	}
	int Trigger(const std::string& event);
	int Trigger(const std::string& event, EventParam& param);
	std::vector<std::string> GetAllEventNames() const;
	
private:
	std::map<std::string, EventSubscriptionList> m_events;
};

extern EventSystem* g_Event;