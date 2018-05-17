#ifndef STDUTILS_PRIVATE_WAITFORIO_H
#define STDUTILS_PRIVATE_WAITFORIO_H

#define NOMINMAX

#include <StdUtils/Exceptions.h>
#include <StdUtils/Networking/Socket.h>

#include <private/Networking/Sockets.h>

#include <algorithm>

namespace StdUtils
{
	namespace Private
	{
		namespace Networking
		{
			const int SocketStatusNone(0);
			const int SocketStatusMonitored(1);
			const int SocketStatusActivelyMonitored(2);

			struct SocketWaitData
			{
				StdUtils::Networking::SocketHandle H;

				bool Input;
				bool Output;

				void* CustomData;

				bool InputUpdated;
				bool OutputUpdated;
			};
			inline uint32_t WaitForSockets(SocketWaitData* sockets, uint32_t socketCount, uint32_t timeoutMS)
			{
				fd_set readset;
				fd_set writeset;

				FD_ZERO(&readset);
				FD_ZERO(&writeset);

				StdUtils::Networking::SocketHandle highest = 0;
				for (uint32_t i = 0; i < socketCount; i++)
				{
					if (sockets[i].H == NULL)
					{
						sockets[i].InputUpdated = sockets[i].Input;
						sockets[i].OutputUpdated = sockets[i].Output;
						return 1;
					}

					sockets[i].InputUpdated = false;
					sockets[i].OutputUpdated = false;

					StdUtils::Networking::SocketHandle h = sockets[i].H;

					if (sockets[i].Input)
						FD_SET(h, &readset);
					if (sockets[i].Output)
						FD_SET(h, &writeset);

					if (sockets[i].Input || sockets[i].Output)
						highest = std::max(highest, h);
				}

				timeval timeout;
				timeout.tv_sec = timeoutMS / 1000;
				timeout.tv_usec = (timeoutMS % 1000) * 1000;

				int eventCount = select(((int)highest) + 1, &readset, &writeset, NULL, (timeoutMS == 0 ? NULL : &timeout));
				if (eventCount < 0)
					throw OSApiException("Error calling select", LASTSOCKETERROR);

				int eventsFound = 0;
				for (uint32_t i = 0; i < socketCount && eventsFound < eventCount; i++)
				{
					StdUtils::Networking::SocketHandle h = sockets[i].H;

					bool is = (sockets[i].Input ? FD_ISSET(h, &readset) != 0 : false);
					bool os = (sockets[i].Output ? FD_ISSET(h, &writeset) != 0 : false);

					if (is || os)
					{
						eventsFound++;
						if (is)
							sockets[i].InputUpdated = true;
						if (os)
							sockets[i].OutputUpdated = true;
					}
				}

				if (eventCount != eventsFound)
					throw Exception("Kernel reported more io handle updates than could be verified"); //TODO?: ... bessere Fehlermeldung? Problem ist, dass WaitForIO intern ist, daher kann ich dem Nutzer der Lib keine wirklich aussagekraeftige Meldung geben

				return eventCount;
			}
		}
	}
}

#endif
