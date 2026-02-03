#pragma once

#include <type_traits>

#include "core/Common.h"

#define ENABLE_MANAGED_MEMORY_TRACING true	


struct ManagedObjectHandle
{
	ManagedObjectHandle(void* object)
		: Object(object)
	{

	}

	~ManagedObjectHandle()
	{
		delete Object;
	}

	void IncrementRefCnt()
	{
		RefCount++;
	}

	void DecrementRefCnt()
	{
		RefCount--;
	}

#if ENABLE_MANAGED_MEMORY_TRACING
	std::string Name;
#endif

	int RefCount = 0;
	void* Object;
};


template<typename ManagedT>
class SharedPtr
{	

public:
	SharedPtr()
	{
		
	}

	SharedPtr(ManagedT* object)
	{
		m_Ref = new ManagedObjectHandle(object);
		m_Ref->IncrementRefCnt();
	}

	SharedPtr(ManagedObjectHandle* handle)
	{
		m_Ref = handle;
		m_Ref->IncrementRefCnt();
	}

	SharedPtr(const SharedPtr<ManagedT>& other)
	{
		if(m_Ref)
			m_Ref->DecrementRefCnt();

		m_Ref = other.m_Ref;
		m_Ref->IncrementRefCnt();
	}

	~SharedPtr()
	{
		m_Ref->DecrementRefCnt();

		if(m_Ref->RefCount <= 0)
		{
			delete m_Ref;
		}
	}
	
	FORCEINLINE bool Valid() const
	{
		return m_Ref;
	}

	SharedPtr<ManagedT>& operator=(const SharedPtr<ManagedT>& other)
	{
		if(m_Ref)
			m_Ref->DecrementRefCnt();

		m_Ref = other.m_Ref;
		m_Ref->IncrementRefCnt();

		return *this;
	}

	template<typename TargetManagedT>
	operator SharedPtr<TargetManagedT>() const
	{
		static_assert(std::is_base_of_v<TargetManagedT, ManagedT>, 
			"Attempted to convert to an incompatible SharedPtr type, SharedPtr type conversion is only supported from derived to base types");

		return { m_Ref };
	}

	template<typename... ConstructorArgType>
	static SharedPtr<ManagedT> Create(ConstructorArgType&&... args)
	{
		ManagedT* ref = new ManagedT(args...);
		return SharedPtr<ManagedT>(ref);
	}

#if ENABLE_MANAGED_MEMORY_TRACING
	template<typename... ConstructorArgType>
	static SharedPtr<ManagedT> CreateTraced(std::string&& name, ConstructorArgType&&... args)
	{
		ManagedT* ref = new ManagedT(args...);
		SharedPtr<ManagedT> managedRef = SharedPtr<ManagedT>(ref);
		managedRef.m_Ref->Name = name;

		return managedRef;
	}
#else
	template <typename... ConstructorArgType>
	static SharedPtr<ManagedT> CreateTraced(std::string &&name, ConstructorArgType &&...args)
	{
		return Create(args...);
	}
#endif

	ManagedT* operator->() const
	{
		return (ManagedT*)(m_Ref->Object);
	}

private:
	ManagedObjectHandle* m_Ref = nullptr;
};