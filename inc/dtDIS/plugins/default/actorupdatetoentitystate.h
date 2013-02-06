/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_PLUGIN_ACTOR_UPDATE_TO_ENTITY_STATE_H__
#define __DELTA_PLUGIN_ACTOR_UPDATE_TO_ENTITY_STATE_H__

#include <dtDIS/imessagetopacketadapter.h> // for base class
#include <dtCore/refptr.h>                 // for member
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>             // for library export symbols

namespace dtGame
{
   class GameManager;
}

namespace dtCore
{
   class UniqueId;
}

namespace DIS
{
   class EntityStatePdu;
}

namespace dtDIS
{
   class SharedState;

   /// responsible for translating ActorUpdateMessage instances to EntityStatePdu instances.
   class DT_DIS_DEFAULT_EXPORT ActorUpdateToEntityState : public dtDIS::IMessageToPacketAdapter
   {
   public:
      ActorUpdateToEntityState(dtDIS::SharedState* config, dtGame::GameManager* gm);
      ~ActorUpdateToEntityState();

      DIS::Pdu* Convert(const dtGame::Message& source);

   private:
      ActorUpdateToEntityState();  ///< not implemented by design
      void SetEntityType(const dtCore::UniqueId& uniqueID, DIS::EntityStatePdu* downcastPdu); 

      dtDIS::SharedState* mConfig;
      dtGame::GameManager* mGM;
      DIS::Pdu* mPdu;
   };
}

#endif  // __DELTA_PLUGIN_ACTOR_UPDATE_TO_ENTITY_STATE_H__

