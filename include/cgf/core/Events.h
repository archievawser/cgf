#pragma once

#include <vector>
#include <type_traits>
#include <functional>

#include "core/Memory.h"


/**
 * @brief An event compatible with both function & member function pointers.
 */
template<typename... EventArgType>
class Event
{
public:
	Event() = default;

	struct Listener
	{
		Listener(Event *owner, std::function<void(EventArgType...)> callback)
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
		std::function<void(EventArgType...)> Callback;
	};

	void Invoke(EventArgType... eventData)
	{
		for (Listener* listener : m_Listeners)
		{
			listener->Callback(eventData...);
		}
	}

	[[nodiscard]] SharedPtr<Listener> Connect(void (*callback)(EventArgType...))
	{
		Listener *newListener = new Listener(this, callback);
		m_Listeners.push_back(newListener);
		return SharedPtr<Listener>(newListener); 
	}

	template <typename ObjectType>
	[[nodiscard]] SharedPtr<Listener> Connect(ObjectType *object, void (ObjectType::*callback)(EventArgType...))
	{
		Listener* newListener = new Listener(this, [object, callback](EventArgType... data) 
		{
			(object->*callback)(data);
		});
		
		m_Listeners.push_back(newListener);
		return SharedPtr<Listener>(newListener);
	}

private:
	std::vector<Listener*> m_Listeners;
};