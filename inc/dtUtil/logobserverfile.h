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
#ifndef LOGOBSERVERFILE_h__
#define LOGOBSERVERFILE_h__

#include <dtUtil/export.h>
#include <dtUtil/logobserver.h>
#include <fstream>

namespace dtUtil
{

   /** 
     * A specialized LogObserver class that outputs Log messages to a file
     */
   class DT_UTIL_EXPORT LogObserverFile : public LogObserver
   {
   public:
      LogObserverFile();

      void OpenFile();
      
      virtual void LogMessage(const LogData& logData);

      void LogHorizRule();

      bool mOpenFailed;

   protected:
      virtual ~LogObserverFile();

   private:
      std::ofstream logFile;

      void TimeTag(std::string prefix);
      void EndFile();

   };

}
#endif // LOGOBSERVERFILE_h__
