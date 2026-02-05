#pragma once

#define FORCEINLINE __forceinline

#ifdef _DEBUG

#include <stdexcept>
#include <iostream>

#define CGF_ERROR(msg) std::cerr << (msg); throw new std::runtime_error(msg)
#define CGF_LOG(msg) std::cout << (msg) << std::endl;
#define CGF_INFO(msg) std::cout << "[" << strrchr(__FILE__, '\\') + 1 << ":" << __LINE__ << "] " << (msg) << std::endl;
#define CGF_ASSERT(exp, msg) if(!(exp)) { CGF_ERROR(msg); }
#define CGF_ENSURE_NOT_NULLPTR(exp) CGF_ASSERT(exp, #exp" is not a valid reference")

#endif