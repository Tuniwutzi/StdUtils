#ifndef STDUTILS_MONITORING_MEMORY_H
#define STDUTILS_MONITORING_MEMORY_H


#include <iostream>

#include <StdUtils/Base.h>


namespace StdUtils
{
	namespace Monitoring
	{
		typedef unsigned long long UsedMemory;
		struct HeapStatus
		{
			UsedMemory AllocatedBytes;
		};

		class Memory
		{
		private:
			Memory();

		public:
			static HeapStatus Heap();
			static void PrintHeap(std::ostream& = std::cout);
		};
	}
}

#endif