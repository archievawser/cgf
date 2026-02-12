#pragma once

#include <vector>
#include <type_traits>
#include <functional>

#include "core/Common.h"


template<typename T>
class SharedPtr;


/**
 * @brief An event capable of binding both function & member function pointers.
 */
template<typename... EventArgT>
class Event
{
public:
	Event() = default;

	~Event()
	{
		for(Listener* listener : m_Connections)
		{
			delete listener;
		}
	}

	struct Listener
	{
		Listener(Event* owner, std::function<void(EventArgT...)> callback)
			: Owner(owner), Callback(callback)
		{
			CGF_INFO("Listener gained");
		}

		~Listener()
		{
			Disconnect();
		}

		void Disconnect()
		{
			CGF_INFO("Listener lost");
			
			for (int i = 0; i < Owner->m_Connections.size(); i++)
			{
				if (Owner->m_Connections[i] == this)
				{
					Owner->m_Connections.erase(Owner->m_Connections.begin() + i);
				}
			}
		}

		Event* Owner;
		std::function<void(EventArgT...)> Callback;
	};

	void Invoke(EventArgT... eventData)
	{
		std::vector<Listener*> connections = m_Connections;

		for (Listener* listener : connections)
		{
			listener->Callback(eventData...);
		}
	}

	unsigned int GetNumberOfListeners()
	{
		return m_Connections.size();
	}

	typedef SharedPtr<Listener> Connection;

	template<typename LambdaT>
	Listener* Bind(LambdaT lambda)
	{
		Listener* newListener = new Listener(this, lambda);
		m_Connections.push_back(newListener);

		return newListener;
	}

	template<typename LambdaT>
	Connection Connect(LambdaT lambda)
	{
		SharedPtr<Listener> newListener = SharedPtr<Listener>::CreateTraced("EventListenerLambda", this, lambda);
		m_Connections.push_back(newListener.GetRaw());

		return newListener;
	}

	[[nodiscard]] Connection Connect(void (*callback)(EventArgT...))
	{
		Connection newListener = SharedPtr<Listener>::CreateTraced("EventListener", this, callback); 
		m_Connections.push_back(newListener.GetRaw());

		return newListener;
	}

	template <typename ObjectT>
	[[nodiscard]] Connection Connect(ObjectT *object, void (ObjectT::*callback)(EventArgT...))
	{
		Connection newListener = SharedPtr<Listener>::CreateTraced("EventListenerMemFunc", this, [object, callback](EventArgT... data) 
		{
			(object->*callback)(data...);
		}); 
		
		m_Connections.push_back(newListener.GetRaw());

		return newListener;
	}

private:
	std::vector<Listener*> m_Connections;
};


typedef Event<> Notifier;
typedef Notifier OnStartEvent;
typedef Event<double> OnTickEvent;