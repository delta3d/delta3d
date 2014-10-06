/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2009 MOVES Institute 
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
* MG
*/

#include <dtCore/plugin_export.h>
#include <dtGame/gameactorproxy.h>

namespace dtActors 
{
   class DT_PLUGIN_EXPORT TriggerVolumeActorProxy : public dtGame::GameActorProxy
   {
   public:
      static const dtUtil::RefString CLASS_NAME;
      static const dtUtil::RefString PROPERTY_MAX_TRIGGER_COUNT;

      TriggerVolumeActorProxy();

      virtual void CreateDrawable();

      virtual void BuildPropertyMap();

      virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode();

      virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

   protected:

      virtual ~TriggerVolumeActorProxy();
   };
}
