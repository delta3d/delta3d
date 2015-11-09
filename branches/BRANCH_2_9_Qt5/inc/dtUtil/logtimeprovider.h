/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2013 David Guthrie
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * David Guthrie
 */

#ifndef LOGTIME_H_
#define LOGTIME_H_

#include <dtUtil/export.h>
#include <dtUtil/datetime.h>

namespace dtUtil
{
   /** Interface class get the time for the logger.
    *  There is probably no need to override this yourself.
    *  It is provided as a means for other parts of the system to provide the time.
    *
    *  @note  The Log time provied does not extend osg::Referenced so that it can be used
    *         easily as an interface on other referenced classes, but the log code assumes that a
    *         dynamic_cast to reference will succeed.
    *
    *  @see  AsReferenced
    */
   class DT_UTIL_EXPORT LogTimeProvider
   {
   public:
	   virtual ~LogTimeProvider() {}

	   virtual const dtUtil::DateTime& GetDateTime() = 0;
	   virtual unsigned GetFrameNumber() = 0;
	   virtual osg::Referenced* AsReferenced() = 0;
   };

}


#endif /* LOGTIME_H_ */
