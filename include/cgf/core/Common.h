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
#include <stdexcept>
#include <iostream>

#define CGF_ERROR(msg) std::cerr << (msg); throw new std::runtime_error(msg)
#define CGF_LOG(msg) std::cout << (msg) << std::endl;
#define CGF_ASSERT(exp, msg) if(!(exp)) { CGF_ERROR(msg); }
#define CGF_ENSURE_NOT_NULLPTR(exp) CGF_ASSERT(exp, #exp" is not a valid reference")

#endif