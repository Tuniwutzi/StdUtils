#ifndef STDUTILS_THREADING_DEFINITIONS_H
#define STDUTILS_THREADING_DEFINITIONS_H


#include <StdUtils/Base.h>


#ifdef _WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#else

#include <pthread.h>
#include <semaphore.h>

#define DWORD uint32_t
#define INFINITE 0xFFFFFFFF

#endif


namespace StdUtils
{
	namespace Threading
	{
	}
}

#endif