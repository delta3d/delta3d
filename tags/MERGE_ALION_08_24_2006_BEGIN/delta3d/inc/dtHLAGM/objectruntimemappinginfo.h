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
 * @author David Guthrie
 */
#include <map>
#include <dtCore/uniqueid.h>
#include "dtHLAGM/distypes.h"
#include "dtHLAGM/objecttoactor.h"

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
          * Bidirectionally maps the given object handle to the unique id.
          * @return false if either object is already involved in another like mapping.
          */
         bool Put(const RTI::ObjectHandle& handle, const dtCore::UniqueId& actorId);

         /**
          * Bidirectionally maps the given entity id to the unique id.
          * @return false if either object is already involved in another like mapping.
          */
         bool Put(const EntityIdentifier& entityId, const dtCore::UniqueId& actorId);

         /**
          * Maps the given object handle to the object to actor.
          * @return false if an object to actor is already mapped to the handle passed in.
          */
         bool Put(const RTI::ObjectHandle& handle, ObjectToActor& ota);

         ///@return the unique id that was mapped to given object handle or NULL if none has been mapped
         const dtCore::UniqueId* GetId(const RTI::ObjectHandle& handle) const;
      
         ///@return the unique id that was mapped to given entity id or NULL if none has been mapped
         const dtCore::UniqueId* GetId(const EntityIdentifier& entityId) const;
      
         ///@return the object handle that was mapped to given unique id or NULL if none has been mapped
         const RTI::ObjectHandle* GetHandle(const dtCore::UniqueId& actorId) const;

         ///@return the entity id that was mapped to given unique id or NULL if none has been mapped
         const EntityIdentifier* GetEntityId(const dtCore::UniqueId& actorId) const;

         ///@return the object to actor that was mapped to given object handle or NULL if none has been mapped
         const ObjectToActor* GetObjectToActor(const RTI::ObjectHandle& handle) const;

         ///@return the object to actor that was mapped to given object handle or NULL if none has been mapped
         ObjectToActor* GetObjectToActor(const RTI::ObjectHandle& handle);

         /**
          * Removes all mappings for the given handle including a unique id, an object to actor, 
          * and an entity id mapped to the unique id.
          */ 
         void Remove(const RTI::ObjectHandle& handle);

         /**
          * Removes the mapping to a unique id for the given entity id. 
          * This will also remove the object handle mapped to the unique id 
          * and the object to actor mapped to the object handle.
          */ 
         void Remove(const EntityIdentifier& entityId);

         /**
          * Removes the mapping to an object handle and an entity id for the given unique id. 
          * this will also remove the object to actor mapped to the object handle.
          */ 
         void Remove(const dtCore::UniqueId& actorId);

         /**
          * Removes all mappings in the instance.
          */
         void Clear();
      private:
         std::map<RTI::ObjectHandle, dtCore::UniqueId> mHLAtoActorMap;
         std::map<dtCore::UniqueId, RTI::ObjectHandle> mActorToHLAMap;
         std::map<EntityIdentifier, dtCore::UniqueId> mEntityIdentifierToUniqueIdMap;
         std::map<dtCore::UniqueId, EntityIdentifier> mUniqueIdToEntityIdentifierMap;
         std::map<RTI::ObjectHandle, dtCore::RefPtr<ObjectToActor> > mObjectHandleToClassMap;
   };
}
