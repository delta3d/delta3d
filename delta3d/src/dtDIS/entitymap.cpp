/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011 MOVES Institute
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
 * Erik Johnson
 */
#include <dtDIS/entitymap.h>

using namespace dtDIS;

//////////////////////////////////////////////////////////////////////////
EntityMap::EntityMap()
{

}

//////////////////////////////////////////////////////////////////////////
EntityMap::~EntityMap()
{

}

//////////////////////////////////////////////////////////////////////////
void EntityMap::SetEntityActorType(const DIS::EntityType& eid, const dtCore::ActorType* at)
{
   EntityMapping::iterator foundItr =  mEntityMap.find(eid);

   if (foundItr != mEntityMap.end())
   {
      //found it
      std::pair<RefActorType, dtCore::ResourceDescriptor> p = foundItr->second;
      p.first = at;
      foundItr->second = p;
   }
   else
   {
      //add it
      std::pair<RefActorType, dtCore::ResourceDescriptor> p(at,dtCore::ResourceDescriptor::NULL_RESOURCE);
      mEntityMap.insert(EntityMapping::value_type(eid, p));
   }
}

//////////////////////////////////////////////////////////////////////////
void EntityMap::SetEntityResource(const DIS::EntityType& eid,
                                 const dtCore::ResourceDescriptor& resource)
{
   EntityMapping::iterator foundItr =  mEntityMap.find(eid);

   if (foundItr != mEntityMap.end())
   {
      //found it
      std::pair<RefActorType, dtCore::ResourceDescriptor> p = foundItr->second;
      p.second = resource;
      foundItr->second = p;
   }
   else
   {
      //add it
      RefActorType refType = NULL;
      std::pair<RefActorType, dtCore::ResourceDescriptor> p(refType,resource);
      mEntityMap.insert(EntityMapping::value_type(eid, p));
   }
}

//////////////////////////////////////////////////////////////////////////
const dtCore::ActorType* EntityMap::GetMappedActorType(const DIS::EntityType& entityType) const
{
   EntityMapping::const_iterator iter = mEntityMap.find(entityType);
   if (iter != mEntityMap.end())
   {
      return iter->second.first.get();
   }
   else
   {
      return GetBestMatchedActorType(entityType);
   }
}

//////////////////////////////////////////////////////////////////////////
void EntityMap::RemoveEntityActorType(const DIS::EntityType& eid)
{
   EntityMapping::iterator iter = mEntityMap.find(eid);
   if (iter != mEntityMap.end())
   {
      if ((iter->second.second == dtCore::ResourceDescriptor::NULL_RESOURCE ||
           iter->second.second.IsEmpty()))
      {
         //found in the map, but no resource mapped to it, so we're ok to remove
         //the whole key/value pair
         mEntityMap.erase(iter);
      }
      else
      {
         //it still has a valid ResourceDescriptor assigned, so we'll just remove
         //the ActorType
         iter->second.first = NULL;
      }
   }   
}

//////////////////////////////////////////////////////////////////////////
const dtCore::ResourceDescriptor& EntityMap::GetMappedResource(const DIS::EntityType& entityType) const
{
   EntityMapping::const_iterator iter = mEntityMap.find(entityType);
   if (iter != mEntityMap.end())
   {
      return iter->second.second;
   }
   else
   {
      return GetBestMatchedResource(entityType);
   }
}

//////////////////////////////////////////////////////////////////////////
void EntityMap::RemoveEntityResource(const DIS::EntityType& eid)
{
   EntityMapping::iterator iter = mEntityMap.find(eid);
   if (iter != mEntityMap.end())
   {
      if (iter->second.first == NULL)
      {
         //found in the map, but no ActorType mapped to it, so we're ok to remove
         //the whole key/value pair
         mEntityMap.erase(iter);
      }
      else
      {
         //it still has a valid ActorType assigned, so we'll just remove
         //the ResourceDescriptor
         iter->second.second = dtCore::ResourceDescriptor::NULL_RESOURCE;
      }
   }   
}

//////////////////////////////////////////////////////////////////////////
const dtCore::ActorType* EntityMap::GetBestMatchedActorType(const DIS::EntityType& entityType) const
{
   EntityMapping::const_iterator itr = FindBestMatchedEntityType(entityType);

   if (itr != mEntityMap.end())
   {
      return itr->second.first;
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////
const dtCore::ResourceDescriptor& EntityMap::GetBestMatchedResource(const DIS::EntityType& entityType) const
{
   EntityMapping::const_iterator itr = FindBestMatchedEntityType(entityType);

   if (itr != mEntityMap.end())
   {
      return itr->second.second;
   }

   return dtCore::ResourceDescriptor::NULL_RESOURCE;
}

//////////////////////////////////////////////////////////////////////////
EntityMap::EntityMapping::const_iterator EntityMap::FindBestMatchedEntityType(const DIS::EntityType &entityType) const
{
   short bestRank = 0;
   EntityMapping::const_iterator bestMatchedItr = mEntityMap.end();
   EntityMapping::const_iterator iter = mEntityMap.begin();
   while (iter != mEntityMap.end())
   {
      short currentRank = 0;
      if (iter->first.getEntityKind() == entityType.getEntityKind())
      {
         currentRank++;
         if (iter->first.getDomain() == entityType.getDomain())
         {
            currentRank++;
            if (iter->first.getCountry() == entityType.getCountry())
            {
               currentRank++;
               if (iter->first.getCategory() == entityType.getCategory())
               {
                  currentRank++;
                  if (iter->first.getSubcategory() == entityType.getSubcategory())
                  {
                     currentRank++;
                     if (iter->first.getSpecific() == entityType.getSpecific())
                     {
                        currentRank++;
                        if (iter->first.getExtra() == entityType.getExtra())
                        {
                           currentRank++;
                        }
                     }
                  }
               }
            }
         }
      }

      if (currentRank > bestRank)
      {
         bestRank = currentRank;
         bestMatchedItr = iter;
      }
      ++iter;
   }
   
   return bestMatchedItr;
}
