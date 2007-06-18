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

#ifndef __DELTA_DTDIS_PLUGIN_DEFAULT_H__
#define __DELTA_DTDIS_PLUGIN_DEFAULT_H__

#include <dtDIS/idisplugin.h>                         // for base class
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>                        // for export symbols

namespace dtDIS
{
   class ESPduProcessor;
   class CreateEntityProcessor;
   class RemoveEntityProcessor;
   class ActorUpdateToEntityState;

   /// the plugin to support various DIS::Pdu types.
   class DT_DIS_DEFAULT_EXPORT DefaultPlugin : public dtDIS::IDISPlugin
   {
   public:
      DefaultPlugin();

      /// attaches the IPacketProcessor and IMessageToPacketAdapter
      void Start(DIS::IncomingMessage& imsg,
                 dtDIS::OutgoingMessage& omsg,
                 dtGame::GameManager* gm,
                 dtDIS::SharedState* config);

      void Finish(DIS::IncomingMessage& imsg, dtDIS::OutgoingMessage& omsg);

      ~DefaultPlugin();

   private:
      ///\todo inform the sending adapter of the new actors or modify the ActiveEntityControl.
      void OnActorAdded(dtGame::GameActorProxy* actor);

      ESPduProcessor* mESProcessor;
      CreateEntityProcessor* mCreateProcessor;
      RemoveEntityProcessor* mRemoveProcessor;
      ActorUpdateToEntityState* mSendingAdapter;
      const dtDIS::SharedState* mConfig;
   };
}

#endif  // __DELTA_DTDIS_PLUGIN_DEFAULT_H__

