#include <StdUtils/Monitoring/Memory.h>


#ifdef _WINDOWS
#include <crtdbg.h>
#endif


using namespace std;


namespace StdUtils
{
	namespace Monitoring
	{
#ifdef _WINDOWS
		HeapStatus Memory::Heap()
		{
			HeapStatus rv;
			rv.AllocatedBytes = 0;

			_CrtMemState st;
			_CrtMemCheckpoint(&st);

			for (int i = 0; i < _MAX_BLOCKS; i++)
				rv.AllocatedBytes += st.lSizes[i];

			return rv;
		}
#endif

		void Memory::PrintHeap(ostream& to)
		{
			HeapStatus hs(Heap());

			to << "Current: " << hs.AllocatedBytes << endl;
		}
	}
}