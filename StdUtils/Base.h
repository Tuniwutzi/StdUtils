#ifndef STDUTILS_BASE
#define STDUTILS_BASE

#include <stdint.h>
#include <string>
//#include <new>

#ifdef _WIN32
#ifndef _WINDOWS
#define _WINDOWS
#endif
#endif

#define STDUTILS_OVERRIDE

#define STDUTILS_NOOP ((void)0)

#define STDUTILS_PREVENT_COPY(classname) private: classname(classname const&)
#define STDUTILS_PREVENT_ASSIGNMENT(classname) private: classname& operator=(classname const&)
#define STDUTILS_NOCOPY(classname)	STDUTILS_PREVENT_COPY(classname);\
									STDUTILS_PREVENT_ASSIGNMENT(classname)

#define STDUTILS_ARRAYCOUNT(arr) (sizeof(arr) / sizeof(*arr))

#define STDUTILS_LOCK(mutex) mutex.lock();\
						try
#define STDUTILS_RELEASE(mutex)	catch (...)\
						{\
						mutex.release();\
						throw;\
						}\
						mutex.release()
#define STDUTILS_RELEASE_AND_RETURN(mutex, rv) if (true) { mutex.release(); return rv; } STDUTILS_NOOP

#define STDUTILS_USING(creation) creation; try
#define STDUTILS_USED(disposal) catch(...) { disposal; throw; } disposal

#define STDUTILS_enum_methods(name) private: \
										name::Value value; \
									public: \
										name() \
											:value() \
										{ \
										} \
										name(Value v) \
											:value(v) \
										{ \
										} \
									public:\
										name::Value operator*() const\
										{\
											return this->value;\
										} \
										operator name::Value() const \
										{ \
											return this->value; \
										}// \
										//bool operator==(name const& other) const \
										//{ \
										//	return this->value == other.value; \
										//} \
										//bool operator!=(name const& other) const \
										//{ \
										//	return this->value != other.value; \
										//}
#define STDUTILS_START_ENUM(name)	struct name \
									{ \
									public: \
										enum Value
#define STDUTILS_END_ENUM(name)			; \
									STDUTILS_enum_methods(name) \
									public:\
										bool checkFlag(name::Value flag) const\
										{\
											return (this->value & flag) == flag; \
										}\
										name& setFlag(name::Value flag) \
										{ \
											this->value = (name::Value)(this->value | flag); \
											return *this;\
										} \
										name& resetFlag(name::Value flags) \
										{ \
											this->value = (name::Value)(this->value & (~flags)); \
											return *this;\
										} \
										name& invertFlag(name::Value flags) \
										{ \
											this->value = (name::Value)(this->value ^ flags); \
											return *this;\
										} \
									}
#define STDUTILS_START_FLAGS(name)	STDUTILS_START_ENUM(name)
#define STDUTILS_END_FLAGS(name)		; \
									STDUTILS_enum_methods(name) \
									public: \
										bool allSet(name::Value flags) const \
										{ \
											return (this->value & flags) == flags; \
										} \
										bool anySet(name::Value flags) const \
										{ \
											return (this->value & flags) != 0; \
										} \
										void set(name::Value flags) \
										{ \
											this->value = (name::Value)(this->value | flags); \
										} \
										void reset(name::Value flags) \
										{ \
											this->value = (name::Value)(this->value & (~flags)); \
										} \
										void reset() \
										{ \
											this->value = (name::Value)0; \
										} \
										void invert(name::Value flags) \
										{ \
											this->value = (name::Value)(this->value ^ flags); \
										} \
									}

#ifdef _WINDOWS /////////////////////// WINDOWS

#define STDUTILS_DLLEXPORT _declspec(dllexport)
#define STDUTILS_DLLIMPORT _declspec(dllimport)

#else ///////////////////////////////// LINUX
						
#define STDUTILS_DLLEXPORT
#define STDUTILS_DLLIMPORT

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif

#ifdef STDUTILS_DYNAMICLIBRARY
#ifdef STDUTILS_LIBRARYBUILD
#define STDUTILS_DLLCONTENT STDUTILS_DLLEXPORT
#else
#define STDUTILS_DLLCONTENT STDUTILS_DLLIMPORT
#endif
#else
#define STDUTILS_DLLCONTENT
#endif

namespace StdUtils
{
    typedef std::string String;
	typedef long double fpmax;
}
#endif
