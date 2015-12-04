/*
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
 * Matthew W. Campbell
 */
#include <prefix/dtcoreprefix.h>
#include <dtCore/actortype.h>

namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString& SharedClassInfo::GetClassName() const { return mClassName; }

   /////////////////////////////////////////////////////////////////////////////
   bool SharedClassInfo::IsInstanceOf(const dtUtil::RefString& name) const
   {
      return mClassHierarchy.find(name) != mClassHierarchy.end();
   }

   /////////////////////////////////////////////////////////////////////////////
   void SharedClassInfo::SetClassName(const dtUtil::RefString& name)
   {
      mClassName = name;
      mClassHierarchy.insert(mClassName);
   }

   //////////////////////////////////////////////////////////////////////////
   ActorType::ActorType(const std::string& name,
            const std::string& category,
            const std::string& desc,
            const ActorType* parentType)
   : ObjectType(name, category, desc, parentType)
   , mClassInfo(new SharedClassInfo)
   {}

   //////////////////////////////////////////////////////////////////////////
   const ActorType* ActorType::GetParentActorType() const
   {
      return dynamic_cast<const ActorType*>(GetParentType());
   }

   //////////////////////////////////////////////////////////////////////////
   SharedClassInfo& ActorType::GetSharedClassInfo() const
   {
      return *mClassInfo;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorType::MergeSharedClassInfo(SharedClassInfo& clsInfo) const
   {
      mClassInfo->mClassHierarchy.insert(clsInfo.mClassHierarchy.begin(), clsInfo.mClassHierarchy.end());
      if (mClassInfo->GetClassName()->empty())
      {
         mClassInfo->SetClassName(clsInfo.GetClassName());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   ActorType::~ActorType() { }
}
