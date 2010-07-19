/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2010, Alion Science and Technology.
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
 * Curtiss Murphy
 */

#include <dtNetGM/serverframesyncmessage.h>
#include <dtGame/message.h>
#include <dtGame/messageparameter.h>

namespace dtNetGM
{
   ////////////////////////////////////////////////////////////////////
   ServerFrameSyncMessage::ServerFrameSyncMessage() : dtGame::Message()
   {
      AddParameter(new dtGame::DoubleMessageParameter("ServerSimTimeSinceStartup"));
      AddParameter(new dtGame::FloatMessageParameter("ServerTimeScale"));
   }

   ////////////////////////////////////////////////////////////////////
   double ServerFrameSyncMessage::GetServerSimTimeSinceStartup() const
   {
      const dtGame::DoubleMessageParameter* mp = static_cast<const dtGame::DoubleMessageParameter*>(GetParameter("ServerSimTimeSinceStartup"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerFrameSyncMessage::SetServerSimTimeSinceStartup(double newValue)
   {
      dtGame::DoubleMessageParameter* mp = static_cast<dtGame::DoubleMessageParameter*>(GetParameter("ServerSimTimeSinceStartup"));
      mp->SetValue(newValue);
   }

   ////////////////////////////////////////////////////////////////////
   float ServerFrameSyncMessage::GetServerTimeScale() const
   {
      const dtGame::FloatMessageParameter* mp = static_cast<const dtGame::FloatMessageParameter*>(GetParameter("ServerTimeScale"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerFrameSyncMessage::SetServerTimeScale(float newValue)
   {
      dtGame::FloatMessageParameter* mp = static_cast<dtGame::FloatMessageParameter*>(GetParameter("ServerTimeScale"));
      mp->SetValue(newValue);
   }
}

//////////////////////////////////////////////////////////////////////////////
