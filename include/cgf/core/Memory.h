#pragma once

#include <type_traits>
#include <unordered_map>
#include <algorithm>

#include "core/Common.h"
#include "core/Events.h"

#define ENABLE_SMART_PTR_TRACING false

#if ENABLE_SMART_PTR_TRACING

#define CGF_LOG_TRACE(name, msg) CGF_INFO("[" + name + "] " msg)

#else

#define CGF_LOG_TRACE(name, msg) 

#endif	


template<typename ManagedT>
struct SharedObjectHandle
{
	SharedObjectHandle(ManagedT* object)
		: Object(object)
	{

	}

#if ENABLE_SMART_PTR_TRACING
	SharedObjectHandle(ManagedT* object, std::string name)
		: Object(object), Name(name)
	{

	}
#endif

	~SharedObjectHandle()
	{
		CGF_LOG_TRACE(Name, "Destroyed ref");

		OnDestruction.Invoke();

		delete Object;
	}

	void IncrementRefCnt()
	{
		CGF_LOG_TRACE(Name, "Gained ref");

		RefCount++;
	}

	void DecrementRefCnt()
	{
		CGF_LOG_TRACE(Name, "Lost ref");

		RefCount--;
	}

	int RefCount = 0;
	Notifier OnDestruction;
	ManagedT* Object;

#if ENABLE_SMART_PTR_TRACING
	std::string Name = "?";
#endif
};


template<typename ManagedT>
class SharedPtr
{	
public:
	SharedPtr()
	{
		
	}

	explicit SharedPtr(ManagedT* object)
	{
		Attach(new SharedObjectHandle<ManagedT>(object));
	}

#if ENABLE_SMART_PTR_TRACING
	explicit SharedPtr(ManagedT* object, std::string name)
	{
		Attach(new SharedObjectHandle<ManagedT>(object, name));
	}
#endif

	explicit SharedPtr(SharedObjectHandle<ManagedT>* handle)
	{
		Attach(handle);
	}

	SharedPtr(const SharedPtr<ManagedT>& other)
	{
		Attach(other.m_Ref);
	}

	~SharedPtr()
	{
		Detach();
	}

	SharedPtr<ManagedT>& operator=(const SharedPtr<ManagedT>& other)
	{
		Attach(other.m_Ref);

		return *this;
	}

	SharedPtr<ManagedT>& operator=(const nullptr_t& other)
	{
		Detach();

		return *this;
	}

	ManagedT& operator*()
	{
		CGF_ASSERT(Valid(), "Cannot dereference nullptr");
		
		return *(ManagedT*)m_Ref->Object;
	}

	template<typename TargetManagedT>
	operator SharedPtr<TargetManagedT>() const
	{
		//static_assert(std::is_base_of_v<TargetManagedT, ManagedT>, 
		//	"Cannot convert to an underived type");

		return SharedPtr<TargetManagedT>( (SharedObjectHandle<TargetManagedT>*) m_Ref );
	}

	operator bool() const
	{
		return Valid();
	}
	
	FORCEINLINE bool Valid() const
	{
		return m_Ref;
	}

	FORCEINLINE ManagedT* GetRaw()
	{
		return m_Ref->Object;
	}

	FORCEINLINE SharedObjectHandle<ManagedT>* GetManager()
	{
		return m_Ref->Object;
	}

 	Notifier* GetDestructionEvent()
	{
		return &m_Ref->OnDestruction;
	}

	template<typename... ConstructorArgT>
	static SharedPtr<ManagedT> Create(ConstructorArgT&&... args)
	{
		ManagedT* ref = new ManagedT(args...);
		return SharedPtr<ManagedT>(ref);
	}

#if ENABLE_SMART_PTR_TRACING
	template<typename... ConstructorArgT>
	static SharedPtr<ManagedT> CreateTraced(std::string&& name, ConstructorArgT&&... args)
	{
		ManagedT* ref = new ManagedT(args...);
		SharedPtr<ManagedT> managedRef = SharedPtr<ManagedT>(ref, name);

		return managedRef;
	}
#else
	template <typename... ConstructorArgT>
	static SharedPtr<ManagedT> CreateTraced(std::string &&name, ConstructorArgT &&...args)
	{
		return Create(args...);
	}
#endif

	ManagedT* operator->() const
	{
		CGF_ASSERT(Valid(), "Cannot dereference nullptr");
		
		return m_Ref->Object;
	}

protected:
	void Detach()
	{
		if(m_Ref)
		{
			m_Ref->DecrementRefCnt();

			if(m_Ref->RefCount <= 0)
			{
				delete m_Ref;
			}

			m_Ref = nullptr;
		}
	}

	void Attach(SharedObjectHandle<ManagedT>* handle)
	{
		if(m_Ref) 
		{
			Detach();
		}

		m_Ref = handle;

		if(handle)
		{
			m_Ref->IncrementRefCnt();
		}
	}

private:
	SharedObjectHandle<ManagedT>* m_Ref = nullptr;
};


template<typename T>
class Pool
{
public:
	struct PoolObject
	{
		PoolObject()
			: Valid(false), Owner(nullptr), Index(-1)
		{

		}

		PoolObject(Pool* owner, int index)
			: Owner(owner), Index(index)
		{
			
		}
		
		~PoolObject()
		{
			if(Valid)
			{
				Owner->Remove(Index);

				Valid = false;
			}
		}

		T& Get()
		{
			return Owner->At(Index);
		}

		bool Valid = true;
		Pool* Owner;
		int Index;
	};

	Pool() = default;

	T& At(int index)
	{
		return m_Data[index];
	}

	void Remove(int index)
	{
		std::swap(m_Data[index], m_Data[m_Count - 1]);
		std::swap(m_PoolObjects[index], m_PoolObjects[m_Count - 1]);
		
		m_PoolObjects[index]->Index = index;

		m_Count--;
	}

	template<typename... ConstructorArgT>
	SharedPtr<PoolObject> Create(ConstructorArgT&&... args)
	{
		if(m_Count >= m_Capacity)
		{
			Resize(m_Capacity * 2);
		}

		int itemIndex = m_Count;
		PoolObject* handle = new PoolObject(this, itemIndex);
		m_Data[itemIndex] = T(args...);
		m_PoolObjects[itemIndex] = handle;

		m_Count++;

		return SharedPtr<PoolObject>(handle);
	}

	void Resize(int newCapacity)
	{
		T* newData = new T[newCapacity];

		std::memcpy(newData, m_Data, sizeof(T) * m_Capacity);

		m_Data = newData;
		m_Capacity = newCapacity;
	}

	T* begin()
	{
		return m_Data;
	}

	T* end()
	{
		return m_Data + m_Count;
	}

private:
	int m_Capacity = 1;
	int m_Count = 0;
	T* m_Data = new T;
	PoolObject** m_PoolObjects = new PoolObject*;
};