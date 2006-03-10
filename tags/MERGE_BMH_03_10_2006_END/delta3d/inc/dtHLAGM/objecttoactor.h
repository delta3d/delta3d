/* 
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
 * @author Olen A. Bruce
 * @author David Guthrie
 */

#ifndef DELTA_OBJECT_TO_ACTOR
#define DELTA_OBJECT_TO_ACTOR

//
// objecttoactor.h: Declaration of the ObjectToActor class.
//
///////////////////////////////////////////////////////////////////////
#define RTI_USES_STD_FSTREAM
#include "RTI.hh"

#include <string>
#include <vector>
#include <dtDAL/actortype.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>
#include "dtHLAGM/onetoonemapping.h"
#include "dtHLAGM/distypes.h"
#include "dtHLAGM/attributetoproperty.h"

namespace dtHLAGM
{
   /**
    * Defines a one-to-one the mapping between and HLA object and a game actor
    */
   class DT_HLAGM_EXPORT ObjectToActor : public osg::Referenced
   {
      public:
     
         /**
          * Constructor.
          */
         ObjectToActor():mRemoteOnly(false), mDISIDSet(false)
         {}
                  
         /**
          * Gets the Game ActorType from the Object to Actor mapping.
          *
          * @return the Game Actor Type
          */
         const dtDAL::ActorType& GetActorType() const
         {
            return *mActorType;
         }
         
         /**
          * Gets the Game ActorType from the Object to Actor mapping.
          *
          * @return the Game Actor Type
          */
         dtDAL::ActorType& GetActorType()
         {
            return *mActorType;
         }
         
         /**
          * @return true if this mapping should only be used for objects simulation remotely in HLA.
          */
         bool IsRemoteOnly() const
         {
         	return mRemoteOnly;
         }

         /**
          * True means this mapping should only apply to objects simulated remotely.  False if it
          * may be used for mappings in either direction.
          * This defaults to false.
          * @note if more than one object class/DIS ID intends to use the same actor type, only one
          * may have this property set to false.
          * @param newRemoteOnly the new value of the remote property
          */
   		 void SetRemoteOnly(bool newRemoteOnly) 
   		 {
   		 	mRemoteOnly = newRemoteOnly;
   		 }
         
         /**
          * Gets the HLA Object Type Name from the Object to Actor mapping.
          *
          * @return HLA Object Type Name
          */
         const std::string &GetObjectTypeName() const
         {
            return mObjectTypeName;
         }
         
         /** 
          * Gets the HLA Object Class Handle from the Object to Actor mapping.
          *
          * @return HLA Object Class Handle
          */
         const RTI::ObjectClassHandle &GetObjectClassHandle() const
         {
            return mObjectClassHandle;
         }
         
         /** 
          * Gets the Object DIS ID from the Object to Actor mapping
          *
          * @return DIS ID
          */
         const EntityType* GetDisID() const
         {
            if (!mDISIDSet) return NULL;
            return &mObjectDisID;
         }
         
         /** 
          * Gets the Object DIS ID from the Object to Actor mapping
          *
          * @return DIS ID
          */
         EntityType* GetDisID()
         {
            if (!mDISIDSet) return NULL;
            return &mObjectDisID;
         }
         
         const std::vector<AttributeToProperty> &GetOneToOneMappingVector() const
         {
            return mOneToOne;
         }
         
         std::vector<AttributeToProperty> &GetOneToOneMappingVector()
         {
            return mOneToOne;
         }
         
         /**
          * Sets the Game ActorType for this Object to Actor mapping.
          *
          * @param type the Game Actor Type
          */
         void SetActorType(dtDAL::ActorType& type)
         {
            mActorType = &type;
         }
         
         /** 
          * Sets the HLA Object Type Name for this Object to Actor mapping.
          *
          * @param objTypeName the HLA Object Type Name
          */
         void SetObjectTypeName(const std::string& objTypeName)
         {
            mObjectTypeName = objTypeName;
         }
         
         /**
          * Sets the HLA Object Class Handle for this Object to Actor mapping.
          *
          * @param objClassHandle the HLA Object Class Handle
          */
         void SetObjectClassHandle(RTI::ObjectClassHandle objClassHandle)
         {
            mObjectClassHandle = objClassHandle;
         }
         
         /** 
          * Sets the Object DIS ID for this Object to Actor mapping.
          *
          * @param objectDisID the DIS ID
          */
         void SetDisID(const EntityType* thisDisID)
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
         
         /**
          * Sets the One to One Mapping vector for this Object to Actor Mapping.
          *
          * &param thisOneToOneMapping the OnetoOneMapping vector
          */
         void SetOneToOneMappingVector(std::vector<AttributeToProperty> &thisOneToOneMapping)
         {
            mOneToOne = thisOneToOneMapping;
         }
         
         ObjectToActor& operator=(const ObjectToActor& setTo)
         {
            mActorType = setTo.mActorType;
            mObjectTypeName = setTo.mObjectTypeName;
            mObjectClassHandle = setTo.mObjectClassHandle;
            mObjectDisID = setTo.mObjectDisID;
            mDISIDSet = setTo.mDISIDSet;
            if (mDISIDSet)
               mObjectDisID = setTo.mObjectDisID;
            mOneToOne = setTo.mOneToOne;
            
            return *this;
         }
         
         bool operator==(const ObjectToActor& toCompare) const
         {
            return mActorType == toCompare.mActorType &&
            mObjectTypeName == toCompare.mObjectTypeName && 
            mObjectDisID == toCompare.mObjectDisID &&
            mDISIDSet == toCompare.mDISIDSet &&
            mOneToOne == toCompare.mOneToOne &&
            (!mDISIDSet ||
               mObjectDisID == toCompare.mObjectDisID);
         }
         
         bool operator!=(const ObjectToActor& toCompare) const
         {
            return !operator==(toCompare);
         }
      private:
         
         /** 
          * Destructor.
          */
         ~ObjectToActor()
         {}
         
         /// The Actor Type for this Object to Actor mapping.
          
         dtCore::RefPtr <dtDAL::ActorType> mActorType;
         
         /// true if this mapping should only be used for objects simulated remotely, i.e. in HLA.
         bool mRemoteOnly;
         
         /// The HLA Object Type Name for this Object to Actor mapping.
         std::string mObjectTypeName;
         
         /// The HLA Object Class Handle for this Object to Actor mapping.
         RTI::ObjectClassHandle mObjectClassHandle;
         
         /// The Object DIS ID for this Object to Actor mapping.
         EntityType mObjectDisID;
         bool mDISIDSet;
         
         /// A vector of One to One mappings for this Object to Actor mapping.
         std::vector<AttributeToProperty> mOneToOne;
         
   };
   
};

#endif // DELTA_OBJECT_TO_ACTOR
