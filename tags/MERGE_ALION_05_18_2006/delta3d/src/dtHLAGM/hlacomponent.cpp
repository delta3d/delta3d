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
 * @author David Guthrie
 */

#include <plib/ul.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>
#include <dtGame/machineinfo.h>
#include <dtCore/system.h>
#include <dtUtil/matrixutil.h>
#include <dtUtil/log.h>
#include <dtGame/message.h>

#include <osg/Vec3d>
#include <osg/Endian>
#include <osg/io_utils>
#include <osgDB/FileUtils>
#include "dtHLAGM/hlacomponent.h"

#include "dtHLAGM/objecttoactor.h"
#include "dtHLAGM/distypes.h"
#include "dtHLAGM/attributetoproperty.h"
#include "dtHLAGM/parametertoparameter.h"
#include "dtHLAGM/interactiontomessage.h"

#if defined(__APPLE__)
#include <sys/socket.h>
#include <netinet/in.h>
#elif !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include <sys/socket.h>
#include <linux/in.h>
#endif

/**
 * Flags nodes as entities, which should not be included in the ground clamping
 * intersection test.
 */
const osg::Node::NodeMask entityMask = 0x01;

namespace dtHLAGM
{
   //IMPLEMENT_MANAGEMENT_LAYER(HLAComponent);
  
   HLAComponent::HLAComponent()
      : mRTIAmbassador(NULL),
        mLocalIPAddress(0x7f000001)
   {
      
      mLogger = &dtUtil::Log::GetInstance("hlafomtranslator.cpp");
     //      mCoordinates.SetGeoOrigin(0, 0, 0);
      
      mSiteIdentifier = (unsigned short)(1 + (rand() % 65535));
      mApplicationIdentifier = (unsigned short)(1 + (rand() % 65535)); 
                 
      mMachineInfo = new dtGame::MachineInfo;
   }
  
   HLAComponent::~HLAComponent()
      throw (RTI::FederateInternalError)
   {
      //RemoveSender(dtCore::System::Instance());
    
      //DeregisterInstance(this);
      LeaveFederationExecution();
   }
  
  
   /**
    * Creates/joins a federation execution.
    *
    * @param executionName the name of the federation execution to join
    * @param fedFilename the fed filename
    * @param federateName the name of this federate
    */
   void HLAComponent::JoinFederationExecution(std::string executionName,
                                                  std::string fedFilename,
                                                  std::string federateName)
   {
      SOCKET some_socket = socket(AF_INET, SOCK_DGRAM, 0);
      //
      // Code from http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1047083789&id=1045780608
      //
      int len;
      sockaddr_in other, me;
      
      memset(&other, 0, sizeof(other));
      
      other.sin_family = AF_INET;
      other.sin_port = 8192;
      
      #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
      other.sin_addr.S_un.S_addr = 0x7F000001;
      #else
      other.sin_addr.s_addr = 0x7F000001;
      #endif
      
      if(connect(some_socket, (sockaddr*)&other, sizeof(other)) == 0)
      {
         if(getsockname(some_socket, (sockaddr*)&me, (socklen_t*)&len) == 0)
         { 
            #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
            mLocalIPAddress = me.sin_addr.S_un.S_addr;
            #else
            mLocalIPAddress = me.sin_addr.s_addr;
            #endif
            
            if(ulIsLittleEndian)
            {
               ulEndianSwap(&mLocalIPAddress);
            }
            
            #if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
            mSiteIdentifier = me.sin_addr.S_un.S_un_w.s_w1;
            #else
            win_addr temp_addr;
            memcpy(&temp_addr,&me.sin_addr,sizeof(win_addr));
            mSiteIdentifier = temp_addr.S_un.S_un_w.s_w1;
            #endif
         }
      }
      
      mRTIAmbassador = new RTI::RTIambassador();
      
      try
      {
         mRTIAmbassador->createFederationExecution(executionName.c_str(),
                                                  osgDB::findDataFile(fedFilename).c_str());
      }
      catch(RTI::FederationExecutionAlreadyExists feae)
      {}
   
      mEntityIdentifierCounter = 1;
      mEventIdentifierCounter = 1;
      
      mRTIAmbassador->joinFederationExecution(federateName.c_str(), 
                                             executionName.c_str(), this);
      
      mExecutionName = executionName;

      std::set<std::string> thisObjectClassHandleMap;
      
      std::map<dtCore::RefPtr<dtDAL::ActorType>, dtCore::RefPtr<ObjectToActor> >::iterator actorToObjectIterator
        = mActorToObjectMap.begin();
      
      while (actorToObjectIterator != mActorToObjectMap.end())
      {
         dtCore::RefPtr<ObjectToActor> thisObjectToActor = actorToObjectIterator->second;
        
         std::string thisObjectClassString = thisObjectToActor->GetObjectTypeName();

         RTI::ObjectClassHandle thisObjectClassHandle(0);

         try {
            thisObjectClassHandle = mRTIAmbassador->getObjectClassHandle(thisObjectClassString.c_str());
         }
         catch (RTI::NameNotFound &){
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Could not find Object Class Name: %s", thisObjectClassString.c_str());
         }
         catch (RTI::FederateNotExecutionMember &){
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Federate not Execution Member");
         }
         catch (RTI::ConcurrentAccessAttempted &){
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Concurrent Access Attempted");
         }
         catch (RTI::RTIinternalError &){
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "RTIinternal Error");
         }
         if (thisObjectClassString != "")
         { 
            thisObjectToActor->SetObjectClassHandle(thisObjectClassHandle);
            
            std::vector<AttributeToProperty>& thisAttributeToPropertyVector = thisObjectToActor->GetOneToOneMappingVector();

            RTI::AttributeHandleSet* ahs = 
               RTI::AttributeHandleSetFactory::create(thisAttributeToPropertyVector.size());

            std::vector<AttributeToProperty>::iterator attributeToPropertyIterator = thisAttributeToPropertyVector.begin();

            mEntityIdentifierAttributeHandle = 
               mRTIAmbassador->getAttributeHandle("EntityIdentifier", thisObjectClassHandle);
            ahs->add(mEntityIdentifierAttributeHandle);

            while (attributeToPropertyIterator != thisAttributeToPropertyVector.end())
            {
               AttributeToProperty& thisAttributeToProperty = *attributeToPropertyIterator;

               std::string thisAttributeHandleString = thisAttributeToProperty.GetHLAName();

               if (!(thisAttributeHandleString.empty()))
               {

                  RTI::AttributeHandle thisAttributeHandle = mRTIAmbassador->getAttributeHandle(thisAttributeHandleString.c_str(), 
                                                                                               thisObjectClassHandle);

                  thisAttributeToProperty.SetAttributeHandle(thisAttributeHandle);

                  if (!ahs->isMember(thisAttributeHandle))
                     ahs->add(thisAttributeHandle);
               }

               ++attributeToPropertyIterator;
            }
            std::set<std::string>::iterator thisObjectClassHandleIterator = thisObjectClassHandleMap.find(thisObjectClassString);

            if (thisObjectClassHandleIterator == thisObjectClassHandleMap.end())
            {
               thisObjectClassHandleMap.insert(std::set<std::string>::value_type(thisObjectClassString));

               try {
                  mRTIAmbassador->subscribeObjectClassAttributes(thisObjectClassHandle,
                                                                *ahs);
               }
               catch (RTI::Exception &){
                 //Do something here if you wish.
               }
               try {
                  mRTIAmbassador->publishObjectClass(thisObjectClassHandle,
                                                    *ahs);
               }
               catch (RTI::Exception &){
                 //Do something here if you wish.
               }
            
               delete ahs;
            }
         }
         ++ actorToObjectIterator;
      }

      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::iterator messageToInteractionIterator
        = mMessageToInteractionMap.begin();

