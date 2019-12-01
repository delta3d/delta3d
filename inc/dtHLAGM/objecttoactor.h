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
 * Olen A. Bruce
 * David Guthrie
 */

#ifndef DELTA_OBJECT_TO_ACTOR
#define DELTA_OBJECT_TO_ACTOR

//
// objecttoactor.h: Declaration of the ObjectToActor class.
//
///////////////////////////////////////////////////////////////////////

#include <dtHLAGM/rtiambassador.h>

#include <string>
#include <vector>
#include <dtCore/actortype.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/export.h>

#include <dtUtil/enumeration.h>


namespace dtHLAGM
{
   /**
    * Defines a one-to-one the mapping between and HLA object and a game actor
    */
   class DT_HLAGM_EXPORT ObjectToActor : public osg::Referenced
   {
      public:
         
         class DT_HLAGM_EXPORT LocalOrRemoteType : public dtUtil::Enumeration
         {
            DECLARE_ENUM(LocalOrRemoteType);
            public:
               static LocalOrRemoteType LOCAL_AND_REMOTE;
               static LocalOrRemoteType LOCAL_ONLY;
               static LocalOrRemoteType REMOTE_ONLY;
            protected:
               LocalOrRemoteType(const std::string& name);
         };
         
         /**
          * Constructor.
          */
         ObjectToActor();

         /**
          * Gets the Game ActorType from the Object to Actor mapping.
          *
          * @return the Game Actor Type
          */
         const dtCore::ActorType& GetActorType() const;

         /**
          * @return true if this mapping should only be used for objects simulated remotely in HLA.
          */
         bool IsRemoteOnly() const;

         /**
          * @return true if this mapping should only be used for objects simulated locally.
          */
         bool IsLocalOnly() const;

         /**
          * REMOTE_ONLY means this mapping should only apply to objects simulated remotely.  LOCAL_ONLY if it
          * for locally simulated ones only.  LOCAL_AND_REMOTE for both.
          * This defaults to LOCAL_AND_REMOTE.
          * @note if more than one object class/DIS ID intends to use the same actor type, only one
          * may have this property set to anything other than REMOTE_ONLY.
          * @note if more than one actor type intends to use the same DIS ID, only one
          * may have this property set to anything other than LOCAL_ONLY.
          * @param newType the new value of the property
          */
         void SetLocalOrRemoteType(LocalOrRemoteType& newType);

         /**
          * Gets the name of the DDMRegionCalculator to use for subscription and publishing.
          *
          * @return the calculator name.
          */
         const std::string& GetDDMCalculatorName() const;

         /**
          * Gets the name of the DDMRegionCalculator to use for subscription and publishing.
          *
          * @return the calculator name.
          */
         void SetDDMCalculatorName(const std::string& newName);

         /**
          * Gets the HLA Object Type Name from the Object to Actor mapping.
          *
          * @return HLA Object Type Name
          */
         const std::string& GetObjectClassName() const;

         /**
          * Gets the HLA Object Class Handle from the Object to Actor mapping.
          *
          * @return HLA Object Class Handle
          */
         RTIObjectClassHandle* GetObjectClassHandle() const;

         /// @return The attribute handle storing the entity id attribute.
         RTIAttributeHandle* GetEntityIdAttributeHandle() const;

         /// @return The attribute handle storing the dis id attribute.
         RTIAttributeHandle* GetEntityTypeAttributeHandle() const;

         /**
          * Gets the Object DIS ID from the Object to Actor mapping
          *
          * @return DIS ID
          */
         const EntityType* GetEntityType() const;

         /**
          * Gets the Object DIS ID from the Object to Actor mapping
          *
          * @return DIS ID
          */
         EntityType* GetEntityType();

         ///@return the name of the attribute that hold the entity id or empty for not used.
         const std::string& GetEntityIdAttributeName() const;

         ///Assigns the attribute name that holds the entity id so that it can be mapped to an actor id.  Set to empty string for unused.
         void SetEntityIdAttributeName(const std::string& newName);

         ///@return the name of the attribute that hold the entity type or empty for the default.
         const std::string& GetEntityTypeAttributeName() const;

