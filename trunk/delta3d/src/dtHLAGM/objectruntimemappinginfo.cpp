/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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

#include "dtHLAGM/objectruntimemappinginfo.h"

namespace dtHLAGM
{
   ObjectRuntimeMappingInfo::ObjectRuntimeMappingInfo()
   {}

   ObjectRuntimeMappingInfo::~ObjectRuntimeMappingInfo()
   {}

   bool ObjectRuntimeMappingInfo::Put(const std::string& rtiId, const dtCore::UniqueId& actorId)
   {
      //do a quick pre-check.  This will make sure that it doesn't insert in the first map only to fail at the second.
      if (mActortoRTIIDMap.find(actorId) != mActortoRTIIDMap.end())
         return false;

      bool ok = mRTIIDtoActorMap.insert(std::make_pair(rtiId, actorId)).second;
      if (ok)
         ok = mActortoRTIIDMap.insert(std::make_pair(actorId, rtiId)).second;

      return ok;
   }

   bool ObjectRuntimeMappingInfo::Put(const RTI::ObjectHandle& handle, const dtCore::UniqueId& actorId)
   {
      //do a quick pre-check.  This will make sure that it doesn't insert in the first map only to fail at the second.
      if (mActorToHLAMap.find(actorId) != mActorToHLAMap.end())
         return false;

      bool ok = mHLAtoActorMap.insert(std::make_pair(handle, actorId)).second;
      if (ok)
         ok = mActorToHLAMap.insert(std::make_pair(actorId, handle)).second;

      return ok;
   }

   bool ObjectRuntimeMappingInfo::Put(const EntityIdentifier& entityId, const dtCore::UniqueId& actorId)
   {
      //do a quick pre-check.  This will make sure that it doesn't insert in the first map only to fail at the second.
      if (mUniqueIdToEntityIdentifierMap.find(actorId) != mUniqueIdToEntityIdentifierMap.end())
         return false;

      bool ok = mEntityIdentifierToUniqueIdMap.insert(std::make_pair(entityId, actorId)).second;
      if (ok)
         ok = mUniqueIdToEntityIdentifierMap.insert(std::make_pair(actorId, entityId)).second;

      return ok;
   }

   bool ObjectRuntimeMappingInfo::Put(const RTI::ObjectHandle& handle, ObjectToActor& ota)
   {
      return mObjectHandleToClassMap.insert(std::make_pair(handle, &ota)).second;
   }

   const dtCore::UniqueId* ObjectRuntimeMappingInfo::GetId(const RTI::ObjectHandle& handle) const
   {
      std::map<RTI::ObjectHandle, dtCore::UniqueId>::const_iterator i = mHLAtoActorMap.find(handle);
      if (i != mHLAtoActorMap.end())
      {
         return &i->second;
      }
      return NULL;
   }

   const dtCore::UniqueId* ObjectRuntimeMappingInfo::GetId(const EntityIdentifier& entityId) const
   {
      std::map<EntityIdentifier, dtCore::UniqueId>::const_iterator i = mEntityIdentifierToUniqueIdMap.find(entityId);
      if (i != mEntityIdentifierToUniqueIdMap.end())
      {
         return &i->second;
      }
      return NULL;
   }

   const dtCore::UniqueId* ObjectRuntimeMappingInfo::GetIdByRTIId(const std::string& rtiId) const
   {
      std::map<std::string, dtCore::UniqueId>::const_iterator i = mRTIIDtoActorMap.find(rtiId);
      if (i != mRTIIDtoActorMap.end())
      {
         return &i->second;
      }
      return NULL;
   }

   const std::string* ObjectRuntimeMappingInfo::GetRTIId(const dtCore::UniqueId& id) const
   {
      std::map<dtCore::UniqueId, std::string>::const_iterator i = mActortoRTIIDMap.find(id);
      if (i != mActortoRTIIDMap.end())
      {
         return &i->second;
      }
      return NULL;
   }

   const RTI::ObjectHandle* ObjectRuntimeMappingInfo::GetHandle(const dtCore::UniqueId& actorId) const
   {
      std::map<dtCore::UniqueId, RTI::ObjectHandle>::const_iterator i = mActorToHLAMap.find(actorId);
      if (i != mActorToHLAMap.end())
      {
         return &i->second;
      }
      return NULL;
   }

