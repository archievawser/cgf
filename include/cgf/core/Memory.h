#pragma once

#include <type_traits>

#include "core/Common.h"

#define ENABLE_MANAGED_MEMORY_TRACING true


template<typename ManagedT>
class SharedPtr
{
	struct ManagedObjectHandle
	{
		ManagedObjectHandle(ManagedT* object)
			: Object(object)
		{

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
		ManagedT* Object;
	};

public:
	SharedPtr()
	{
		
	}

	SharedPtr(ManagedT* object)
	{
		m_Ref = new ManagedObjectHandle(object);
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
	}
	
	FORCEINLINE bool Valid() const
	{
		return m_Ref;
	}

	SharedPtr<ManagedT>& operator=(const SharedPtr<ManagedT>& other)
	{
		m_Ref = other.m_Ref;
		m_Ref->IncrementRefCnt();

		return *this;
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
		return m_Ref->Object;
	}

private:
	ManagedObjectHandle* m_Ref = nullptr;
};