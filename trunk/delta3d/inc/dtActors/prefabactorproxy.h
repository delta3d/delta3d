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
#include <dtCore/transformableactorproxy.h>
#include <dtABC/export.h>
#include <dtCore/actorproperty.h>
#include <dtCore/map.h>
#include <osgText/Text>
#include <dtCore/transformable.h>

namespace dtActors
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT PrefabActor : public dtCore::Transformable
   {
   public:
      typedef dtCore::Transformable BaseClass;

      PrefabActor(const std::string& name = "PrefabActor");

   protected:
      virtual ~PrefabActor();

   private:
   };

   /////////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT PrefabActorProxy : public dtCore::TransformableActorProxy
   {
   public:
      typedef dtCore::TransformableActorProxy BaseClass;

      static const dtUtil::RefString CLASS_NAME;

      PrefabActorProxy();

      virtual void CreateDrawable();

      virtual void BuildPropertyMap();

      void SetMap(dtCore::Map* map);

      void SetPrefab(const std::string& fileName);

      std::vector<dtCore::RefPtr<dtCore::BaseActorObject> >& GetPrefabProxies();

   protected:
      virtual ~PrefabActorProxy();

   private:
      std::vector<dtCore::RefPtr<dtCore::BaseActorObject> > mActors;

      dtCore::RefPtr<dtCore::Map> mMap;
   };
}
#endif // prefabactorproxy_h__
