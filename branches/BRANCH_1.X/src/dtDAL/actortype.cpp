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
#include <prefix/dtdalprefix-src.h>
#include <dtDAL/actortype.h>

namespace dtDAL 
{

   ///////////////////////////////////////////////////////////////////////////////
   void ActorType::GenerateUniqueId()
   {
      mUniqueId = mName + mCategory;
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   const std::string ActorType::ToString() const
   {
      return GetCategory() + "." + GetName();
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ActorType::InstanceOf(const ActorType &rhs) const
   {
      if (rhs == *this)
         return true;
         
      const ActorType *parent = GetParentActorType();
      while (parent != NULL)
      {
         if (*parent == rhs)
            return true;
         parent = parent->GetParentActorType();         
      }
      
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ActorType::InstanceOf(const std::string &category, const std::string &name) const
   {
      dtCore::RefPtr<ActorType> typeToCheck = new ActorType(name,category);
      return InstanceOf(*typeToCheck);
   }
   
}