         ///Assigns the attribute name that holds the entity type so that it can be mapped to an actor type.  Set to use the default.
         void SetEntityTypeAttributeName(const std::string& newName);

         const std::vector<AttributeToPropertyList>& GetOneToManyMappingVector() const;

         std::vector<AttributeToPropertyList>& GetOneToManyMappingVector();

         /**
          * Sets the Game ActorType for this Object to Actor mapping.
          *
          * @param type the Game Actor Type
          */
         void SetActorType(const dtCore::ActorType& type);

         /**
          * Sets the HLA Object Type Name for this Object to Actor mapping.
          *
          * @param objTypeName the HLA Object Type Name
          */
         void SetObjectClassName(const std::string& objTypeName);

         /**
          * Sets the HLA Object Class Handle for this Object to Actor mapping.
          *
          * @param objClassHandle the HLA Object Class Handle
          */
         void SetObjectClassHandle(RTIObjectClassHandle* objClassHandle);

         /// Sets The attribute handle storing the entity id attribute.
         void SetEntityIdAttributeHandle(RTIAttributeHandle* newEntityIdAttributeHandle);

         /// Sets The attribute handle storing DIS Id attribute.
         void SetEntityTypeAttributeHandle(RTIAttributeHandle* newEntityTypeAttributeHandle);

         /**
          * Sets the Object DIS ID for this Object to Actor mapping.
          *
          * @param objectEntityType the DIS ID
          */
         void SetEntityType(const EntityType* thisEntityType);

         /**
          * Sets the One to One Mapping vector for this Object to Actor Mapping.
          *
          * &param thisOneToManyMapping the OnetoOneMapping vector
          */
         void SetOneToManyMappingVector(std::vector<AttributeToPropertyList>& thisOneToManyMapping);

         /**
          * Get the object mapping name.
          * @param Name assigned to the mapping as found in the "name" attribute
          *        of an object tag in a mapping XML.
          */
         void SetMappingName( const std::string& name );

         /**
          * Get the object mapping name.
          * @return Name assigned to the mapping as found in the "name" attribute
          *         of an object tag in a mapping XML.
          */
         const std::string& GetMappingName() const;

         ObjectToActor& operator=(const ObjectToActor& setTo);

         bool operator==(const ObjectToActor& toCompare) const;

         bool operator!=(const ObjectToActor& toCompare) const;
         
      private:

         /**
          * Destructor.
          */
         ~ObjectToActor();

         /// The Actor Type for this Object to Actor mapping.

         dtCore::RefPtr <const dtCore::ActorType> mActorType;

         /// true if this mapping should only be used for objects simulated remotely/locally or both.
         LocalOrRemoteType* mLocalOrRemoteType;

         /// name of the calculator/calculators that will handle subscription and publishing for this  
         std::string mDDMCalculatorName;

         /// The HLA Object Type Name for this Object to Actor mapping.
         std::string mObjectClassName;

         /// The name of the mapping as seen ing the name attribute of the mapping XML object tag.
         std::string mMappingName;

         /// The Object DIS ID for this Object to Actor mapping.
         EntityType mObjectEntityType;
         bool mEntityTypeSet;

         /// The HLA Object Class Handle for this Object to Actor mapping.
         dtCore::RefPtr<RTIObjectClassHandle> mObjectClassHandle;

         ///Entity identifier attribute handle.  This will be set after connection if the name is not empty.
         dtCore::RefPtr<RTIAttributeHandle> mEntityIdAttributeHandle;

         //The name of the attribute used for the entity id.
         std::string mEntityIdAttribute;

         //The name of the attribute used for the entity type.
         std::string mEntityTypeAttribute;

         ///DIS Id attribute handle.  This will be set after connection if the name is not empty.
         dtCore::RefPtr<RTIAttributeHandle> mEntityTypeAttributeHandle;

         /// A vector of One to One mappings for this Object to Actor mapping.
         std::vector<AttributeToPropertyList> mOneToMany;

   };

}

#endif // DELTA_OBJECT_TO_ACTOR
