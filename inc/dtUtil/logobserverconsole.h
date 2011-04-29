/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, MOVES Institute
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
 * Erik Johnson
 */
#ifndef LOGOBSERVERCONSOLE_h__
#define LOGOBSERVERCONSOLE_h__

#include <dtUtil/logobserver.h>
#include <dtUtil/export.h>

namespace dtUtil
{
   /** Specialized LogObserver used to display Log messages in the console 
     * window.
     * @see Log::AddObserver()
     */
   class DT_UTIL_EXPORT LogObserverConsole : public LogObserver
   {
   public:
      LogObserverConsole();

      virtual void LogMessage(const LogData& logData);

   protected:
      virtual ~LogObserverConsole();
   };
}

#endif // LOGOBSERVERCONSOLE_h__
