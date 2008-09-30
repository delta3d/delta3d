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
      mDISIDSet(false),
      mObjectClassHandle(0), 
      mEntityIdAttributeHandle(0), 
      mDisIDAttributeHandle(0)
   {}

   /////////////////////////////////////////////////////////////////////
   ObjectToActor::~ObjectToActor()
   {}

   /////////////////////////////////////////////////////////////////////
   const dtDAL::ActorType& ObjectToActor::GetActorType() const
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
   const RTI::ObjectClassHandle ObjectToActor::GetObjectClassHandle() const
   {
      return mObjectClassHandle;
   }

   /////////////////////////////////////////////////////////////////////
   const RTI::AttributeHandle ObjectToActor::GetEntityIdAttributeHandle() const
   {
      return mEntityIdAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   const RTI::AttributeHandle ObjectToActor::GetDisIDAttributeHandle() const
   {
      return mDisIDAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   const EntityType* ObjectToActor::GetDisID() const
   {
      if (!mDISIDSet) return NULL;
      return &mObjectDisID;
   }

   /////////////////////////////////////////////////////////////////////
   EntityType* ObjectToActor::GetDisID()
   {
      if (!mDISIDSet) return NULL;
      return &mObjectDisID;
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
   void ObjectToActor::SetActorType(const dtDAL::ActorType& type)
   {
      mActorType = &type;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetObjectClassName(const std::string& objTypeName)
   {
      mObjectClassName = objTypeName;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetObjectClassHandle(const RTI::ObjectClassHandle& objClassHandle)
   {
      mObjectClassHandle = objClassHandle;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetEntityIdAttributeHandle(const RTI::AttributeHandle newEntityIdAttributeHandle)
   {
      mEntityIdAttributeHandle = newEntityIdAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetDisIDAttributeHandle(const RTI::AttributeHandle newDisIDAttributeHandle)
   {
      mDisIDAttributeHandle = newDisIDAttributeHandle;
   }

   /////////////////////////////////////////////////////////////////////
   void ObjectToActor::SetDisID(const EntityType* thisDisID)
   {
      if (thisDisID == NULL)
      {
         mDISIDSet = false;
      }
      else
      {
         mObjectDisID = *thisDisID;
         mDISIDSet = true;
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
      mObjectDisID = setTo.mObjectDisID;
      mDISIDSet = setTo.mDISIDSet;
      if (mDISIDSet)
         mObjectDisID = setTo.mObjectDisID;
      mEntityIdAttribute = setTo.mEntityIdAttribute;
      mOneToMany = setTo.mOneToMany;

      return *this;
   }

   /////////////////////////////////////////////////////////////////////
   bool ObjectToActor::operator==(const ObjectToActor& toCompare) const
   {
      return mActorType == toCompare.mActorType &&
      mObjectClassName == toCompare.mObjectClassName &&
      mObjectDisID == toCompare.mObjectDisID &&
      mDISIDSet == toCompare.mDISIDSet &&
      mOneToMany == toCompare.mOneToMany &&
      mEntityIdAttribute == toCompare.mEntityIdAttribute &&

      (!mDISIDSet ||
               mObjectDisID == toCompare.mObjectDisID);
   }

   /////////////////////////////////////////////////////////////////////
   bool ObjectToActor::operator!=(const ObjectToActor& toCompare) const
   {
      return !operator==(toCompare);
   }
}
