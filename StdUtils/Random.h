#ifndef STDUTILS_RANDOM_H
#define STDUTILS_RANDOM_H

#include <StdUtils/Base.h>

#ifdef _WINDOWS
#include <random>
#endif

#include <StdUtils/Base.h>


namespace StdUtils
{
#ifdef _WINDOWS
	class Random
	{
	private:
		std::default_random_engine gen;

	public:
		Random();

	public:
		int next();
		int next(int max);
	};
#else
	//TODO: Temporaer; Ab gcc 4.7
	class Random
	{
	public:
		Random();

	public:
		int next();
		int next(int max);
	};
#endif
}

#endif