   const EntityIdentifier* ObjectRuntimeMappingInfo::GetEntityId(const dtCore::UniqueId& actorId) const
   {
      std::map<dtCore::UniqueId, EntityIdentifier>::const_iterator i = mUniqueIdToEntityIdentifierMap.find(actorId);
      if (i != mUniqueIdToEntityIdentifierMap.end())
      {
         return &i->second;
      }
      return NULL;
   }

   const ObjectToActor* ObjectRuntimeMappingInfo::GetObjectToActor(const RTI::ObjectHandle& handle) const
   {
      std::map<RTI::ObjectHandle, dtCore::RefPtr<ObjectToActor> >::const_iterator i = mObjectHandleToClassMap.find(handle);
      if (i != mObjectHandleToClassMap.end())
      {
         return i->second.get();
      }
      return NULL;
   }

   ObjectToActor* ObjectRuntimeMappingInfo::GetObjectToActor(const RTI::ObjectHandle& handle)
   {
      std::map<RTI::ObjectHandle, dtCore::RefPtr<ObjectToActor> >::iterator i = mObjectHandleToClassMap.find(handle);
      if (i != mObjectHandleToClassMap.end())
      {
         return i->second.get();
      }
      return NULL;
   }


   void ObjectRuntimeMappingInfo::Remove(const RTI::ObjectHandle& handle)
   {
      std::map<RTI::ObjectHandle, dtCore::UniqueId>::iterator i = mHLAtoActorMap.find(handle);
      if (i != mHLAtoActorMap.end())
      {
         const dtCore::UniqueId id = i->second;
         mHLAtoActorMap.erase(i);
         Remove(id);
      }

      std::map<RTI::ObjectHandle, dtCore::RefPtr<ObjectToActor> >::iterator i2 = mObjectHandleToClassMap.find(handle);
      if (i2 != mObjectHandleToClassMap.end())
         mObjectHandleToClassMap.erase(i2);
   }

   void ObjectRuntimeMappingInfo::Remove(const EntityIdentifier& entityId)
   {
      std::map<EntityIdentifier, dtCore::UniqueId>::iterator i = mEntityIdentifierToUniqueIdMap.find(entityId);
      if (i != mEntityIdentifierToUniqueIdMap.end())
      {
         const dtCore::UniqueId id = i->second;
         mEntityIdentifierToUniqueIdMap.erase(i);
         Remove(id);
      }
   }

   void ObjectRuntimeMappingInfo::Remove(const dtCore::UniqueId& actorId)
   {
      std::map<dtCore::UniqueId, EntityIdentifier>::iterator i = mUniqueIdToEntityIdentifierMap.find(actorId);
      if (i != mUniqueIdToEntityIdentifierMap.end())
      {
         const EntityIdentifier id = i->second;
         mUniqueIdToEntityIdentifierMap.erase(i);
         Remove(id);
      }

      std::map<dtCore::UniqueId, RTI::ObjectHandle>::iterator i2 = mActorToHLAMap.find(actorId);
      if (i2 != mActorToHLAMap.end())
      {
         const RTI::ObjectHandle handle = i2->second;
         mActorToHLAMap.erase(i2);
         Remove(handle);
      }

      // Remove the RTI id string mapped to the actor id
      std::map<dtCore::UniqueId, std::string>::iterator i3 = mActortoRTIIDMap.find(actorId);
      if (i3 != mActortoRTIIDMap.end())
      {
         std::string rtiId = i3->second;
         mActortoRTIIDMap.erase(i3);

         // Remove the actor id mapped to RTI id string
         std::map<std::string, dtCore::UniqueId>::iterator i4 = mRTIIDtoActorMap.find(rtiId);
         if (i4 != mRTIIDtoActorMap.end())
         {
            mRTIIDtoActorMap.erase(i4);
         }
      }
   }

   void ObjectRuntimeMappingInfo::Clear()
   {
      mActortoRTIIDMap.clear();
      mRTIIDtoActorMap.clear();
      mHLAtoActorMap.clear();
      mActorToHLAMap.clear();
      mEntityIdentifierToUniqueIdMap.clear();
      mUniqueIdToEntityIdentifierMap.clear();
      mObjectHandleToClassMap.clear();
   }
   
   void ObjectRuntimeMappingInfo::GetAllActorIds(std::vector<dtCore::UniqueId>& toFill) const
   { 
      toFill.clear();
      toFill.reserve(mActorToHLAMap.size());
      for (std::map<dtCore::UniqueId, RTI::ObjectHandle>::const_iterator i = mActorToHLAMap.begin();
         i != mActorToHLAMap.end(); ++i)
      {
         toFill.push_back(i->first);
      }
   }

}
