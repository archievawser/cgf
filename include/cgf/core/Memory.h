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


/**
 * @brief A wrapper for an arbitrarily typed, heap-allocated pointer which
 * facilitates reference counting for use with smart pointers. Upon losing all
 * references (a condition checked for every time DecrementRefCnt() is called)
 * the handle deletes itself and the pointer it references.
 * 
 * The handle may store trace information which can be useful for debugging if
 * the associated preprocessor value is truthy.
 * 
 * @tparam ManagedT 
 */
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

	/**
	 * @brief Adds one to the pointers's reference count
	 */
	void IncrementRefCnt()
	{
		CGF_LOG_TRACE(Name, "Gained ref");

		RefCount++;
	}

	/**
	 * @brief Subtracts one from the pointers's reference count
	 */
	void DecrementRefCnt()
	{
		CGF_LOG_TRACE(Name, "Lost ref");

		RefCount--;

		if(RefCount <= 0)
		{
			delete this;
		}
	}

	int RefCount = 0;
	Notifier OnDestruction;
	ManagedT* Object;

#if ENABLE_SMART_PTR_TRACING
	std::string Name = "?";
#endif
};


template <typename T, typename = void> 
struct overloads_arrow_operator 
{
    enum { value = 0 };
};


template <typename T> 
struct overloads_arrow_operator<T, std::void_t<decltype(std::declval<T>().operator->())>> 
{
    enum { value = 1 };
};


/**
 * @brief
 * 
 * @tparam ManagedT 
 */
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

	SharedPtr(nullptr_t ptr)
	{
		
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
		static_assert(std::is_base_of_v<TargetManagedT, ManagedT> || std::is_base_of_v<ManagedT, TargetManagedT>, 
			"Cannot convert to or from an underived type");

		return SharedPtr<TargetManagedT>( (SharedObjectHandle<TargetManagedT>*) m_Ref );
	}

	/**
	 * @brief see Valid()
	 */
	operator bool() const
	{
		return Valid();
	}
	
	/**
	 * @brief Indicates the validity of the raw pointer being managed 
	 * and whether one exists.
	 */
	FORCEINLINE bool Valid() const
	{
		return m_Ref;
	}

	/**
	 *	
	 */
	FORCEINLINE ManagedT* GetRaw()
	{
		return m_Ref->Object;
	}

	/**
	 * Get a reference to the event invoked immediately prior to the
	 * destruction of the pointer being shared.
	 */
 	Notifier* GetDestructionEvent()
	{
		return &m_Ref->OnDestruction;
	}

	/**
	 * @brief Constructs a shared pointer, passing the provided arguments
	 * to the constructor of the value type.
	 */
	template<typename... ConstructorArgT>
	static SharedPtr<ManagedT> Create(ConstructorArgT&&... args)
	{
		ManagedT* ref = new ManagedT(args...);
		return SharedPtr<ManagedT>(ref);
	}

#if ENABLE_SMART_PTR_TRACING
	/**
	 *	
	 */
	template<typename... ConstructorArgT>
	static SharedPtr<ManagedT> CreateTraced(std::string&& name, ConstructorArgT&&... args)
	{
		ManagedT* ref = new ManagedT(args...);
		SharedPtr<ManagedT> managedRef = SharedPtr<ManagedT>(ref, name);

		return managedRef;
	}
#else
	/**
	 *	
	 */
	template <typename... ConstructorArgT>
	static SharedPtr<ManagedT> CreateTraced(std::string &&name, ConstructorArgT &&...args)
	{
		return Create(args...);
	}
#endif

	/**
	 * <!--
	 * Although ugly, SFINAE is applied here to allow arrow operator chaining when ManagedT
	 * implements the arrow operator, which makes cases where ManagedT is another nonprimitive 
	 * reference type a lot less ugly. For example, members of Pool<T> objects would otherwise
	 * have to be referenced like 'PooledPtr<T>()->Get().SomeMember'.
	 * -->
	 */
	template<typename _ManagedT = ManagedT>
	typename std::enable_if_t<!overloads_arrow_operator<_ManagedT>::value, _ManagedT*> operator->() const
	{
		CGF_ASSERT(Valid(), "Cannot dereference nullptr");
		
		return m_Ref->Object;
	}

	template<typename _ManagedT = ManagedT>
	typename std::enable_if_t<overloads_arrow_operator<_ManagedT>::value, _ManagedT&> operator->() const
	{
		CGF_ASSERT(Valid(), "Cannot dereference nullptr");
		
		return *(m_Ref->Object);
	}

protected:
	/**
	 * Detach from the current shared object and decrease the object's
	 * reference count.
	 */
	void Detach()
	{
		if(m_Ref)
		{
			m_Ref->DecrementRefCnt();
			m_Ref = nullptr;
		}
	}

	/**
	 * Attach to a new shared object, detaching from the current one
	 * if it exists, and increasing its reference count.
	 */
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

/**
 * @brief Constructs a dynamically sized, contiguous array of T. Objects within the array are
 * accessed through a PooledPtr<T>, and their existence with the array is tied to the lifespan
 * of their corresponding PooledPtr.
 * 
 * The PooledPtr type is a shorthand alias for the type SharedPtr<typename Pool<T>::PoolObject>.
 * The PoolObject type references its object in its pool array by index instead of pointer, as pointers to
 * pool array members would be invalidated upon resizing / reallocation. As expected, it also removes its
 * referenced object upon destruction.
 */
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

		T* operator->() const
		{
			return &(Owner->At(Index));
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

	T& operator[](int index)
	{
		return At(index);
	}

	T& At(int index)
	{
		return m_Data[index];
	}

	FORCEINLINE int GetCount() const
	{
		return m_Count;
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


template<typename T>
using PooledPtr = SharedPtr<typename Pool<T>::PoolObject>;