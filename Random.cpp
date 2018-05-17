#include <StdUtils/Random.h>


#include <time.h>
#include <stdlib.h>


using namespace std;


namespace StdUtils
{
#ifdef _WINDOWS
	Random::Random()
		:gen((unsigned int)time(NULL))
	{
		//std::random_device rd;
		//gen.seed(rd());
	}

	int Random::next()
	{
		uniform_int_distribution<int> dist;
		return dist(this->gen);
	}
	int Random::next(int max)
	{
		uniform_int_distribution<int> distribution(0, max);
		return distribution(this->gen);
	}
#else
	//TODO!: Threadsafety!
	Random::Random()
	{
		static bool seeded = false;
		if (!seeded)
		{
			srand(time(NULL));
			seeded = true;
		}
	}

	int Random::next()
	{
		return rand();
	}
	int Random::next(int max)
	{
		return rand() % (max+1);
	}
#endif
}
