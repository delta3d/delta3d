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

#ifndef DELTA_OBJECT_RUNTIME_MAPPING_INFO
#define DELTA_OBJECT_RUNTIME_MAPPING_INFO

#include <map>
#include <dtCore/uniqueid.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/rtihandle.h>

namespace dtHLAGM
{

   /**
    * @class ObjectRuntimeMappingInfo
    * @brief Simple class that holds all of the data used at runtime when mapping 
    *        RTI objects to actor types.
    */
   class DT_HLAGM_EXPORT ObjectRuntimeMappingInfo 
   {
      public:
         ObjectRuntimeMappingInfo();
         
         ~ObjectRuntimeMappingInfo();

         /**
          * Bidirectionally maps the given RTI id string (RTIObjectIdentifierStruct) to the actor id.
          * @return false if either object is already involved in another like mapping.
          */
         bool Put(const std::string& rtiId, const dtCore::UniqueId& actorId);
         
         /**
          * Bidirectionally maps the given object handle to the unique id.
          * @return false if either object is already involved in another like mapping.
          */
         bool Put(RTIObjectInstanceHandle& handle, const dtCore::UniqueId& actorId);

         /**
          * Bidirectionally maps the given entity id to the unique id.
          * @return false if either object is already involved in another like mapping.
          */
         bool Put(const EntityIdentifier& entityId, const dtCore::UniqueId& actorId);

         /**
          * Maps the given object handle to the object to actor.
          * @return false if an object to actor is already mapped to the handle passed in.
          */
         bool Put(RTIObjectInstanceHandle& handle, ObjectToActor& ota);

         ///@return the unique id that was mapped to given object handle or NULL if none has been mapped
         const dtCore::UniqueId* GetId(RTIObjectInstanceHandle& handle) const;
      
         ///@return the unique id that was mapped to given entity id or NULL if none has been mapped
         const dtCore::UniqueId* GetId(const EntityIdentifier& entityId) const;

         ///@return the unique id that was mapped to given rti string id or NULL if none has been mapped
         const dtCore::UniqueId* GetIdByRTIId(const std::string& rtiId) const;

         ///@return the rti string id that was mapped to given unique id or NULL if none has been mapped
         const std::string* GetRTIId(const dtCore::UniqueId& actorId) const;

         ///@return the object handle that was mapped to given unique id or NULL if none has been mapped
         RTIObjectInstanceHandle* GetHandle(const dtCore::UniqueId& actorId) const;

         ///@return the entity id that was mapped to given unique id or NULL if none has been mapped
         const EntityIdentifier* GetEntityId(const dtCore::UniqueId& actorId) const;

         ///@return the object to actor that was mapped to given object handle or NULL if none has been mapped
         const ObjectToActor* GetObjectToActor(RTIObjectInstanceHandle& handle) const;

         ///@return the object to actor that was mapped to given object handle or NULL if none has been mapped
         ObjectToActor* GetObjectToActor(RTIObjectInstanceHandle& handle);

         /**
          * Removes all mappings for the given handle including a unique id, an object to actor, 
          * and an entity id id mapped to the unique id.
          */ 
         void Remove(RTIObjectInstanceHandle& handle);

         /**
          * Removes the mapping to a unique id for the given entity id. 
          * This will also remove the object handle mapped to the unique id 
          * and the object to actor mapped to the object handle.
          */ 
         void Remove(const EntityIdentifier& entityId);

         /**
          * Removes the mapping to an object handle and an entity id for the given unique id. 
          * This will also remove the object to actor mapped to the object handle; and remove
          * the RTI id string mapping to the actor id.
          */ 
         void Remove(const dtCore::UniqueId& actorId);

         /**
          * Removes all mappings in the instance.
          */
         void Clear();

         /// @return a list of all existing actors in the mapping file that are mapped to RTI object handles.
         void GetAllActorIds(std::vector<dtCore::UniqueId>& toFill) const;

      private:
         std::map<dtCore::UniqueId, std::string> mActortoRTIIDMap;
         std::map<std::string, dtCore::UniqueId> mRTIIDtoActorMap;
         std::map<dtCore::RefPtr<RTIObjectInstanceHandle>, dtCore::UniqueId> mHLAtoActorMap;
         std::map<dtCore::UniqueId, dtCore::RefPtr<RTIObjectInstanceHandle> > mActorToHLAMap;
         std::map<EntityIdentifier, dtCore::UniqueId> mEntityIdentifierToUniqueIdMap;
         std::map<dtCore::UniqueId, EntityIdentifier> mUniqueIdToEntityIdentifierMap;
         std::map<dtCore::RefPtr<RTIObjectInstanceHandle>, dtCore::RefPtr<ObjectToActor> > mObjectHandleToClassMap;
   };
}
#endif