      while (messageToInteractionIterator != mMessageToInteractionMap.end())
      {
         dtCore::RefPtr<InteractionToMessage> thisInteractionToMessage = messageToInteractionIterator->second;
         const std::string& thisInteractionClassString = thisInteractionToMessage->GetInteractionName();
         RTI::InteractionClassHandle thisInteractionClassHandle = mRTIAmbassador->getInteractionClassHandle(thisInteractionClassString.c_str());
         thisInteractionToMessage->SetInteractionClassHandle(thisInteractionClassHandle);
         std::vector<ParameterToParameter>& thisParameterToParameterVector = thisInteractionToMessage->GetOneToOneMappingVector();
         std::vector<ParameterToParameter>::iterator parameterToParameterIterator = thisParameterToParameterVector.begin();

         while (parameterToParameterIterator != thisParameterToParameterVector.end())
         {
           ParameterToParameter& thisParameterToParameter = *parameterToParameterIterator;
           const std::string& thisParameterHandleString = thisParameterToParameter.GetHLAName();
           RTI::ParameterHandle thisParameterHandle = mRTIAmbassador->getParameterHandle(thisParameterHandleString.c_str(), thisInteractionClassHandle);
           thisParameterToParameter.SetParameterHandle(thisParameterHandle);
           ++parameterToParameterIterator;
         }

         mRTIAmbassador->publishInteractionClass(thisInteractionClassHandle);
         mRTIAmbassador->subscribeInteractionClass(thisInteractionClassHandle);

         ++messageToInteractionIterator;
      }
   }
  
   /**
    * Leaves/destroys the joined execution.
    */
   void HLAComponent::LeaveFederationExecution()  //this is kind of broken
   {
      if (!mExecutionName.empty())
      {
         try
         {
            mRTIAmbassador->resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
         }
         catch(RTI::RTIinternalError ine)
         {}
         
         try
         {
            mRTIAmbassador->destroyFederationExecution(mExecutionName.c_str());
         }
         catch(RTI::FederatesCurrentlyJoined fcj)
         {
            //std::cout<<"Problem DestroyingFed: " << fcj <<std::endl;
         }
         
         mExecutionName.clear();
      }
      if (mRTIAmbassador != NULL)
         delete mRTIAmbassador;
   }
    
   /**
    * Sets the DIS/RPR-FOM site identifier.
    *
    * @param siteIdentifier the new site identifier
    */
   void HLAComponent::SetSiteIdentifier(unsigned short siteIdentifier)
   {
      mSiteIdentifier = siteIdentifier;
   }
  
   /**
    * Returns the DIS/RPR-FOM site identifier.
    *
    * @return the site identifier
    */
   unsigned short HLAComponent::GetSiteIdentifier() const
   {
      return mSiteIdentifier;
   }
  
   /**
    * Sets the DIS/RPR-FOM application identifier.
    *
    * @param applicationIdentifier the new application identifier
    */
   void HLAComponent::SetApplicationIdentifier(unsigned short applicationIdentifier)
   {
      mApplicationIdentifier = applicationIdentifier;
   }
  
   /**
    * Returns the DIS/RPR-FOM application identifier.
    *
    * @return the DIS/RPR-FOM application identifier
    */
   unsigned short HLAComponent::GetApplicationIdentifier() const
   {
      return mApplicationIdentifier;
   }
  
   /**
    * Invoked by the RTI ambassador to request that the federate provide
    * updated attribute values for the specified object.
    *
    * @param theObject the handle of the object of interest
    * @param theAttributes the set of attributes to update
    */
   void HLAComponent::provideAttributeValueUpdate(RTI::ObjectHandle theObject,
                                                      const RTI::AttributeHandleSet& theAttributes)
      throw (RTI::ObjectNotKnown,
             RTI::AttributeNotKnown,
             RTI::AttributeNotOwned,
             RTI::FederateInternalError)
   {
   }
  
   /**
    * Invoked by the RTI ambassador to notify the federate of updated object
    * attribute values.
    *
    * @param theObject the handle of the modified object
    * @param theAttributes the new attribute values
    * @param theTime the event timestamp
    * @param theTag the user-supplied tag associated with the event
    * @param theHandle the event retraction handle
    */
   void HLAComponent::reflectAttributeValues(RTI::ObjectHandle theObject,
                                                 const RTI::AttributeHandleValuePairSet& theAttributes,
                                                 const RTI::FedTime& theTime,
                                                 const char *theTag,
                                                 RTI::EventRetractionHandle theHandle)
      throw (RTI::ObjectNotKnown,
             RTI::AttributeNotKnown,
             RTI::FederateOwnsAttributes,
             RTI::InvalidFederationTime,
             RTI::FederateInternalError)        
   {
      reflectAttributeValues(theObject, theAttributes, theTag);
   }
  
   /**
    * Invoked by the RTI ambassador to notify the federate of a deleted object
    * instance.
    *
    * @param theObject the handle of the removed object
    * @param theTime the event timestamp
    * @param theTag the user-supplied tag associated with the event
    * @param theHandle the event retraction handle
    */
   void HLAComponent::removeObjectInstance(RTI::ObjectHandle theObject,
                                               const RTI::FedTime& theTime,
                                               const char *theTag,
                                               RTI::EventRetractionHandle theHandle)
      throw (RTI::ObjectNotKnown,
             RTI::InvalidFederationTime,
             RTI::FederateInternalError)        
   {
      removeObjectInstance(theObject, theTag);
   }
  
   /**
    * Invoked by the RTI ambassador to notify the federate of a removed object
    * instance.
    *
    * @param theObject the handle of the removed object
    * @param theTag the user-supplied tag associated with the event
    */
   void HLAComponent::removeObjectInstance(RTI::ObjectHandle theObject,
                                               const char *theTag)
      throw (RTI::ObjectNotKnown,
             RTI::FederateInternalError)        
   {
      std::map<RTI::ObjectHandle, dtCore::UniqueId>::iterator hlaToActorIterator;
      
      if ((hlaToActorIterator = mHLAtoActorMap.find(theObject))
          != mHLAtoActorMap.end())
      {
         dtCore::UniqueId actorID = hlaToActorIterator->second;
         
         mHLAtoActorMap.erase(theObject);
         mActorToHLAMap.erase(actorID);
         mUniqueIdToEntityIdentifierMap.erase(actorID);

         DeleteActor(actorID);
      }
   }

   void HLAComponent::DeleteActor(const dtCore::UniqueId& toDelete)
   {
      dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_DELETED);
      msg->SetSource(*mMachineInfo);
      msg->SetAboutActorId(toDelete);
      GetGameManager()->ProcessMessage(*msg);
   }

   const ObjectToActor* HLAComponent::GetActorMapping(dtDAL::ActorType &type) const
   {
      std::map<dtCore::RefPtr<dtDAL::ActorType>, dtCore::RefPtr<ObjectToActor> >::const_iterator actorToObjectIterator;
      dtCore::RefPtr<dtDAL::ActorType> refActorType = &type;
      const ObjectToActor* thisObjectToActor = NULL;

      actorToObjectIterator = mActorToObjectMap.find(refActorType);
      if (actorToObjectIterator != mActorToObjectMap.end())
      {
         thisObjectToActor = actorToObjectIterator->second.get();
      }
      return thisObjectToActor;
   }
   
   const ObjectToActor* HLAComponent::GetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID) const
   {
      std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::const_iterator objectToActorIterator;
      const ObjectToActor* thisObjectToActor = NULL;
      objectToActorIterator = mObjectToActorMap.find(objTypeName);
      if (objectToActorIterator != mObjectToActorMap.end())
      {
         if (thisDisID != NULL)
         {
            //find the one with the right DIS ID if it exists.
            while (objectToActorIterator !=  mObjectToActorMap.end() &&
               objectToActorIterator->second->GetObjectTypeName() == objTypeName)
            {
               if (objectToActorIterator->second->GetDisID() != NULL &&
                  *objectToActorIterator->second->GetDisID() == *thisDisID)
                  break;
                  
               ++objectToActorIterator;
            }
            
            if (objectToActorIterator->second->GetDisID() == NULL)
               return NULL;
               
            if (*objectToActorIterator->second->GetDisID() != *thisDisID)
               return NULL;
         }
            
         thisObjectToActor = objectToActorIterator->second.get();
      }
      return thisObjectToActor;
   }
 
   void HLAComponent::RegisterActorMapping(dtDAL::ActorType &type, 
                                               const std::string& objTypeName, 
                                               const EntityType* thisDisID, 
                                               std::vector<AttributeToProperty> &oneToOneActorVector,
                                               bool remoteOnly)
   {
      dtCore::RefPtr<ObjectToActor> thisActorMapping = new ObjectToActor;
      
      thisActorMapping->SetActorType(type);
      thisActorMapping->SetObjectTypeName(objTypeName);
      thisActorMapping->SetDisID(thisDisID);
      thisActorMapping->SetOneToOneMappingVector(oneToOneActorVector);
      thisActorMapping->SetRemoteOnly(remoteOnly);
      
      dtCore::RefPtr<dtDAL::ActorType> refActorType = &type;

      RegisterActorMapping(*thisActorMapping);
      
   }
  
   void HLAComponent::RegisterActorMapping(ObjectToActor& objectToActor)
   {
      if (!objectToActor.IsRemoteOnly())
      {
         if (!mActorToObjectMap.insert(std::make_pair(&objectToActor.GetActorType(), &objectToActor)).second)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to mapping for actor type %s.%s and classname %s.  A mapping already exists.  Forcing mapping to be remote only.",
               objectToActor.GetActorType().GetCategory().c_str(),
               objectToActor.GetActorType().GetName().c_str(),
               objectToActor.GetObjectTypeName().c_str());
            objectToActor.SetRemoteOnly(true);
         }
         
      }
      mObjectToActorMap.insert(std::make_pair(objectToActor.GetObjectTypeName(), &objectToActor));
   }
  
   void HLAComponent::UnregisterActorMapping(dtDAL::ActorType &type)
   {    
      dtCore::RefPtr<ObjectToActor> otoa =  InternalUnregisterActorMapping(type);
      if (otoa.valid())
         InternalUnregisterObjectMapping(otoa->GetObjectTypeName(), otoa->GetDisID());
   }
  
   void HLAComponent::UnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisDisID)
   {
      dtCore::RefPtr<ObjectToActor> otoa =  InternalUnregisterObjectMapping(objTypeName, thisDisID);
      if (otoa.valid() && !otoa->IsRemoteOnly())
         InternalUnregisterActorMapping(otoa->GetActorType());
   }
   
   dtCore::RefPtr<ObjectToActor> HLAComponent::InternalUnregisterActorMapping(dtDAL::ActorType &type)
   {
      std::map<dtCore::RefPtr<dtDAL::ActorType>, dtCore::RefPtr<ObjectToActor> >::iterator actorToObjectIterator;
      dtCore::RefPtr<ObjectToActor> thisObjectToActor;
      
      actorToObjectIterator = mActorToObjectMap.find(&type);
      if (actorToObjectIterator != mActorToObjectMap.end())
      {
         thisObjectToActor = actorToObjectIterator->second;
                           
         mActorToObjectMap.erase(actorToObjectIterator);

      }
      return thisObjectToActor;
   }
   
   dtCore::RefPtr<ObjectToActor> HLAComponent::InternalUnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisDisID)
   {
      std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::iterator objectToActorIterator;
      dtCore::RefPtr<ObjectToActor> thisObjectToActor;
      
      objectToActorIterator = mObjectToActorMap.find(objTypeName);
      if (objectToActorIterator != mObjectToActorMap.end())
      {
         if (thisDisID != NULL)
         {
            //find the one with the right DIS ID if it exists.
            while (objectToActorIterator !=  mObjectToActorMap.end() &&
               objectToActorIterator->second->GetObjectTypeName() == objTypeName)
            {
               if (objectToActorIterator->second->GetDisID() != NULL &&
                  *objectToActorIterator->second->GetDisID() == *thisDisID)
                  break;
                  
               ++objectToActorIterator;
            }
            
            if (objectToActorIterator->second->GetDisID() == NULL)
               return NULL;
               
            if (*objectToActorIterator->second->GetDisID() != *thisDisID)
               return NULL;
         }
         thisObjectToActor = objectToActorIterator->second;
         
         mObjectToActorMap.erase(objectToActorIterator);
      }
      return thisObjectToActor;
   }
  
   const InteractionToMessage* HLAComponent::GetMessageMapping(const dtGame::MessageType &type) const
   {
      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::const_iterator messageToInteractionIterator;
      const InteractionToMessage* thisInteractionToMessage = NULL;
      
      if ((messageToInteractionIterator = mMessageToInteractionMap.find(&type)) != (mMessageToInteractionMap.end()))
      {
         thisInteractionToMessage = messageToInteractionIterator->second.get();         
      }
      return thisInteractionToMessage;
   }

   const InteractionToMessage* HLAComponent::GetInteractionMapping(const std::string& interName) const
   {
      std::map<std::string, dtCore::RefPtr<InteractionToMessage> >::const_iterator interactionToMessageIterator;
      const InteractionToMessage* thisInteractionToMessage = NULL;
      
      interactionToMessageIterator = mInteractionToMessageMap.find(interName);
      if (interactionToMessageIterator != mInteractionToMessageMap.end())
      {
         thisInteractionToMessage = interactionToMessageIterator->second.get();
      }
      return thisInteractionToMessage;
   }
   
   void HLAComponent::RegisterMessageMapping(InteractionToMessage& interactionToMessage)
   {
      mMessageToInteractionMap.insert(std::make_pair(&interactionToMessage.GetMessageType(), &interactionToMessage));
      
      mInteractionToMessageMap.insert(std::make_pair(interactionToMessage.GetInteractionName(), &interactionToMessage));
   }

   void HLAComponent::RegisterMessageMapping(const dtGame::MessageType &type,
                                                 const std::string& interactionTypeName,
                                                 std::vector<ParameterToParameter> &oneToOneMessageVector)
   {
      dtCore::RefPtr<InteractionToMessage> thisMessageMapping = new InteractionToMessage;
      
      thisMessageMapping->SetMessageType(type);
      thisMessageMapping->SetInteractionName(interactionTypeName);
      thisMessageMapping->SetOneToOneMappingVector(oneToOneMessageVector);

      RegisterMessageMapping(*thisMessageMapping);      
   }
  
   void HLAComponent::UnregisterMessageMapping(const dtGame::MessageType &type)
   {
      dtCore::RefPtr<InteractionToMessage> itom = InternalUnregisterMessageMapping(type);
      if (itom.valid())
         InternalUnregisterInteractionMapping(itom->GetInteractionName());
   }
  
   void HLAComponent::UnregisterInteractionMapping(const std::string& interName)
   {
      dtCore::RefPtr<InteractionToMessage> itom = InternalUnregisterInteractionMapping(interName);
      if (itom.valid())
         InternalUnregisterInteractionMapping(itom->GetInteractionName());
   }

   dtCore::RefPtr<InteractionToMessage> HLAComponent::InternalUnregisterMessageMapping(const dtGame::MessageType &type)
   {
      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::iterator messageToInteractionIterator;
      dtCore::RefPtr<InteractionToMessage> thisInteractionToMessage;
      
      messageToInteractionIterator = mMessageToInteractionMap.find(&type);
      if (messageToInteractionIterator != mMessageToInteractionMap.end())
      {
         thisInteractionToMessage = messageToInteractionIterator->second;
         
         mMessageToInteractionMap.erase(messageToInteractionIterator);
      }
      return thisInteractionToMessage;
   }

   dtCore::RefPtr<InteractionToMessage> HLAComponent::InternalUnregisterInteractionMapping(const std::string& interName)
   {
      std::map<std::string, dtCore::RefPtr<InteractionToMessage> >::iterator interactionToMessageIterator;
      dtCore::RefPtr<InteractionToMessage> thisInteractionToMessage;
      
      interactionToMessageIterator = mInteractionToMessageMap.find(interName);
      if (interactionToMessageIterator != mInteractionToMessageMap.end())
      {
         thisInteractionToMessage = interactionToMessageIterator->second;
         
         mInteractionToMessageMap.erase(interactionToMessageIterator);
      }
      return thisInteractionToMessage;
   }
  
   void HLAComponent::RegisterMessageTranslator()
   {
     
   }
  
   void HLAComponent::UnregisterMessageTranslator()
   {
     
   }
  
   void HLAComponent::RegisterInteractionTranslator()
   {
     
   }
  
   void HLAComponent::UnregisterInteractionTranslator()
   {
     
   }
  
   /**
    * Called to remove all current mappings from instance of HLAComponent class
    */
   void HLAComponent::ClearConfiguration()
   {
      mActorToObjectMap.clear();
      mObjectToActorMap.clear();
      mMessageToInteractionMap.clear();
      mInteractionToMessageMap.clear();
   } 
   
   void HLAComponent::MapHLAFieldToMessageParameter(const OneToOneMapping& mapping, const char* buffer, dtGame::MessageParameter& parameter)
   {
      const AttributeType& hlaType = mapping.GetHLAType();
      
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
            
      if (hlaType == AttributeType::WORLD_COORDINATE_TYPE)
      {
         WorldCoordinate wc;
         wc.Decode(buffer);
   
         osg::Vec3d inPos(wc.GetX(), wc.GetY(), wc.GetZ());
         osg::Vec3 position = mCoordinates.ConvertToLocalTranslation(inPos);

         if (parameterDataType == dtDAL::DataType::VEC3)
         {
            static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(position);
         }
      }
      else if (hlaType == AttributeType::EULER_ANGLES_TYPE)
      {
         EulerAngles eulerAngles;
         
         eulerAngles.Decode(buffer);
         
         //std::cout << eulerAngles.GetPsi() << " " << eulerAngles.GetTheta() << " " << eulerAngles.GetPhi() << " " << std::endl;
         osg::Vec3 result = mCoordinates.ConvertToLocalRotation(eulerAngles.GetPsi(), eulerAngles.GetTheta(), eulerAngles.GetPhi());
                  
         if (parameterDataType == dtDAL::DataType::VEC3)
         {
            //convert to x,y,z
            osg::Vec3 thisEulerAngle(result[1], result[2], result[0]);
            
            static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(thisEulerAngle);
         }
         else if (parameterDataType == dtDAL::DataType::VEC3D)
         {
            //convert to x,y,z
            osg::Vec3d thisEulerAngle(result[1], result[2], result[0]);
            
            static_cast<dtGame::Vec3dMessageParameter&>(parameter).SetValue(thisEulerAngle);
         }
      }
      else if(hlaType == AttributeType::VELOCITY_VECTOR_TYPE)
      {
         // USED FOR ANGULAR VELOCITY, ACCELERATION VECTOR, AND VELOCITY VECTOR
         VelocityVector velocityVector;
         velocityVector.Decode(buffer);
     
         if (parameterDataType == dtDAL::DataType::VEC3)
         {
            osg::Vec3 thisVector;
                        
            thisVector[0] = velocityVector.GetX();
            thisVector[1] = velocityVector.GetY();
            thisVector[2] = velocityVector.GetZ();

            thisVector =  mCoordinates.GetOriginRotationMatrix().preMult(thisVector);
            
            static_cast<dtGame::Vec3MessageParameter&>(parameter).SetValue(thisVector);
         }
      }
      else if (hlaType == AttributeType::UNSIGNED_INT_TYPE)
      {
         unsigned int value = *(unsigned int*)(&buffer[0]);
   
         if (osg::getCpuByteOrder() == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&value), sizeof(unsigned int));
         }
        
         SetIntegerValue((long)value, parameter, mapping);
        
      }
      else if (hlaType == AttributeType::UNSIGNED_SHORT_TYPE)
      {
         unsigned short value = *(unsigned short*)(&buffer[0]);
         
         if (osg::getCpuByteOrder() == osg::LittleEndian)
           osg::swapBytes((char*)(&value), sizeof(unsigned short));

         SetIntegerValue((long)value, parameter, mapping);
      }
      else if (hlaType == AttributeType::UNSIGNED_CHAR_TYPE)
      {
         unsigned char value = *(unsigned char*)(&buffer[0]);         
         SetIntegerValue((long)value, parameter, mapping);
      }
      else if (hlaType == AttributeType::EVENT_IDENTIFIER_TYPE)
      {
         EventIdentifier eventIdentifier;
         eventIdentifier.Decode(buffer);
         //if (
      }
      else if (hlaType == AttributeType::ENTITY_TYPE)
      {
         EntityType entityType;
         entityType.Decode(buffer);
      }
      else if (hlaType == AttributeType::MARKING_TYPE)
      {
         if (parameterDataType == dtDAL::DataType::STRING)
         {
            //unsigned char markingEnum = *(unsigned char*)(&buffer[0]);
            std::string markingText;
            //1 is ASCII
            //if (markingEnum == 1)
            //{
            for (int i=0; i < 11; ++i)
            {
               char c = buffer[i + 1];
               if (c == '\0')
                  break;
               markingText.append(1, c);
            }            
            //}
            static_cast<dtGame::StringMessageParameter&>(parameter).SetValue(markingText);
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Unable to map HLA type \"%s\" to \"%s\"", AttributeType::MARKING_TYPE.GetName().c_str(),
               parameterDataType.GetName().c_str());            
         }         
      }
      else
      {
         //NOT A RECOGNIZED ATTRIBUTE DATA TYPE.
      }
   }
   
   void HLAComponent::SetIntegerValue(long value, dtGame::MessageParameter& parameter, const OneToOneMapping& mapping)
   {
      const dtDAL::DataType& parameterDataType = parameter.GetDataType();
      
      //std::cout << mapping.GetHLAName() << " " << value << std::endl;
      
      if (parameterDataType == dtDAL::DataType::UINT)
      {
         static_cast<dtGame::UnsignedIntMessageParameter&>(parameter).SetValue(unsigned(value));
      }
      else if (parameterDataType == dtDAL::DataType::ULONGINT)
      {
         static_cast<dtGame::UnsignedLongIntMessageParameter&>(parameter).SetValue((unsigned long)(value));
      }
      else if (parameterDataType == dtDAL::DataType::USHORTINT)
      {
         static_cast<dtGame::UnsignedShortIntMessageParameter&>(parameter).SetValue((unsigned short)(value));
      }
      else if (parameterDataType == dtDAL::DataType::INT)
      {
         static_cast<dtGame::IntMessageParameter&>(parameter).SetValue(int(value));
      }
      else if (parameterDataType == dtDAL::DataType::LONGINT)
      {
         static_cast<dtGame::LongIntMessageParameter&>(parameter).SetValue(long(value));
      }
      else if (parameterDataType == dtDAL::DataType::SHORTINT)
      {
         static_cast<dtGame::ShortIntMessageParameter&>(parameter).SetValue(short(value));
      }
      else if (parameterDataType == dtDAL::DataType::BOOLEAN)
      {
         static_cast<dtGame::BooleanMessageParameter&>(parameter).SetValue(value != 0);
      }
      else if (parameterDataType == dtDAL::DataType::ENUMERATION)
      {
         std::string sValue;
         if (mapping.GetGameEnumerationValue(int(value), sValue))
            static_cast<dtGame::EnumMessageParameter&>(parameter).SetValue(sValue);
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "No mapping was specified for value \"%d\" for an enumeration on mapping of \"%s\" to \"%s\". " 
                  "Using default value \"%s\".", (int)value, mapping.GetGameName().c_str(), 
                  mapping.GetHLAName().c_str(), mapping.GetDefaultValue().c_str());
            
            static_cast<dtGame::EnumMessageParameter&>(parameter).SetValue(mapping.GetDefaultValue());
            
         }
      }    
   }
  
   dtGame::MessageParameter* HLAComponent::FindOrAddMessageParameter(const std::string& name, const dtDAL::DataType& type, dtGame::Message& msg)
   {
      //first check to see if the named parameter is one of the default parameters
      //on an actor update message.
      dtCore::RefPtr<dtGame::MessageParameter> propertyParameter = msg.GetParameter(name);
      
      if (propertyParameter == NULL)   
      {
         //Check to see if an update parameter already exists.
         propertyParameter = static_cast<dtGame::ActorUpdateMessage&>(msg).GetUpdateParameter(name);                           
         if (propertyParameter == NULL)
         {
            try 
            {
               propertyParameter = static_cast<dtGame::ActorUpdateMessage&>(msg).AddUpdateParameter(name, type);                           
            }
            catch (const dtUtil::Exception& ex)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Exception thrown attempting to create an update parameter of type \"%s\" with name \"%s\".", 
                  type.GetName().c_str(), name.c_str());
               ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
            }
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Attempting to map a second object attribute to update parameter \"%s\". Mapping will be ignored.", name.c_str());
            
            propertyParameter = NULL;
            
         }
      }
      
      if (propertyParameter != NULL && propertyParameter->GetDataType() != type)
      {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Parameter named \"%s\" should be mapping to type \"%s\" but it really has type \"%s\".  The mapping is being ignored.", 
               name.c_str(), type.GetName().c_str(), propertyParameter->GetDataType().GetName().c_str());
            
            propertyParameter = NULL;
      }
      
      return propertyParameter.get();
   }
   
   /**
    * Invoked by the RTI ambassador to notify the federate of updated object
    * attribute values.
    *
    * @param theObject the handle of the modified object
    * @param theAttributes the new attribute values
    * @param theTag the user-supplied tag associated with the event
    */
   void HLAComponent::reflectAttributeValues(RTI::ObjectHandle theObject,
                                                 const RTI::AttributeHandleValuePairSet& theAttributes,
                                                 const char *theTag)
      throw (RTI::ObjectNotKnown,
             RTI::AttributeNotKnown,
             RTI::FederateOwnsAttributes,
             RTI::FederateInternalError)
   {
      try
      {
         dtCore::RefPtr<ObjectToActor> bestObjectToActor;
         dtCore::UniqueId currentEntityID;
         bool bNewObject = false;
    
         std::map<RTI::ObjectHandle, dtCore::UniqueId>::iterator mHLAtoActorMapIterator;
         
         if ((mHLAtoActorMapIterator = mHLAtoActorMap.find(theObject)) 
             != mHLAtoActorMap.end())
         {
            currentEntityID = mHLAtoActorMapIterator->second;
   
            std::map<RTI::ObjectHandle, dtCore::RefPtr<ObjectToActor> >::iterator mObjectHandleToClassMapIterator;
   
            if ((mObjectHandleToClassMapIterator = mObjectHandleToClassMap.find(theObject))
                != mObjectHandleToClassMap.end())
              bestObjectToActor = mObjectHandleToClassMapIterator->second;
            else 
              bestObjectToActor = GetBestObjectToActor(theObject, theAttributes);
         }
         else
         {
            mHLAtoActorMap.insert(std::map<RTI::ObjectHandle, dtCore::UniqueId>
                                  ::value_type(theObject, currentEntityID));
            mActorToHLAMap.insert(std::map<dtCore::UniqueId, RTI::ObjectHandle>
                                  ::value_type(currentEntityID, theObject));
            bNewObject = true;
   
            bestObjectToActor = GetBestObjectToActor(theObject, theAttributes);
         }
         
         if (bestObjectToActor != NULL)
         {
            //USE OBJECTTOACTOR TO CREATE ACTOR UPDATE
            std::vector<AttributeToProperty> currentAttributeToPropertyVector = bestObjectToActor->GetOneToOneMappingVector();
           
            std::vector<AttributeToProperty>::iterator vectorIterator;
         
            dtCore::RefPtr<dtGame::GameManager> gameManager = GetGameManager();
         
            dtGame::MessageFactory& factory = gameManager->GetMessageFactory();
         
            dtCore::RefPtr<dtGame::Message> msg;
            if (bNewObject)
               msg = factory.CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED);
            else
               msg = factory.CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
         
            for (vectorIterator = currentAttributeToPropertyVector.begin();
                 vectorIterator != currentAttributeToPropertyVector.end();
                 vectorIterator++)
            {
               if (vectorIterator->IsInvalid()) continue;
               
               const std::string& propertyString = vectorIterator->GetGameName();
               const std::string& attributeString = vectorIterator->GetHLAName();
   
               bool matched = false;
               for(unsigned int i=0;i<theAttributes.size();i++)
               {
                  RTI::AttributeHandle handle = theAttributes.getHandle(i);
               
                  if (handle == vectorIterator->GetAttributeHandle())
                  {
                     matched = true;
                     unsigned long length;
                     char* buf = theAttributes.getValuePointer(i, length);
   
                     if (!(propertyString.empty()) && !(attributeString.empty()))
                     {                    
                        const dtDAL::DataType& propertyDataType = vectorIterator->GetGameType();
                        
                        dtCore::RefPtr<dtGame::MessageParameter> propertyParameter = FindOrAddMessageParameter(propertyString, propertyDataType, *msg);
                         
                        if (propertyParameter != NULL)
                        {
                           MapHLAFieldToMessageParameter(*vectorIterator, buf, *propertyParameter);
                        }
                        else
                        {
                           //this is logged in the call to find the parameter.
                           vectorIterator->SetInvalid(true);
                        }
                        
                     }
                  }
               }
               //if this mapping is not in the HLA message
               if (!matched)
               {
                  if (bNewObject || vectorIterator->IsRequiredForGame())
                  {
                     const std::string& defaultValue = vectorIterator->GetDefaultValue();
                     if (!defaultValue.empty() && !propertyString.empty())
                     {
                        const dtDAL::DataType& propertyDataType = vectorIterator->GetGameType();
       
                        dtCore::RefPtr<dtGame::MessageParameter> propertyParameter = FindOrAddMessageParameter(propertyString, propertyDataType, *msg);
                         
                        if (propertyParameter != NULL)
                        {
                           propertyParameter->FromString(defaultValue);
                        }
                        else
                        {
                           //this is logged in the call to find the parameter.
                           vectorIterator->SetInvalid(true);
                        }

                     }
                  }
               }
            }
   
            dtGame::ActorUpdateMessage& auMsg = static_cast<dtGame::ActorUpdateMessage&>(*msg);
   
            auMsg.SetActorTypeName(bestObjectToActor->GetActorType().GetName());
            auMsg.SetActorTypeCategory(bestObjectToActor->GetActorType().GetCategory());
   
            auMsg.SetAboutActorId(currentEntityID);
            auMsg.SetSource(*mMachineInfo);
            
            gameManager->ProcessMessage(*msg);
         }
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }
   }
  
   ObjectToActor* HLAComponent::GetBestObjectToActor(RTI::ObjectHandle theObject, 
                                                              const RTI::AttributeHandleValuePairSet& theAttributes)
   {
      int bestRank = -1;
      ObjectToActor* bestObjectToActor = NULL;
      
      RTI::ObjectClassHandle classHandle = mRTIAmbassador->getObjectClass(theObject);
      
      std::string classHandleString = mRTIAmbassador->getObjectClassName(classHandle);
      
      bool foundType = false;
      EntityType currentEntityType;
      
      for(unsigned int i=0;i<theAttributes.size();i++)
      {
         RTI::AttributeHandle handle = theAttributes.getHandle(i);
       
         if (std::string(mRTIAmbassador->getAttributeName(handle, classHandle)) == "EntityType")
         {
            unsigned long length;
            char* buf = theAttributes.getValuePointer(i, length);
            
            foundType = true;
            currentEntityType.Decode(buf);
            
            std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::iterator objectToActorIterator;
            
            for (objectToActorIterator = mObjectToActorMap.find(classHandleString); objectToActorIterator != mObjectToActorMap.end();
                 objectToActorIterator++)
            {
               ObjectToActor& thisObjectToActor = *objectToActorIterator->second;
               //TODO check for nulls
               int thisRank = currentEntityType.RankMatch(*thisObjectToActor.GetDisID());
               if (thisRank > bestRank)
               {
                  bestRank = thisRank;
                  bestObjectToActor = &thisObjectToActor;
               }
            }

            break;
         }
      }
      if (bestObjectToActor != NULL)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Creating actor of type %s.%s for DIS ID [%d %d %d %d %d %d %d].",
               bestObjectToActor->GetActorType().GetCategory().c_str(),
               bestObjectToActor->GetActorType().GetName().c_str(),
               (int)currentEntityType.GetKind(), (int)currentEntityType.GetDomain(), (int)currentEntityType.GetCountry(), 
               (int)currentEntityType.GetCategory(), (int)currentEntityType.GetSubcategory(), (int)currentEntityType.GetSpecific(), 
               (int)currentEntityType.GetExtra());
         }

         mObjectHandleToClassMap.insert(std::map<RTI::ObjectHandle, dtCore::RefPtr<ObjectToActor> >
                                        ::value_type(theObject, bestObjectToActor));
      }
      else
      {
         if (foundType)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Ignoring entity update with DIS ID [%d %d %d %d %d %d %d].",
               (int)currentEntityType.GetKind(), (int)currentEntityType.GetDomain(), (int)currentEntityType.GetCountry(), 
               (int)currentEntityType.GetCategory(), (int)currentEntityType.GetSubcategory(), (int)currentEntityType.GetSpecific(), 
               (int)currentEntityType.GetExtra());
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Ignoring entity update with no entity type.");
         }

      }

      
      return bestObjectToActor;
   }


   /**
    * Invoked by the RTI ambassador to notify the federate of a new object
    * instance.
    *
    * @param theObject the handle of the discovered object
    * @param theObjectClass the handle of the discovered object's class
    * @param theObjectName the name of the discovered object
    */
   void HLAComponent::discoverObjectInstance(RTI::ObjectHandle theObject,
                                                 RTI::ObjectClassHandle theObjectClassHandle,
                                                 const char* theObjectName)
      throw (RTI::CouldNotDiscover,
             RTI::ObjectClassNotKnown,
             RTI::FederateInternalError)
   {
     /*      //Create a new Unique ID
      dtCore::UniqueId actorID;
      
      //ADD to HmHLAtoActorMap, mActorToHLAMap
      mHLAtoActorMap.insert(std::map<RTI::ObjectHandle, dtCore::UniqueId>
      ::value_type(theObject, actorID));
      
      mActorToHLAMap.insert(std::map<dtCore::UniqueId, RTI::ObjectHandle>
      ::value_type(actorID, theObject));
      
      //Create INFO_ACTOR_CREATED msg.
      dtCore::RefPtr<dtGame::GameManager> gameManager = GetGameManager();
      
      dtGame::MessageFactory& factory = gameManager->GetMessageFactory();
      
      dtCore::RefPtr<dtGame::Message> msg = factory.CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED);
      
      //Add Unique ID to Message, and send.
      
      //Does this really need to be done???
      */
   }

   /**
    * Invoked by the RTI ambassador to notify the federate of a received
    * interaction.
    *
    * @param theInteraction the handle of the received interaction
    * @param theParameters the parameters of the interaction
    * @param theTag the user-supplied tag associated with the event
    */
   void HLAComponent::receiveInteraction(RTI::InteractionClassHandle theInteraction,
                                             const RTI::ParameterHandleValuePairSet& theParameters,
                                             const char *theTag)
      throw (RTI::InteractionClassNotKnown,
             RTI::InteractionParameterNotKnown,
             RTI::FederateInternalError)
   {
      try
      {
         //Compare RTI InteractionClassHandle to InteractionToMessageMap Class Handle
         std::string classHandleString = mRTIAmbassador->getInteractionClassName(theInteraction);
         
         std::map<std::string, dtCore::RefPtr<InteractionToMessage> >::iterator interactionToMessageIterator;
         interactionToMessageIterator = mInteractionToMessageMap.find(classHandleString);
         
         dtCore::RefPtr<InteractionToMessage> thisInteractionToMessage = interactionToMessageIterator->second;
         
         const dtGame::MessageType& messageType = thisInteractionToMessage->GetMessageType();
         dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(messageType);
   
         std::vector<ParameterToParameter> currentParameterToParameterVector = thisInteractionToMessage->GetOneToOneMappingVector();      
         std::vector<ParameterToParameter>::iterator vectorIterator;
         
         for (vectorIterator = currentParameterToParameterVector.begin();
              vectorIterator != currentParameterToParameterVector.end();
              vectorIterator++)
         {
            for (unsigned int i=0; i<theParameters.size(); i++)
            {
               RTI::ParameterHandle handle = theParameters.getHandle(i);
               if (handle == vectorIterator->GetParameterHandle())
               {
                  unsigned long length;
                  char* buf = theParameters.getValuePointer(i, length);
                  const dtDAL::DataType& gameParameterDataType = vectorIterator->GetGameType();
                                 
                  const std::string& gameParameterName = vectorIterator->GetGameName();
                  dtGame::MessageParameter* messageParameter = message->GetParameter(gameParameterName);                
                  
                  if (messageParameter == NULL)
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, 
                        "No message parameter named %s found in messageType %s when trying to map from HLA interaction %s",
                        gameParameterName.c_str(), 
                        messageType.GetName().c_str(),
                        classHandleString.c_str());
                     continue;
                  }
                  else if (gameParameterDataType != messageParameter->GetDataType())
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__, 
                        "Message parameter named %s found in messageType %s has unexpected type %s.  %s was expected.  Attempting to set anyway.",
                        gameParameterName.c_str(), 
                        messageType.GetName().c_str(),
                        messageParameter->GetDataType().GetName().c_str(),
                        gameParameterDataType.GetName().c_str());
                     continue;
                  }
                  
                  MapHLAFieldToMessageParameter(*vectorIterator, buf, *messageParameter);
                  
               }
            }
         }
         
         message->SetSource(*mMachineInfo);         
         GetGameManager()->ProcessMessage(*message);
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }
      
   }

   void HLAComponent::SendMessage(const dtGame::Message& message)
   {
      if (mExecutionName != "")
      {
         if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
         {
            //Delete HLA Object, and all Mappings referencing the Actor or the Object.
            const dtCore::UniqueId actorID = message.GetAboutActorId();
            std::map<dtCore::UniqueId, RTI::ObjectHandle>::iterator actorToHLAiterator;
            
            if ((actorToHLAiterator = mActorToHLAMap.find(actorID))
                != mActorToHLAMap.end())
            {
               RTI::ObjectHandle thisObjectHandle = actorToHLAiterator->second;
               
               mActorToHLAMap.erase(actorID);
               mHLAtoActorMap.erase(thisObjectHandle);
               mUniqueIdToEntityIdentifierMap.erase(actorID);
               
               mRTIAmbassador->deleteObjectInstance(thisObjectHandle, "");
            }
         }
         else if ((message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_CREATED)
                  || (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_UPDATED))
         {
            const dtCore::UniqueId actorID = message.GetAboutActorId();
            
            const std::string actorName = actorID.ToString();
            
            const char* objectName = actorName.c_str();
            
            unsigned short thisEntityId;
            
            //Check to see if this object has been created in HLA.  
            std::map<dtCore::UniqueId, unsigned short>::iterator entityIdentifierIterator;
	    
            if ((entityIdentifierIterator = mUniqueIdToEntityIdentifierMap.find(actorID)) 
                != mUniqueIdToEntityIdentifierMap.end())
            {
               thisEntityId = entityIdentifierIterator->second;
            }
            else
            {
               //If not, add entityIdentifier
               thisEntityId = mEntityIdentifierCounter;
               mEntityIdentifierCounter++;
               
               mUniqueIdToEntityIdentifierMap.insert(std::map<dtCore::UniqueId, unsigned short>
                                                     ::value_type(actorID, thisEntityId));
            }
	    
            //Get Actor Type
            dtGame::GameActorProxy* ga = GetGameManager()->FindGameActorById(actorID);
            
            dtCore::RefPtr<dtDAL::ActorType> actorType = &ga->GetActorType();
            
            //Lookup ObjectToActor from mActorToObjectMap
            std::map<dtCore::RefPtr<dtDAL::ActorType>, dtCore::RefPtr<ObjectToActor> >::iterator actorToObjectIterator;
            dtCore::RefPtr<ObjectToActor> thisObjectToActor = new ObjectToActor;
            
            if ((actorToObjectIterator = mActorToObjectMap.find(actorType)) != (mActorToObjectMap.end()))
            {
               thisObjectToActor = actorToObjectIterator->second;
            }
            //Get ClassHandle from ObjectToActor
            RTI::ObjectClassHandle classHandle = thisObjectToActor->GetObjectClassHandle();
            
            RTI::ObjectHandle objectHandle;
            //IF objectHandle has not been created...
            std::map<dtCore::UniqueId, RTI::ObjectHandle>::iterator uniqueIdToObjectHandleIterator;
            
            if ((uniqueIdToObjectHandleIterator = mActorToHLAMap.find(actorID))
                != mActorToHLAMap.end())
            {
               objectHandle = uniqueIdToObjectHandleIterator->second;
            }
            else
            {
               //Pass ClassHandle to registerObjectInstance
               objectHandle = mRTIAmbassador->registerObjectInstance(classHandle,
                                                                    objectName);
               mActorToHLAMap.insert(std::map<dtCore::UniqueId, RTI::ObjectHandle>
                                     ::value_type(actorID, objectHandle));
               
               mHLAtoActorMap.insert(std::map<RTI::ObjectHandle, dtCore::UniqueId>
                                     ::value_type(objectHandle, actorID));
            }
            
            //Create AttributeHandleValuePairSet
            RTI::AttributeHandleValuePairSet* theAttributes = 
              RTI::AttributeSetFactory::create(15);
            
            theAttributes->empty();
            
            //Make a loop that looks at all parameter to attribute mappings inside of ObjectToActor
            std::vector<AttributeToProperty> attributeToPropertyVector = thisObjectToActor->GetOneToOneMappingVector();
            
            std::vector<AttributeToProperty>::iterator vectorIterator;
	    
            for (vectorIterator = attributeToPropertyVector.begin();
                 vectorIterator != attributeToPropertyVector.end();
                 vectorIterator++)
            {
               //Check if Parameter returns NULL
               std::string gameName = vectorIterator->GetGameName();
	       
               dtCore::RefPtr<const dtGame::MessageParameter> messageParameter = message.GetParameter(gameName);
               
               if (messageParameter != NULL)
               {
                  const dtDAL::DataType& gameDataType = vectorIterator->GetGameType();
                  const AttributeType& hlaType = vectorIterator->GetHLAType();
                  
                  if (gameDataType == dtDAL::DataType::ENUMERATION)
                    //This is probably not the way to do it.  Probably just get the DIS Enumeration from
                    // the objectToActor mapping.
                  {
                     if (hlaType == AttributeType::ENTITY_TYPE)
                     {
                        EntityType entityType;
                        
                        std::string valueString = static_cast<const dtGame::EnumMessageParameter*>(messageParameter.get())->GetValue();
                        
                        char encodedEntityType[8];
                        
                        strcpy(encodedEntityType, valueString.c_str());
                        
                        entityType.SetKind(encodedEntityType[0]);
                        entityType.SetDomain(encodedEntityType[1]);
                        entityType.SetCountry(*(unsigned short *)(&encodedEntityType[2]));
                        entityType.SetCategory(encodedEntityType[4]);
                        entityType.SetSubcategory(encodedEntityType[5]);
                        entityType.SetSpecific(encodedEntityType[6]);
                        entityType.SetExtra(encodedEntityType[7]);
                        
                        entityType.Encode(encodedEntityType);
                        
                        theAttributes->add(vectorIterator->GetAttributeHandle(),
                                           encodedEntityType, 8);
                     }
                     else if (hlaType == AttributeType::UNSIGNED_INT_TYPE)
                     {
                        char encodedDamageState[4];
                        std::string valueString = static_cast<const dtGame::EnumMessageParameter*>(messageParameter.get())->GetValue();
                        
                        *((int*)encodedDamageState) = atoi(valueString.c_str());
                        
                        if(ulIsLittleEndian)
                        {
                           ulEndianSwap((int*)encodedDamageState);
                        }
                        
                        theAttributes->add(vectorIterator->GetAttributeHandle(),
                                           encodedDamageState, 4);
                     }
                  }
                  else if (gameDataType == dtDAL::DataType::VEC3)
                  {
                     if (hlaType == AttributeType::EULER_ANGLES_TYPE)
                     {
                        EulerAngles eulerAngles;
                        char encodedOrientation[12];
                        
                        osg::Vec3f vec = static_cast<const dtGame::Vec3fMessageParameter*>(messageParameter.get())->GetValue();
                        
                        eulerAngles.SetPsi(vec[0]);
                        eulerAngles.SetTheta(vec[1]);
                        eulerAngles.SetPhi(vec[2]);
                        
                        eulerAngles.Encode(encodedOrientation);
                        
                        theAttributes->add(vectorIterator->GetAttributeHandle(),
                                           encodedOrientation,
                                           12);
                     }
                     else if (hlaType == AttributeType::VELOCITY_VECTOR_TYPE)
                     {
                        VelocityVector velocityVector;
                        char encodedVelocityVector[12];
                        
                        osg::Vec3f vec = static_cast<const dtGame::Vec3fMessageParameter*>(messageParameter.get())->GetValue();
                        
                        velocityVector.SetX(vec[0]);
                        velocityVector.SetY(vec[1]);
                        velocityVector.SetZ(vec[2]);
                        
                        velocityVector.Encode(encodedVelocityVector);
                        
                        theAttributes->add(vectorIterator->GetAttributeHandle(),
                                           encodedVelocityVector,
                                           12);
                     }
                  }
                  else if (gameDataType == dtDAL::DataType::VEC3)
                  {
                     osg::Vec3d vec = static_cast<const dtGame::Vec3dMessageParameter*>(messageParameter.get())->GetValue();
                     
                     WorldCoordinate worldLocation;
                     char encodedWorldLocation[24];
                     worldLocation.SetX(vec[0]);
                     worldLocation.SetY(vec[1]);
                     worldLocation.SetZ(vec[2]);
                     
                     worldLocation.Encode(encodedWorldLocation);
                     
                     theAttributes->add(vectorIterator->GetAttributeHandle(),
                                        encodedWorldLocation,
                                        24);
                  }
               }
            }
            
            //Since there is not mapping for Entity Identifier, add this attribute manually.
            EntityIdentifier thisEntityIdentifier;
            thisEntityIdentifier.SetSiteIdentifier(mSiteIdentifier);
            thisEntityIdentifier.SetApplicationIdentifier(mApplicationIdentifier);
            thisEntityIdentifier.SetEntityIdentifier(thisEntityId);
            
            char encodedEntityIdentifier[6];
            
            thisEntityIdentifier.Encode(encodedEntityIdentifier);
            
            theAttributes->add(mEntityIdentifierAttributeHandle,
                               encodedEntityIdentifier,
                               6);
            
            if(theAttributes->size() > 0)
            {
               mRTIAmbassador->updateAttributeValues(objectHandle,
                                                    *theAttributes,
                                                    "");
            }
            delete theAttributes;
	    
            mRTIAmbassador->tick();     
         }
      }
      /*	osg::Vec3 vec;
                
      osg::Matrix mat;
      
      char 	  encodedDeadReckoningAlgorithm[1],
      encodedForceIdentifier[1],
      encodedMarking[12],
      encodedFirePowerDisabled[1],
      encodedImmobilized[1],
      encodedCamouflageType[4],
      encodedIsConcealed[1],
      encodedTrailingEffectsCode[4],
      encodedPowerPlantOn[1];
      
      if(mObjectsToUpdate.count((*m).first) > 0)
      {
      encodedDeadReckoningAlgorithm[0] = 2; // DRM(F, P, W)
      
      theAttributes->add(mDeadReckoningAlgorithmAttributeHandle,
      encodedDeadReckoningAlgorithm, 1);
      
      *((char*)encodedForceIdentifier) = 0; // Other
      
      theAttributes->add(mForceIdentifierAttributeHandle,
      encodedForceIdentifier, 1);
      
      encodedMarking[0] = 1; // ASCII
      
      memset(encodedMarking + 1, 0, 11);
      
      theAttributes->add(mMarkingAttributeHandle,
      encodedMarking, 12);
      
      *((char*)encodedFirePowerDisabled) = 0; // False
      
      theAttributes->add(mFirePowerDisabledAttributeHandle,
      encodedFirePowerDisabled, 1);
      
      *((char*)encodedImmobilized) = 0; // False
      
      theAttributes->add(mImmobilizedAttributeHandle,
      encodedImmobilized, 1);
      
      *((int*)encodedCamouflageType) = 0; // None
      
      theAttributes->add(mCamouflageTypeAttributeHandle,
      encodedCamouflageType, 4);
      
      *((char*)encodedIsConcealed) = 0; // False
      
      theAttributes->add(mIsConcealedAttributeHandle,
      encodedIsConcealed, 1);
      
      *((int*)encodedTrailingEffectsCode) = 0; // No trail
      
      theAttributes->add(mTrailingEffectsCodeAttributeHandle,
      encodedTrailingEffectsCode, 4);
      
      *((char*)encodedPowerPlantOn) = 0; // Whatever
      
      theAttributes->add(mPowerPlantOnAttributeHandle,
      encodedPowerPlantOn, 1);
      }
      
      if(theAttributes->size() > 0)
      {
      mRTIAmbassador->updateAttributeValues((*m).first,
      *theAttributes,
      "");
      }
      }
      
      mObjectsToUpdate.clear();
      
      delete theAttributes;
      
      mRTIAmbassador->tick();     
      
      // Request types of newly discovered objects
      
      RTI::AttributeHandleSet* requiredAttributes =
      RTI::AttributeHandleSetFactory::create(2);
      
      requiredAttributes->add(mEntityIdentifierAttributeHandle);
      requiredAttributes->add(mEntityTypeAttributeHandle);
      
      //	for(std::set<RTI::ObjectHandle>::iterator objs =
      //    mNewlyDiscoveredObjects.begin();
      //  objs != mNewlyDiscoveredObjects.end(); objs++)
      //	{
      // mRTIAmbassador->requestObjectAttributeValueUpdate(*objs,
      //						    *requiredAttributes);
      //	}
      
      //	mNewlyDiscoveredObjects.clear();
      
      delete requiredAttributes;
      }    
      } */
   }
   
   void HLAComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
      {
         if (mRTIAmbassador != NULL)
            mRTIAmbassador->tick();
      }
   }

   void HLAComponent::SetOriginLocation(const osg::Vec3d& location)
   {
      mCoordinates.SetOriginLocation(location.x(), location.y(), location.z());
   }

   const osg::Vec3d HLAComponent::GetOriginLocation()
   {
      osg::Vec3d location;
      mCoordinates.GetOriginLocation(location.x(), location.y(), location.z());
      return location;
   }

   void HLAComponent::SetGeoOrigin(double lat, double lon, double elevation)
   {
      mCoordinates.SetGeoOrigin(lat, lon, elevation);
   }

   void HLAComponent::SetOriginRotation(const osg::Vec3& rotation)
   {
      mCoordinates.SetOriginRotation(rotation[0], rotation[1], rotation[2]);
   }
  
   const osg::Vec3 HLAComponent::GetOriginRotation() const
   {
      osg::Vec3 rotation;
      mCoordinates.GetOriginRotation(rotation.x(), rotation.y(), rotation.z());
      return rotation;
   }
}

