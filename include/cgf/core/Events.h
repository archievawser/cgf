#pragma once

#include <vector>
#include <type_traits>
#include <functional>

#include "core/Memory.h"


/**
 * @brief An event capable of binding both function & member function pointers.
 */
template<typename... EventArgT>
class Event
{
public:
	Event() = default;

	struct Listener
	{
		Listener(Event *owner, std::function<void(EventArgT...)> callback)
			: Owner(owner), Callback(callback)
		{
			
		}

		~Listener()
		{
			Disconnect();
		}

		void Disconnect()
		{
			for (int i = 0; i < Owner->m_Listeners.size(); i++)
			{
				if (Owner->m_Listeners[i] == this)
				{
					Owner->m_Listeners.erase(Owner->m_Listeners.begin() + i);
				}
			}
		}

		Event* Owner;
		std::function<void(EventArgT...)> Callback;
	};

	void Invoke(EventArgT... eventData)
	{
		for (Listener* listener : m_Listeners)
		{
			listener->Callback(eventData...);
		}
	}

	[[nodiscard]] SharedPtr<Listener> Connect(void (*callback)(EventArgT...))
	{
		Listener* newListener = new Listener(this, callback);
		m_Listeners.push_back(newListener);

		return SharedPtr<Listener>(newListener); 
	}

	template <typename ObjectT>
	[[nodiscard]] SharedPtr<Listener> Connect(ObjectT *object, void (ObjectT::*callback)(EventArgT...))
	{
		Listener* newListener = new Listener(this, [object, callback](EventArgT... data) 
		{
			(object->*callback)(data...);
		});
		
		m_Listeners.push_back(newListener);
		return SharedPtr<Listener>(newListener);
	}

	typedef SharedPtr<Listener> ConnectionT;

private:
	std::vector<Listener*> m_Listeners;
};


typedef Event<> OnStartEvent;
typedef Event<double> OnUpdateEvent;