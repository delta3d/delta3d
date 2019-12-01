/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2015, Caper Holdings, Inc
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
 * David Guthrie
 */
#ifndef PREFABACTORREGISTRY_H_
#define PREFABACTORREGISTRY_H_

#include <dtCore/actorpluginregistry.h>
#include <dtCore/actortype.h>
#include <dtCore/project.h>
#include <dtUtil/hashmap.h>

namespace dtCore
{

   class PrefabActorRegistry: public ActorPluginRegistry
   {
   public:
      PrefabActorRegistry();
      virtual ~PrefabActorRegistry();

      virtual void RegisterActorTypes();

      virtual void GetSupportedActorTypes(std::vector<dtCore::RefPtr<const ActorType> >& actorTypes);

      virtual bool IsActorTypeSupported(const ActorType& type) const;

      virtual const ActorType* GetActorType(const std::string& category, const std::string& name) const;

      virtual dtCore::RefPtr<BaseActorObject> CreateActor(const ActorType& type);

   private:
      void RecursiveAdd(const Project::ResourceTree::const_iterator& iter);
      typedef dtUtil::HashMap<std::string, ActorTypePtr> TypeListMap;
      TypeListMap mCurTypeList;
   };

} /* namespace dtCore */

#endif /* PREFABACTORREGISTRY_H_ */
