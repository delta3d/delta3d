/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Chris DuBuc
 */

#ifndef DELTA_LMS_MESSAGE_VALUE
#define DELTA_LMS_MESSAGE_VALUE

#include <dtUtil/enumeration.h>
#include <dtLMS/export.h>

namespace dtLMS
{
   /**
    * This class is an enumeration which defines the values of lms messages
    * that can be sent to or received from the lms messaging applet. It contains
    * inner classes for Simulation, Launch Page, and Objective Completion messages.
    */
   class LmsMessageValue
   {
   public:

      /**
       * This class is an enumeration which defines the valid simulation status
       * values of lms messages that can be sent to the lms messaging applet.
       */
      class DT_LMS_EXPORT SimulationValue : public dtUtil::Enumeration
      {
         DECLARE_ENUM(SimulationValue);
         
      public:
         static const SimulationValue RUNNING;
         static const SimulationValue STOPPED_NORMAL;
         static const SimulationValue STOPPED_ABNORMAL;

         protected:

         /// Constructor
         SimulationValue(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }

         /// Destructor
         virtual ~SimulationValue() { }

      };

      /**
       * This class is an enumeration which defines the valid launch page values of
       * lms messages that can be received from the lms messaging applet.
       */
      class DT_LMS_EXPORT LaunchPageValue : public dtUtil::Enumeration
      {
         DECLARE_ENUM(LaunchPageValue);
         
      public:
         static const LaunchPageValue LISTENING;
         static const LaunchPageValue STOPPED;

         protected:

         /// Constructor
         LaunchPageValue(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }

         /// Destructor
         virtual ~LaunchPageValue() { }
      };

      /**
       * This class is an enumeration which defines the valid objective completion values 
       * of lms messages that can be sent to the lms messaging applet.
       */
      class DT_LMS_EXPORT ObjectiveCompletionValue : public dtUtil::Enumeration
      {
         DECLARE_ENUM(ObjectiveCompletionValue);
         
      public:
         static const ObjectiveCompletionValue COMPLETE;
         static const ObjectiveCompletionValue INCOMPLETE;

         protected:

         /// Constructor
         ObjectiveCompletionValue(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }

         /// Destructor
         virtual ~ObjectiveCompletionValue() { }
      };
   };

} // namespace dtLMS

#endif // DELTA_LMS_MESSAGE_VALUE
