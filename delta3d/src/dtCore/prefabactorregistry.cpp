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

#include <dtCore/prefabactorregistry.h>
#include <dtCore/project.h>
#include <dtCore/datatype.h>
#include <dtCore/prefabactortype.h>
#include <dtCore/exceptionenum.h>

namespace dtCore
{

   ///////////////////////////////////////////////////////////////////////////////
   PrefabActorRegistry::PrefabActorRegistry() : ActorPluginRegistry("Dynamic actor types of prefabs.")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   PrefabActorRegistry::~PrefabActorRegistry()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabActorRegistry::RegisterActorTypes()
   {
      dtCore::Project::ResourceTree prefabs;
      Project::GetInstance().GetResourcesOfType(DataType::PREFAB, prefabs);
      RecursiveAdd(prefabs.begin());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabActorRegistry::RecursiveAdd(const Project::ResourceTree::const_iterator& iter)
   {
      for (Project::ResourceTree::const_iterator i = iter.tree_ref().in();
         i != iter.tree_ref().end();
         ++i)
      {
         if (i->isCategory())
         {
            RecursiveAdd(i);
         }
         else
         {
            // pass in the resource
            ActorTypePtr atp = new PrefabActorType(i->getResource(), std::string(), NULL);
            mCurTypeList.insert(std::make_pair(atp->GetFullName(), atp));
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void PrefabActorRegistry::GetSupportedActorTypes(std::vector<dtCore::RefPtr<const ActorType> >& actorTypes)
   {
      RegisterActorTypes(); // this will update the list for the prefabs, but it's not a very efficient way to do it.
      actorTypes.reserve(mCurTypeList.size());
      TypeListMap::const_iterator i, iend;
      i = mCurTypeList.begin();
      iend = mCurTypeList.end();
      for (; i != iend; ++i)
      {
         actorTypes.push_back(i->second.get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool PrefabActorRegistry::IsActorTypeSupported(const ActorType& type) const
   {
      return mCurTypeList.find(type.GetFullName()) != mCurTypeList.end();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const ActorType* PrefabActorRegistry::GetActorType(const std::string& category, const std::string& name) const
   {
      TypeListMap::const_iterator itor = mCurTypeList.find(category + "." + name);
      if (itor != mCurTypeList.end())
      {
         return itor->second.get();
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<BaseActorObject> PrefabActorRegistry::CreateActor(const ActorType& type)
   {
      dtCore::RefPtr<BaseActorObject> result;
      if (IsActorTypeSupported(type))
      {
         try
         {
            const PrefabActorType& prefType = dynamic_cast<const PrefabActorType&>(type);
            result = dtCore::Project::GetInstance().LoadPrefab(prefType);
         }
         catch(const InvalidContextException&)
         {
            LOG_ERROR("The Prefab actor registry attempted to create a registered ActorType, but no project contexts are registered: " + type.GetFullName());
         }
         catch(const dtUtil::FileNotFoundException&)
         {
            LOG_ERROR("The Prefab actor registry attempted to create a registered ActorType, but the resource doesn't exist: " + type.GetFullName());
         }
         catch(const MapParsingException&)
         {
            LOG_ERROR("The Prefab actor registry attempted to create a registered ActorType, but the prefab resource failed to load: " + type.GetFullName());
         }
         catch(std::bad_cast&)
         {
            LOG_ERROR("The Prefab actor registry attempted to create a registered ActorType, but the type class appears to have been created incorrectly: " + type.GetFullName());
         }
      }
      return result;
   }


} /* namespace dtCore */
