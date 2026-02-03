#pragma once

#include <type_traits>

#include "core/Common.h"


class ManagedObject
{
	template<typename T>
	friend class SharedPtr;

public:
	ManagedObject() = default;
	ManagedObject(const ManagedObject& other) = delete;

private:
	int m_RefCount = 0;
};


template<typename ManagedT>
class SharedPtr
{
	static_assert(std::is_base_of_v<ManagedObject, ManagedT>);

public:
	SharedPtr(ManagedT* object)
		: m_Ref(object)
	{
		m_Ref->m_RefCount++;
	}

	SharedPtr(const SharedPtr<ManagedT>& other)
	{
		if(m_Ref)
			m_Ref->m_RefCount--;
		
		m_Ref = other.m_Ref;
		other.m_Ref->m_RefCount++;
	}

	~SharedPtr()
	{
		m_Ref->m_RefCount--;

		if(m_Ref->m_RefCount == 0)
		{
			CGF_LOG("Pointer decayed");
			delete m_Ref;
		}
	}

	ManagedT* operator->() const
	{
		return m_Ref;
	}

private:
	ManagedT* m_Ref = nullptr;
};