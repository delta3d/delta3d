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

#include <dtNetGM/serversynccontrolmessage.h>
#include <dtGame/message.h>
#include <dtGame/messageparameter.h>

namespace dtNetGM
{
   ////////////////////////////////////////////////////////////////////
   ServerSyncControlMessage::ServerSyncControlMessage() : dtGame::Message()
   {
      AddParameter(new dtGame::BooleanMessageParameter("SyncEnabled", false));
      AddParameter(new dtGame::UnsignedIntMessageParameter("NumSyncsPerSecond", 60));
      AddParameter(new dtGame::FloatMessageParameter("MaxWaitTime", 4.0f));
   }


   ////////////////////////////////////////////////////////////////////
   bool ServerSyncControlMessage::GetSyncEnabled() const
   {
      const dtGame::BooleanMessageParameter* mp = static_cast<const dtGame::BooleanMessageParameter*>(GetParameter("SyncEnabled"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerSyncControlMessage::SetSyncEnabled(bool newValue)
   {
      dtGame::BooleanMessageParameter* mp = static_cast<dtGame::BooleanMessageParameter*>(GetParameter("SyncEnabled"));
      mp->SetValue(newValue);
   }

   ////////////////////////////////////////////////////////////////////
   unsigned int ServerSyncControlMessage::GetNumSyncsPerSecond() const
   {
      const dtGame::UnsignedIntMessageParameter* mp = static_cast<const dtGame::UnsignedIntMessageParameter*>(GetParameter("NumSyncsPerSecond"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerSyncControlMessage::SetNumSyncsPerSecond(unsigned int newValue)
   {
      dtGame::UnsignedIntMessageParameter* mp = static_cast<dtGame::UnsignedIntMessageParameter*>(GetParameter("NumSyncsPerSecond"));
      mp->SetValue(newValue);
   }

   ////////////////////////////////////////////////////////////////////
   float ServerSyncControlMessage::GetMaxWaitTime() const
   {
      const dtGame::FloatMessageParameter* mp = static_cast<const dtGame::FloatMessageParameter*>(GetParameter("MaxWaitTime"));
      return mp->GetValue();
   }

   ////////////////////////////////////////////////////////////////////
   void ServerSyncControlMessage::SetMaxWaitTime(float newValue)
   {
      dtGame::FloatMessageParameter* mp = static_cast<dtGame::FloatMessageParameter*>(GetParameter("MaxWaitTime"));
      mp->SetValue(newValue);
   }

}

//////////////////////////////////////////////////////////////////////////////
