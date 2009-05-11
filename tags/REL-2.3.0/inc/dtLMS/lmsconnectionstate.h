/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Christopher DuBuc
 */

#ifndef DELTA_LMS_CONNECTION_STATE
#define DELTA_LMS_CONNECTION_STATE

#include <dtUtil/enumeration.h>
#include <dtLMS/export.h>

namespace dtLMS
{
   /**
    * This class is an enumeration which stores the connection state of
    * the LmsClientSocket.
    */
   class DT_LMS_EXPORT LmsConnectionState : public dtUtil::Enumeration
   {
      DECLARE_ENUM(LmsConnectionState);
      
   public:
      static const LmsConnectionState INITIALIZING;
      static const LmsConnectionState INITIALIZED;
      static const LmsConnectionState HANDSHAKING;
      static const LmsConnectionState CONNECTED;
      static const LmsConnectionState FINISHED;
      static const LmsConnectionState ERRORSTATE;

      protected:

      /// Constructor
      LmsConnectionState(const std::string& name)
         : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }

      /// Destructor
      virtual ~LmsConnectionState() { }
   };
} // namespace dtLMS

#endif // DELTA_LMS_CONNECTION_STATE
