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

#ifndef DELTA_LMS_MESSAGE_TYPE
#define DELTA_LMS_MESSAGE_TYPE

#include <dtUtil/enumeration.h>
#include <dtLMS/export.h>

namespace dtLMS
{
   /**
    * This class is an enumeration which defines the types of lms messages
    * that can be sent to the lms messaging applet.
    */
   class DT_LMS_EXPORT LmsMessageType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(LmsMessageType);
      
   public:
      static const LmsMessageType UNKNOWN;
      static const LmsMessageType SIMULATION;
      static const LmsMessageType LAUNCH_PAGE;
      static const LmsMessageType OBJECTIVE_COMPLETION;
      static const LmsMessageType OBJECTIVE_SCORE;

      protected:

      /// Constructor
      LmsMessageType(const std::string& name)
         : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }

      /// Destructor
      virtual ~LmsMessageType() { }
   };
} // namespace dtLMS

#endif // DELTA_LMS_MESSAGE_TYPE
