/* -*-c++-*-
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
 * Jeff P. Houde
 */

#ifndef prefabactorproxy_h__
#define prefabactorproxy_h__

#include <dtCore/plugin_export.h>
#include <dtGame/gameactorproxy.h>
#include <dtABC/export.h>
#include <dtCore/actorproperty.h>
#include <dtCore/map.h>
#include <osgText/Text>
#include <dtCore/transformable.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT PrefabActor : public dtGame::GameActorProxy
   {
   public:
      typedef dtGame::GameActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      PrefabActor();

      void CreateDrawable() override;

      void BuildPropertyMap() override;

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, PrefabResource);

   protected:
      void LoadPrefab(const dtCore::ResourceDescriptor& value);
      virtual ~PrefabActor();
   };
}
#endif // prefabactorproxy_h__
