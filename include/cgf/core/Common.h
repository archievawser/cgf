#pragma once

#define FORCEINLINE __forceinline


template<typename T>
struct Singleton
{
	Singleton() = delete;

	static FORCEINLINE T* Get()
	{
		return Instance;
	}

	template<typename... ArgType>
	static void Initialize(ArgType... args)
	{
		Instance = new T(args...);
	}

	static T *Instance;
};


template <typename T>
T *Singleton<T>::Instance;


#ifdef _DEBUG
#include <cassert>

#define GE_ASSERT(exp) assert(exp)
#define GE_ERROR_IF_NULLPTR(exp) GE_ASSERT(exp)
#endif