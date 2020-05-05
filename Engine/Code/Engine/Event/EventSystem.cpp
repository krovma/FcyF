#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Event/EventSystem.hpp"

//////////////////////////////////////////////////////////////////////////
//STATIC std::map<std::string, EventSubscribers> EventSystem::s_events;
EventSystem* g_Event = new EventSystem();
////////////////////////////////
EventSystem::~EventSystem()
{
	Shutdown();
}

////////////////////////////////
void EventSystem::Startup()
{

}

////////////////////////////////
void EventSystem::Shutdown()
{
	//Currently nothing
	//All subscription has an auto/static storage duration
}

////////////////////////////////
void EventSystem::BeginFrame()
{
	Trigger("BeginFrame");
}

////////////////////////////////
void EventSystem::EndFrame()
{
	Trigger("EndFrame");
}

////////////////////////////////
void EventSystem::SubscribeEventCallback(const std::string& event, EventCallback callback)
{
	m_events[event].push_back(EventSubscription(callback));
}

////////////////////////////////
void EventSystem::UnsubscribeEventCallback(const std::string& event, EventCallback callback)
{
	const EventSubscription tmp(callback);
	const auto eventFound = m_events.find(event);
	if (eventFound != m_events.end()) {
		EventSubscriptionList subscribers = eventFound->second;
		for (auto subscriptionIter = subscribers.begin(); subscriptionIter != subscribers.end(); ++subscriptionIter) {
			if (*subscriptionIter == tmp) {
				subscribers.erase(subscriptionIter);
				break;
			}
		}
	}
}

////////////////////////////////
int EventSystem::Trigger(const std::string& event)
{
	int eventTriggered = 0;
	EventParam emptyParam;
	const auto eventFound = m_events.find(event);
	if (eventFound != m_events.end()) {
		EventSubscriptionList subscribers = eventFound->second;
		for (auto& eachSubscription : subscribers) {
			const bool isConsumed = eachSubscription.Call(emptyParam);
			++eventTriggered;
			if (isConsumed) {
				break;
			}
		}
	}
	return eventTriggered;
}

////////////////////////////////
int EventSystem::Trigger(const std::string& event, EventParam& param)
{
	int eventTriggered = 0;
	const auto eventFound = m_events.find(event);
	if (eventFound != m_events.end()) {
		EventSubscriptionList subscribers = eventFound->second;
		for (auto& eachSubscription : subscribers) {
			const bool isConsumed = eachSubscription.Call(param);
			++eventTriggered;
			if (isConsumed) {
				break;
			}
		}
	}
	return eventTriggered;

}

////////////////////////////////
std::vector<std::string> EventSystem::GetAllEventNames() const
{
	std::vector<std::string> allKey;
	allKey.reserve(m_events.size());
	for (const auto& eachEvent : m_events) {
		allKey.push_back(eachEvent.first);
	}
	return allKey;
}
//
// ////////////////////////////////
// EventSubscription::EventSubscription(EventCallback callback)
// 	:m_callback(callback)
// {
// }
//
// ////////////////////////////////
// const EventSubscription& EventSubscription::operator=(const EventSubscription& copy)
// {
// 	m_callback = copy.m_callback;
// 	return *this;
// }
//
// ////////////////////////////////
// bool EventSubscription::operator==(const EventSubscription& compareWith) const
// {
// 	return m_callback.target<bool(*)(EventParam&)>() == compareWith.m_callback.target<bool(*)(EventParam&)>();
// }
