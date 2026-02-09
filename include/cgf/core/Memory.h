#pragma once

#include <type_traits>

#include "core/Common.h"

#define ENABLE_SMART_PTR_TRACING false

#if ENABLE_SMART_PTR_TRACING

#define CGF_LOG_TRACE(name, msg) CGF_INFO("[" + name + "] " msg)

#else

#define CGF_LOG_TRACE(name, msg) 

#endif


template<typename ManagedT>
class SharedPtr
{	
	struct ManagedObjectHandle
	{
		ManagedObjectHandle(ManagedT* object)
			: Object(object)
		{

		}

#if ENABLE_SMART_PTR_TRACING
		ManagedObjectHandle(ManagedT* object, std::string name)
			: Object(object), Name(name)
		{

		}
#endif

		~ManagedObjectHandle()
		{
			CGF_LOG_TRACE(Name, "Destroyed ref");
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

#if ENABLE_SMART_PTR_TRACING
		std::string Name = "?";
#endif

		int RefCount = 0;
		ManagedT* Object;
	};

public:
	SharedPtr()
	{
		
	}

	explicit SharedPtr(ManagedT* object)
	{
		m_Ref = new ManagedObjectHandle(object);
		m_Ref->IncrementRefCnt();
	}

#if ENABLE_SMART_PTR_TRACING
	explicit SharedPtr(ManagedT* object, std::string name)
	{
		m_Ref = new ManagedObjectHandle(object, name);
		m_Ref->IncrementRefCnt();
	}
#endif

	explicit SharedPtr(ManagedObjectHandle* handle)
	{
		m_Ref = handle;
		m_Ref->IncrementRefCnt();
	}

	SharedPtr(const SharedPtr<ManagedT>& other)
	{
		if(m_Ref)
			m_Ref->DecrementRefCnt();

		m_Ref = other.m_Ref;

		if(m_Ref)
			m_Ref->IncrementRefCnt();
	}

	~SharedPtr()
	{
		if(m_Ref)
		{
			m_Ref->DecrementRefCnt();

			if(m_Ref->RefCount <= 0)
			{
				delete m_Ref;
			}
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

		if (other.m_Ref)
			other.m_Ref->IncrementRefCnt();

		m_Ref = other.m_Ref;

		return *this;
	}

	SharedPtr<ManagedT>& operator=(const nullptr_t& other)
	{
		if(m_Ref)
			m_Ref->DecrementRefCnt();
		
		m_Ref = nullptr;

		return *this;
	}

	ManagedT& operator*()
	{
		CGF_ASSERT(Valid(), "Cannot dereference nullptr");
		
		return *(ManagedT*)m_Ref->Object;
	}

	FORCEINLINE ManagedT* GetRaw()
	{
		return m_Ref->Object;
	}

	template<typename TargetManagedT>
	operator SharedPtr<TargetManagedT>() const
	{
		static_assert(std::is_base_of_v<TargetManagedT, ManagedT>, 
			"Cannot convert to an underived type");

		return { m_Ref };
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
		
		return (ManagedT*)(m_Ref->Object);
	}

private:
	ManagedObjectHandle* m_Ref = nullptr;
};