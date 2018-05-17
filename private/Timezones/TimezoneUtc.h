/*
 * TimezoneUtc.h
 *
 *  Created on: 12 Oct 2014
 *      Author: root
 */

#ifndef TIMEZONEUTC_H_
#define TIMEZONEUTC_H_

#include <StdUtils/DateTime.h>


namespace StdUtils
{
	class TimezoneUtc : public Timezone
	{
	public:
		virtual ~TimezoneUtc() {}
	public:
		DateTime now() const STDUTILS_OVERRIDE;
		String const& toString() const STDUTILS_OVERRIDE;
	};
}


#endif /* TIMEZONEUTC_H_ */
