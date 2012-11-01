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
#include <dtHLAGM/objecttoactor.h>

namespace dtHLAGM
{
   IMPLEMENT_ENUM(ObjectToActor::LocalOrRemoteType);
   ObjectToActor::LocalOrRemoteType ObjectToActor::LocalOrRemoteType::LOCAL_AND_REMOTE("LOCAL_AND_REMOTE");
   ObjectToActor::LocalOrRemoteType ObjectToActor::LocalOrRemoteType::LOCAL_ONLY("LOCAL_ONLY");
   ObjectToActor::LocalOrRemoteType ObjectToActor::LocalOrRemoteType::REMOTE_ONLY("REMOTE_ONLY");
   
   ObjectToActor::LocalOrRemoteType::LocalOrRemoteType(const std::string& name)
   : dtUtil::Enumeration(name)
   {
   }

   /////////////////////////////////////////////////////////////////////
   ObjectToActor::ObjectToActor(): 
      mLocalOrRemoteType(&ObjectToActor::LocalOrRemoteType::LOCAL_AND_REMOTE), 
      mEntityTypeSet(false)
   {}

   /////////////////////////////////////////////////////////////////////
   ObjectToActor::~ObjectToActor()
   {}

   /////////////////////////////////////////////////////////////////////
   const dtCore::ActorType& ObjectToActor::GetActorType() const
   {
      return *mActorType;
   }

   /////////////////////////////////////////////////////////////////////
   bool ObjectToActor::IsRemoteOnly() const
   {
      return *mLocalOrRemoteType == ObjectToActor::LocalOrRemoteType::REMOTE_ONLY;
   }


   /////////////////////////////////////////////////////////////////////
   bool ObjectToActor::IsLocalOnly() const
   {
      return *mLocalOrRemoteType == ObjectToActor::LocalOrRemoteType::LOCAL_ONLY;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetLocalOrRemoteType(ObjectToActor::LocalOrRemoteType& newType)
   {
      mLocalOrRemoteType = &newType;
   }


   /////////////////////////////////////////////////////////////////////
   const std::string& ObjectToActor::GetDDMCalculatorName() const
   {
      return mDDMCalculatorName;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetDDMCalculatorName(const std::string& newName)
   {
      mDDMCalculatorName = newName;
   }

   /////////////////////////////////////////////////////////////////////
   const std::string& ObjectToActor::GetObjectClassName() const
   {
      return mObjectClassName;
   }

   /////////////////////////////////////////////////////////////////////
   RTIObjectClassHandle* ObjectToActor::GetObjectClassHandle() const
   {
      return mObjectClassHandle;
   }

   /////////////////////////////////////////////////////////////////////
   RTIAttributeHandle* ObjectToActor::GetEntityIdAttributeHandle() const
   {
      return mEntityIdAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   RTIAttributeHandle* ObjectToActor::GetEntityTypeAttributeHandle() const
   {
      return mEntityTypeAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   const EntityType* ObjectToActor::GetEntityType() const
   {
      if (!mEntityTypeSet) return NULL;
      return &mObjectEntityType;
   }

   /////////////////////////////////////////////////////////////////////
   EntityType* ObjectToActor::GetEntityType()
   {
      if (!mEntityTypeSet) return NULL;
      return &mObjectEntityType;
   }

   /////////////////////////////////////////////////////////////////////
   const std::string& ObjectToActor::GetEntityIdAttributeName() const 
   { 
      return mEntityIdAttribute;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetEntityIdAttributeName(const std::string& newName)
   { 
      mEntityIdAttribute = newName;
   }

   /////////////////////////////////////////////////////////////////////
   const std::string& ObjectToActor::GetEntityTypeAttributeName() const
   {
      return mEntityTypeAttribute;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetEntityTypeAttributeName(const std::string& newName)
   {
      mEntityTypeAttribute = newName;
   }

   /////////////////////////////////////////////////////////////////////
   const std::vector<AttributeToPropertyList>& ObjectToActor::GetOneToManyMappingVector() const
   {
      return mOneToMany;
   }

   /////////////////////////////////////////////////////////////////////
   std::vector<AttributeToPropertyList>& ObjectToActor::GetOneToManyMappingVector()
   {
      return mOneToMany;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetActorType(const dtCore::ActorType& type)
   {
      mActorType = &type;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetObjectClassName(const std::string& objTypeName)
   {
      mObjectClassName = objTypeName;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetObjectClassHandle(RTIObjectClassHandle* objClassHandle)
   {
      mObjectClassHandle = objClassHandle;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetEntityIdAttributeHandle(RTIAttributeHandle* newEntityIdAttributeHandle)
   {
      mEntityIdAttributeHandle = newEntityIdAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetEntityTypeAttributeHandle(RTIAttributeHandle* newEntityTypeAttributeHandle)
   {
      mEntityTypeAttributeHandle = newEntityTypeAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetEntityType(const EntityType* thisEntityType)
   {
      if (thisEntityType == NULL)
      {
         mEntityTypeSet = false;
      }
      else
      {
         mObjectEntityType = *thisEntityType;
         mEntityTypeSet = true;
      }
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetOneToManyMappingVector(std::vector<AttributeToPropertyList> &thisOneToManyMapping)
   {
      mOneToMany = thisOneToManyMapping;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetMappingName( const std::string& name )
   {
      mMappingName = name;
   }

   /////////////////////////////////////////////////////////////////////
   const std::string& ObjectToActor::GetMappingName() const
   {
      return mMappingName;
   }

   /////////////////////////////////////////////////////////////////////
   ObjectToActor& ObjectToActor::operator=(const ObjectToActor& setTo)
   {
      mActorType = setTo.mActorType;
      mObjectClassName = setTo.mObjectClassName;
      mObjectClassHandle = setTo.mObjectClassHandle;
      mObjectEntityType = setTo.mObjectEntityType;
      mEntityTypeSet = setTo.mEntityTypeSet;
      if (mEntityTypeSet)
         mObjectEntityType = setTo.mObjectEntityType;
      mEntityIdAttribute = setTo.mEntityIdAttribute;
      mEntityTypeAttribute = setTo.mEntityTypeAttribute;
      mOneToMany = setTo.mOneToMany;

      return *this;
   }

   /////////////////////////////////////////////////////////////////////
   bool ObjectToActor::operator==(const ObjectToActor& toCompare) const
   {
      return mActorType == toCompare.mActorType &&
      mObjectClassName == toCompare.mObjectClassName &&
      mObjectEntityType == toCompare.mObjectEntityType &&
      mEntityTypeSet == toCompare.mEntityTypeSet &&
      mOneToMany == toCompare.mOneToMany &&
      mEntityIdAttribute == toCompare.mEntityIdAttribute &&
      mEntityTypeAttribute == toCompare.mEntityTypeAttribute &&
      (!mEntityTypeSet ||
               mObjectEntityType == toCompare.mObjectEntityType);
   }

   /////////////////////////////////////////////////////////////////////
   bool ObjectToActor::operator!=(const ObjectToActor& toCompare) const
   {
      return !operator==(toCompare);
   }
}
