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

#ifndef ENTITYMAP_H__
#define ENTITYMAP_H__

#include <dtDIS/dtdisexport.h>
#include <dtCore/actortype.h>           // for mapped dependency type.
#include <dtCore/resourcedescriptor.h>  // for mapped type
#include <DIS/EntityType.h>            // for mapped dependency type.

#include <map>

namespace dtDIS
{
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
   namespace details
   {
      /// the model of how to compare DIS::EntityType instances.
      struct EntityTypeCompare
      {
         bool operator ()(const DIS::EntityType& lhs, const DIS::EntityType& rhs) const
         {
            if (lhs.getCategory() != rhs.getCategory())
               return lhs.getCategory() < rhs.getCategory();
            else if (lhs.getCountry() != rhs.getCountry())
               return lhs.getCountry() < rhs.getCountry();
            else if (lhs.getDomain() != rhs.getDomain())
               return lhs.getDomain() < rhs.getDomain();
            else if (lhs.getEntityKind() != rhs.getEntityKind())
               return lhs.getEntityKind() < rhs.getEntityKind();
            else if (lhs.getExtra() != rhs.getExtra())
               return lhs.getExtra() < rhs.getExtra();
            else if (lhs.getSpecific() != rhs.getSpecific())
               return lhs.getSpecific() < rhs.getSpecific();
            else if (lhs.getSubcategory() != rhs.getSubcategory())
               return lhs.getSubcategory() < rhs.getSubcategory();           
            else 
               return false;
         }
      };
   } // end namespace details
   ///@endcond

   ///A structure holding the game data mapped to a particular DIS EntityType
   class DT_DIS_EXPORT EntityMap
   {
   public:
      EntityMap();
      ~EntityMap();

      /** Set the ActorType to create for the supplied DIS Entity type
      * @param eid The DIS Entity Type
      * @param at The ActorType to use
      */
      void SetEntityActorType(const DIS::EntityType& eid, 
                              const dtCore::ActorType* at);
      
      /** Get the ActorType mapped to the supplied DIS EntityType.
      * @param entityType The DIS Entity type
      * @return The mapped ActorType (could be NULL)
      */     
      const dtCore::ActorType* GetMappedActorType(const DIS::EntityType& entityType) const;

      /** Remove any ActorTypes mapped to the supplied DIS EntityType.
      * @param entityType The DIS Entity type
      */
      void RemoveEntityActorType(const DIS::EntityType& eid);

      /** Set the ResourceDescriptor to apply on the Actor for the supplied DIS Entity type.
      * @param eid The DIS Entity Type
      * @param resource The ResourceDescriptor to use
      */
      void SetEntityResource(const DIS::EntityType& eid, 
                             const dtCore::ResourceDescriptor& resource);

      
      /** Get the ActorType mapped to the supplied DIS EntityType.
      * @param entityType The DIS Entity type
      * @return The mapped ActorType (could be NULL)
      */     
      const dtCore::ResourceDescriptor& GetMappedResource(const DIS::EntityType& entityType) const;

      /** Remove any ResourceDescriptors mapped to the supplied DIS EntityType.
      * @param entityType The DIS Entity type
      */
      void RemoveEntityResource(const DIS::EntityType& eid);

   private:
      const dtCore::ActorType* GetBestMatchedActorType(const DIS::EntityType& entityType) const;
      const dtCore::ResourceDescriptor& GetBestMatchedResource(const DIS::EntityType& entityType) const;

      /// a convenience typedef
      typedef dtCore::RefPtr<const dtCore::ActorType> RefActorType;

      typedef std::map<DIS::EntityType,
         std::pair<RefActorType, dtCore::ResourceDescriptor>,details::EntityTypeCompare > EntityMapping;

      EntityMapping::const_iterator FindBestMatchedEntityType(const DIS::EntityType &entityType) const;

      EntityMapping mEntityMap;    
   };

}
#endif // ENTITYMAP_H__
