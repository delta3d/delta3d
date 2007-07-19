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

#ifndef __DELTA_DTDIS_IDISPLUGIN_H__
#define __DELTA_DTDIS_IDISPLUGIN_H__

#include <vector>                       // for typedef, param
#include <dtCore/refptr.h>              // for typedef, param
#include <dtGame/gameactor.h>           // for typedef, param

#include <dtDIS/dtdisexport.h>          // for library export symbols

///@cond DOXYGEN_SHOULD_SKIP_THIS
namespace DIS
{
   class IncomingMessage;
}
///@endcond

namespace dtGame
{
   class GameManager;
   class GameActorProxy;
}

namespace dtDIS
{
   class OutgoingMessage;
   class SharedState;

   /// the interface for all DIS plugins
   class DT_DIS_EXPORT IDISPlugin
   {
   public:
      virtual ~IDISPlugin();

      /// plugin is informed of the state of the game.
      /// called after being successfully loaded.
      /// @param incoming used to attach packet handlers.
      /// @param outgoing used to attach message handlers.
      /// @param gm used to deliver the state of the game.
      /// @param config used to share connection and setup data.
      virtual void Start(DIS::IncomingMessage& incoming,
                         dtDIS::OutgoingMessage& outgoing,
                         dtGame::GameManager* gm,
                         dtDIS::SharedState* config)=0;

      /// plugin should forget the state of the game.
      /// called before unloading.
      virtual void Finish(DIS::IncomingMessage& incoming,
                          dtDIS::OutgoingMessage& outgoing)=0;
   };
}

#endif  //  __DELTA_DTDIS_IDISPLUGIN_H__
