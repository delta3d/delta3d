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

#include <dtUtil/matrixutil.h>
#include <dtUtil/log.h>

#include <dtCore/uniqueid.h>

#include <dtDAL/actortype.h>

#include <dtGame/message.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>
#include <dtGame/machineinfo.h>

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
#include "dtHLAGM/parametertranslator.h"
#include "dtHLAGM/rprparametertranslator.h"

#if defined(__APPLE__)
#include <sys/socket.h>
#include <netinet/in.h>
#elif !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include <sys/socket.h>
#include <linux/in.h>
#endif

#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
typedef unsigned int SOCKET;

typedef struct win_addr {
   union {
      struct {
         unsigned char s_b1,s_b2,s_b3,s_b4;
      } S_un_b;
      struct {
         unsigned short s_w1,s_w2;
      } S_un_w;
      unsigned long S_addr;
   } S_un;
} win_addr;

#else
typedef int socklen_t;
#endif

/**
 * Flags nodes as entities, which should not be included in the ground clamping
 * intersection test.
 */
const osg::Node::NodeMask entityMask = 0x01;

namespace dtHLAGM
{

   const std::string HLAComponent::ABOUT_ACTOR_ID("aboutActorId");
   const std::string HLAComponent::SENDING_ACTOR_ID("sendingActorId");

   //IMPLEMENT_MANAGEMENT_LAYER(HLAComponent);

   HLAComponent::HLAComponent(const std::string& name)
      : GMComponent(name), mRTIAmbassador(NULL),
        mLocalIPAddress(0x7f000001),
        mMachineInfo(new dtGame::MachineInfo)
   {

      mLogger = &dtUtil::Log::GetInstance("hlacomponent.cpp");

      mSiteIdentifier = (unsigned short)(1 + (rand() % 65535));
      mApplicationIdentifier = (unsigned short)(1 + (rand() % 65535));
      mParameterTranslators.push_back(new RPRParameterTranslator(mCoordinates, mRuntimeMappings));
   }

   HLAComponent::~HLAComponent()
      throw (RTI::FederateInternalError)
   {
      //DeregisterInstance(this);
      LeaveFederationExecution();
   }


   void HLAComponent::RegisterObjectToActorWithRTI(ObjectToActor& objectToActor)
   {
      const std::string& thisObjectClassString = objectToActor.GetObjectClassName();
      RTI::ObjectClassHandle thisObjectClassHandle(0);

      try
      {
         thisObjectClassHandle = mRTIAmbassador->getObjectClassHandle(thisObjectClassString.c_str());
      }
      catch (const RTI::NameNotFound &)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                              "Could not find Object Class Name: %s", thisObjectClassString.c_str());
         return;
      }
      catch (const RTI::FederateNotExecutionMember &)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                              "Federate not Execution Member");
         return;
      }
      catch (const RTI::ConcurrentAccessAttempted &)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                              "Concurrent Access Attempted");
         return;
      }
      catch (const RTI::RTIinternalError &)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                              "RTIinternal Error");
         return;
      }

      objectToActor.SetObjectClassHandle(thisObjectClassHandle);
      std::vector<AttributeToPropertyList>& thisAttributeToPropertyListVector = objectToActor.GetOneToManyMappingVector();

      RTI::AttributeHandleSet* ahs =
            RTI::AttributeHandleSetFactory::create(thisAttributeToPropertyListVector.size());

      std::vector<AttributeToPropertyList>::iterator attributeToPropertyListIterator = thisAttributeToPropertyListVector.begin();

      //if we have a name for the entity id attribute, subscribe to it and save the handle.
      if (!objectToActor.GetEntityIdAttributeName().empty())
      {
         RTI::AttributeHandle entityIdentifierAttributeHandle =
               mRTIAmbassador->getAttributeHandle(objectToActor.GetEntityIdAttributeName().c_str(), thisObjectClassHandle);

         objectToActor.SetEntityIdAttributeHandle(entityIdentifierAttributeHandle);

         ahs->add(entityIdentifierAttributeHandle);
      }

      //if we have a name for the entity type attribute, subscribe to it and save the handle.
      if (objectToActor.GetDisID() != NULL)
      {
         RTI::AttributeHandle disIDAttributeHandle =
               mRTIAmbassador->getAttributeHandle("EntityType", thisObjectClassHandle);

         objectToActor.SetDisIDAttributeHandle(disIDAttributeHandle);

         ahs->add(disIDAttributeHandle);
      }

      while (attributeToPropertyListIterator != thisAttributeToPropertyListVector.end())
      {
         AttributeToPropertyList& thisAttributeToPropertyList = *attributeToPropertyListIterator;
         std::string thisAttributeHandleString = thisAttributeToPropertyList.GetHLAName();

         if (!(thisAttributeHandleString.empty()))
         {
            RTI::AttributeHandle thisAttributeHandle = mRTIAmbassador->getAttributeHandle(thisAttributeHandleString.c_str(),
                  thisObjectClassHandle);
            thisAttributeToPropertyList.SetAttributeHandle(thisAttributeHandle);

            if (!ahs->isMember(thisAttributeHandle))
               ahs->add(thisAttributeHandle);
         }

         ++attributeToPropertyListIterator;
      }

      bool subscribed = false;
      try
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Subscribing to object class %s.", thisObjectClassString.c_str());

         mRTIAmbassador->subscribeObjectClassAttributes(thisObjectClassHandle, *ahs);
         subscribed = true;

         if (!objectToActor.IsRemoteOnly())
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Publishing object class %s.", thisObjectClassString.c_str());

            mRTIAmbassador->publishObjectClass(thisObjectClassHandle, *ahs);
         }
      }
      catch (const RTI::Exception &)
      {
         if (!subscribed)
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Error subscribing to object class %s.", thisObjectClassString.c_str());
         else
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Error publishing object class %s.", thisObjectClassString.c_str());
      }

      delete ahs;
   }

   void HLAComponent::RegisterInteractionToMessageWithRTI(InteractionToMessage& interactionToMessage)
   {
      const std::string& thisInteractionClassString = interactionToMessage.GetInteractionName();
      RTI::InteractionClassHandle thisInteractionClassHandle
            = mRTIAmbassador->getInteractionClassHandle(thisInteractionClassString.c_str());
      interactionToMessage.SetInteractionClassHandle(thisInteractionClassHandle);
      std::vector<ParameterToParameterList>& thisParameterToParameterListVector = interactionToMessage.GetOneToManyMappingVector();
      std::vector<ParameterToParameterList>::iterator parameterToParameterIterator = thisParameterToParameterListVector.begin();

      while (parameterToParameterIterator != thisParameterToParameterListVector.end())
      {
         ParameterToParameterList& thisParameterToParameterList = *parameterToParameterIterator;
         const std::string& thisParameterHandleString = thisParameterToParameterList.GetHLAName();
         RTI::ParameterHandle thisParameterHandle = mRTIAmbassador->getParameterHandle(thisParameterHandleString.c_str(), thisInteractionClassHandle);
         thisParameterToParameterList.SetParameterHandle(thisParameterHandle);
         ++parameterToParameterIterator;
      }

      bool subscribed = false;
      try
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Subscribing to Interaction class %s.", thisInteractionClassString.c_str());

         mRTIAmbassador->subscribeInteractionClass(thisInteractionClassHandle);

         subscribed = true;

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Publishing to Interaction class %s.", thisInteractionClassString.c_str());

         mRTIAmbassador->publishInteractionClass(thisInteractionClassHandle);
      }
      catch (const RTI::Exception &)
      {
         if (!subscribed)
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "Error subscribing to interaction class %s.", thisInteractionClassString.c_str());
         else
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "Error publishing interaction class %s.", thisInteractionClassString.c_str());
      }
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

            if(osg::getCpuByteOrder() == osg::LittleEndian)
            {
               osg::swapBytes((char*)&mLocalIPAddress, sizeof(mLocalIPAddress));
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
         std::string fedFile = osgDB::findDataFile(fedFilename).c_str();
         mRTIAmbassador->createFederationExecution(executionName.c_str(), fedFile.c_str());
      }
      catch(RTI::FederationExecutionAlreadyExists&)
      {
         //okay, this is fine, but it's odd that this would happen.
      }

      mEntityIdentifierCounter = 1;
      mEventIdentifierCounter = 1;

      mRTIAmbassador->joinFederationExecution(federateName.c_str(),
                                              executionName.c_str(), this);

      mExecutionName = executionName;

      std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::iterator objectToActorIterator
        = mObjectToActorMap.begin();

      while (objectToActorIterator != mObjectToActorMap.end())
      {
         ObjectToActor& thisObjectToActor = *(objectToActorIterator->second);
         const std::string& thisObjectClassString = thisObjectToActor.GetObjectClassName();

         if (!thisObjectClassString.empty())
            RegisterObjectToActorWithRTI(thisObjectToActor);

         ++ objectToActorIterator;
      }

      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::iterator messageToInteractionIterator
        = mMessageToInteractionMap.begin();

      while (messageToInteractionIterator != mMessageToInteractionMap.end())
      {
         dtCore::RefPtr<InteractionToMessage> thisInteractionToMessage = messageToInteractionIterator->second;
         const std::string& thisInteractionClassString = thisInteractionToMessage->GetInteractionName();

         if (!thisInteractionClassString.empty())
            RegisterInteractionToMessageWithRTI(*thisInteractionToMessage);

         ++messageToInteractionIterator;
      }
   }

   /**
    * Leaves/destroys the joined execution.
    */
   void HLAComponent::LeaveFederationExecution()  //this is kind of broken
   {
      //drop all instance mapping data.
      mRuntimeMappings.Clear();

      if (!mExecutionName.empty())
      {
         try
         {
            mRTIAmbassador->resignFederationExecution(RTI::DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES);
         }
         catch(RTI::RTIinternalError&)
         {
            //okay since we already resigned
         }

         try
         {
            mRTIAmbassador->destroyFederationExecution(mExecutionName.c_str());
         }
         catch(RTI::FederatesCurrentlyJoined&)
         {
            //std::cout<<"Problem DestroyingFed: " << fcj <<std::endl;
         }

         mExecutionName.clear();
      }
      if (mRTIAmbassador != NULL)
         delete mRTIAmbassador;

      mRTIAmbassador = NULL;

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
    * Returns the DIS/RPR-FOM site idntifier.
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

   void HLAComponent::removeObjectInstance(RTI::ObjectHandle theObject,
                                               const char *theTag)
      throw (RTI::ObjectNotKnown,
             RTI::FederateInternalError)
   {
      std::map<RTI::ObjectHandle, dtCore::UniqueId>::iterator hlaToActorIterator;

      const dtCore::UniqueId* actorId = mRuntimeMappings.GetId(theObject);

      if (actorId != NULL)
      {
         DeleteActor(*actorId);
         //This is subtle, but the mapping has to be removed after sending the message because
         //the actorId is deleted when it's removed from the mappings.
         mRuntimeMappings.Remove(theObject);
      }
   }

   void HLAComponent::DeleteActor(const dtCore::UniqueId& toDelete)
   {
      dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_DELETED);
      msg->SetSource(*mMachineInfo);
      msg->SetAboutActorId(toDelete);
      GetGameManager()->SendMessage(*msg);
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

   ObjectToActor* HLAComponent::GetActorMapping(dtDAL::ActorType &type)
   {
      std::map<dtCore::RefPtr<dtDAL::ActorType>, dtCore::RefPtr<ObjectToActor> >::iterator actorToObjectIterator;
      dtCore::RefPtr<dtDAL::ActorType> refActorType = &type;
      ObjectToActor* thisObjectToActor = NULL;

      actorToObjectIterator = mActorToObjectMap.find(refActorType);
      if (actorToObjectIterator != mActorToObjectMap.end())
      {
         thisObjectToActor = actorToObjectIterator->second.get();
      }

      return thisObjectToActor;
   }

   const ObjectToActor* HLAComponent::GetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID) const
   {
      return InternalGetObjectMapping(objTypeName, thisDisID);
   }

   ObjectToActor* HLAComponent::GetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID)
   {
      return const_cast<ObjectToActor*>(InternalGetObjectMapping(objTypeName, thisDisID));
   }

   const ObjectToActor* HLAComponent::InternalGetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID) const
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
               objectToActorIterator->second->GetObjectClassName() == objTypeName)
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
                                               std::vector<AttributeToPropertyList> &oneToOneActorVector,
                                               bool remoteOnly)
   {
      dtCore::RefPtr<ObjectToActor> thisActorMapping = new ObjectToActor;

      thisActorMapping->SetActorType(type);
      thisActorMapping->SetObjectClassName(objTypeName);
      thisActorMapping->SetDisID(thisDisID);
      thisActorMapping->SetOneToManyMappingVector(oneToOneActorVector);
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
               objectToActor.GetObjectClassName().c_str());
            objectToActor.SetRemoteOnly(true);
         }

      }
      mObjectToActorMap.insert(std::make_pair(objectToActor.GetObjectClassName(), &objectToActor));
   }

   void HLAComponent::UnregisterActorMapping(dtDAL::ActorType &type)
   {
      dtCore::RefPtr<ObjectToActor> otoa =  InternalUnregisterActorMapping(type);
      if (otoa.valid())
         InternalUnregisterObjectMapping(otoa->GetObjectClassName(), otoa->GetDisID());
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
               objectToActorIterator->second->GetObjectClassName() == objTypeName)
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

   InteractionToMessage* HLAComponent::GetMessageMapping(const dtGame::MessageType &type)
   {
      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::iterator messageToInteractionIterator;
      InteractionToMessage* thisInteractionToMessage = NULL;

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

   InteractionToMessage* HLAComponent::GetInteractionMapping(const std::string& interName)
   {
      std::map<std::string, dtCore::RefPtr<InteractionToMessage> >::iterator interactionToMessageIterator;
      InteractionToMessage* thisInteractionToMessage = NULL;

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
                                                 std::vector<ParameterToParameterList> &oneToOneMessageVector)
   {
      dtCore::RefPtr<InteractionToMessage> thisMessageMapping = new InteractionToMessage;

      thisMessageMapping->SetMessageType(type);
      thisMessageMapping->SetInteractionName(interactionTypeName);
      thisMessageMapping->SetOneToManyMappingVector(oneToOneMessageVector);

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

   template <typename MappingObject, typename mapType, typename mapTypeIterator>
   void GetAllMappings(std::vector<MappingObject*> toFill, mapType& readFrom, mapTypeIterator beginIt)
   {
      //Adding the beginIt parameter for the begin iterator makes the template able to figure out what type
      //it should be.

      toFill.clear();

      mapTypeIterator mappingIterator = beginIt;

      while (mappingIterator != readFrom.end())
      {
         MappingObject* thisMapping = (mappingIterator->second).get();
         toFill.push_back(thisMapping);
         ++mappingIterator;
      }
   }

   void HLAComponent::GetAllObjectToActorMappings(std::vector<ObjectToActor*> toFill)
   {
      GetAllMappings(toFill, mObjectToActorMap, mObjectToActorMap.begin());
   }

   void HLAComponent::GetAllObjectToActorMappings(std::vector<const ObjectToActor*> toFill) const
   {
      GetAllMappings(toFill, mObjectToActorMap, mObjectToActorMap.begin());
   }

   void HLAComponent::GetAllInteractionToMessageMappings(std::vector<InteractionToMessage*> toFill)
   {
      GetAllMappings(toFill, mInteractionToMessageMap, mInteractionToMessageMap.begin());
   }

   void HLAComponent::GetAllInteractionToMessageMappings(std::vector<const InteractionToMessage*> toFill) const
   {
      GetAllMappings(toFill, mInteractionToMessageMap, mInteractionToMessageMap.begin());
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

   //void HLAComponent::ReadAndMapEntityId()

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
         const dtCore::UniqueId* currentActorId;
         bool bNewObject = false;

         currentActorId = mRuntimeMappings.GetId(theObject);

         if (currentActorId == NULL)
            return;

         bool needBestMapping = false;

         bestObjectToActor = mRuntimeMappings.GetObjectToActor(theObject);

         if (!bestObjectToActor.valid())
         {
            needBestMapping = true;
            bNewObject = true;

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Preparing to Map in new Object.  The id is \"%s\".", currentActorId->ToString().c_str());
            }
         }
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "An object to actor was found for object with ID \"%s\".", currentActorId->ToString().c_str());
            }
         }

         if (needBestMapping)
         {
            bool hadEntityTypeProperty;
            bestObjectToActor = GetBestObjectToActor(theObject, theAttributes, hadEntityTypeProperty);

            if (!bestObjectToActor.valid())
            {

               //support for the useEntityId concept needs to be added.
               if (hadEntityTypeProperty) //|| !useEntityId)
               {
                  // Test clean up.  This is not a complete solution as we don't want to return
                  // an error everytime we get a partial update
                  mRuntimeMappings.Remove(*currentActorId);
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                     "Unable to map in object, no object to actor found.");

                  mRuntimeMappings.Remove(theObject);
               }
               else
               {
                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                        "No mapping was found, but the PDU had no entity type data.  Waiting for a full update.");
               }

               return;
            }
            else
            {
               mRuntimeMappings.Put(theObject, *bestObjectToActor);
            }
         }

         if (bNewObject)
         {
            for (unsigned i=0; i < theAttributes.size(); ++i)
            {
               RTI::AttributeHandle handle = theAttributes.getHandle(i);
               if (handle == bestObjectToActor->GetEntityIdAttributeHandle())
               {
                  EntityIdentifier ei;

                  unsigned long length;
                  char* buffer = theAttributes.getValuePointer(i, length);
                  if (length < (unsigned long)(ei.EncodedLength()))
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                         "Expected Entity id attribute to have length %u, but it has length %u.  Id will be ignored",
                                         ei.EncodedLength(), length);
                  }
                  else
                  {
                     ei.Decode(buffer);
                     mRuntimeMappings.Put(ei, *currentActorId);
                     if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     {
                        std::ostringstream ss;
                        ss << "The Entity Identifier value is \"" << ei << "\" and the currect actor id is \"" << currentActorId->ToString() << ".";
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                        ss.str().c_str());
                     }
                  }
               }
            }
         }

         //USE OBJECTTOACTOR TO CREATE ACTOR UPDATE
         std::vector<AttributeToPropertyList>& currentAttributeToPropertyListVector = bestObjectToActor->GetOneToManyMappingVector();

         std::vector<AttributeToPropertyList>::iterator vectorIterator;

         dtCore::RefPtr<dtGame::GameManager> gameManager = GetGameManager();

         dtGame::MessageFactory& factory = gameManager->GetMessageFactory();

         dtCore::RefPtr<dtGame::Message> msg;
         if (bNewObject)
            msg = factory.CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED);
         else
            msg = factory.CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);

         for (vectorIterator = currentAttributeToPropertyListVector.begin();
               vectorIterator != currentAttributeToPropertyListVector.end();
               vectorIterator++)
         {
            if (vectorIterator->IsInvalid()) continue;

            const std::string& propertyString = vectorIterator->GetParameterDefinitions()[0].GetGameName();
            const std::string& attributeString = vectorIterator->GetHLAName();

            bool matched = false;
            for(unsigned i=0; i < theAttributes.size(); ++i)
            {
               RTI::AttributeHandle handle = theAttributes.getHandle(i);


               if (handle == vectorIterator->GetAttributeHandle())
               {
                  matched = true;
                  unsigned long length;
                  char* buf = theAttributes.getValuePointer(i, length);

                  if (!(propertyString.empty()) && !(attributeString.empty()))
                  {
                     const dtDAL::DataType& propertyDataType = vectorIterator->GetParameterDefinitions()[0].GetGameType();

                     dtCore::RefPtr<dtGame::MessageParameter> propertyParameter;

                     //The about actor id and source actor ID are special cases.
                     //We create a dummy parameter to allow the mapper code to work, and
                     //then set it back to the message after the fact.
                     if (propertyDataType == dtDAL::DataType::ACTOR &&
                        (propertyString == ABOUT_ACTOR_ID ||
                         propertyString == SENDING_ACTOR_ID))
                     {
                        propertyParameter =
                              dtGame::MessageParameter::CreateFromType(dtDAL::DataType::ACTOR,
                                                                      "propertyString");
                     }
                     else
                     {
                        propertyParameter =
                           FindOrAddMessageParameter(propertyString, propertyDataType, *msg);
                     }

                     if (propertyParameter != NULL)
                     {
                        std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;
                        messageParameters.push_back(propertyParameter);

                        MapToMessageParameters(buf, length, messageParameters, *vectorIterator);

                        if (propertyDataType == dtDAL::DataType::ACTOR)
                        {
                           if (propertyString == ABOUT_ACTOR_ID)
                              msg->SetAboutActorId(propertyParameter->ToString());
                           else if (propertyString == SENDING_ACTOR_ID)
                              msg->SetSendingActorId(propertyParameter->ToString());
                        }
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
               if (bNewObject || vectorIterator->GetParameterDefinitions()[0].IsRequiredForGame())
               {
                  const std::string& defaultValue = vectorIterator->GetParameterDefinitions()[0].GetDefaultValue();
                  if (!defaultValue.empty() && !propertyString.empty())
                  {
                     const dtDAL::DataType& propertyDataType = vectorIterator->GetParameterDefinitions()[0].GetGameType();

                     //The actor id special cases are not supported here because people can't hard
                     //code actor id's with default values.
                     dtCore::RefPtr<dtGame::MessageParameter> propertyParameter =
                        FindOrAddMessageParameter(propertyString, propertyDataType, *msg);

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

         auMsg.SetAboutActorId(*currentActorId);
         auMsg.SetSource(*mMachineInfo);

         gameManager->SendMessage(*msg);
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }
   }

   ObjectToActor* HLAComponent::GetBestObjectToActor(RTI::ObjectHandle theObject,
      const RTI::AttributeHandleValuePairSet& theAttributes, bool& hadEntityTypeProperty)
   {
      int bestRank = -1;
      ObjectToActor* bestObjectToActor = NULL;

      RTI::ObjectClassHandle classHandle = mRTIAmbassador->getObjectClass(theObject);

      std::string classHandleString = mRTIAmbassador->getObjectClassName(classHandle);

      hadEntityTypeProperty = false;
      EntityType currentEntityType;

      for (unsigned int i=0; i < theAttributes.size(); i++)
      {
         RTI::AttributeHandle handle = theAttributes.getHandle(i);

		 std::string attribName = std::string(mRTIAmbassador->getAttributeName(handle, classHandle));
         if (attribName == "EntityType")
         {
            unsigned long length;
            char* buf = theAttributes.getValuePointer(i, length);

            hadEntityTypeProperty = true;
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
            std::ostringstream ss;
            ss << currentEntityType;
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Creating actor of type %s.%s for DIS ID [%s] and object class \"%s\".",
               bestObjectToActor->GetActorType().GetCategory().c_str(),
               bestObjectToActor->GetActorType().GetName().c_str(),
               ss.str().c_str(), classHandleString.c_str());
         }

      }
      else
      {
         if (hadEntityTypeProperty)
         {
            std::ostringstream ss;
            ss << currentEntityType;
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Ignoring entity update with DIS ID [%s] and object class \"%s\".",
               ss.str().c_str(), classHandleString.c_str());
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
      dtCore::UniqueId newId;
      mRuntimeMappings.Put(theObject, newId);
   }

   const ParameterTranslator* HLAComponent::FindTranslatorForAttributeType(const AttributeType& type) const
   {
      for (unsigned i = 0; i < mParameterTranslators.size(); ++i)
      {
         if (mParameterTranslators[i]->TranslatesAttributeType(type))
         {
            return mParameterTranslators[i].get();
         }
      }

      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
         "No parameter translator was found to mapping attribute type \"%s\"",
         type.GetName().c_str());

      return NULL;
   }

   void HLAComponent::MapToMessageParameters(const char* buffer,
                                             size_t size,
                                             std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters,
                                             const OneToManyMapping& mapping) const
   {
      const ParameterTranslator* pt = FindTranslatorForAttributeType(mapping.GetHLAType());

      if (pt != NULL)
         pt->MapToMessageParameters(buffer, size, parameters, mapping);
   }

   void HLAComponent::MapFromMessageParameters(char* buffer,
                                 size_t maxSize,
                                 std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters,
                                 const OneToManyMapping& mapping) const
   {
      const ParameterTranslator* pt = FindTranslatorForAttributeType(mapping.GetHLAType());

      if (pt != NULL)
         pt->MapFromMessageParameters(buffer, maxSize, parameters, mapping);
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
         // Compare RTI InteractionClassHandle to InteractionToMessageMap Class Handle
         std::string classHandleString = mRTIAmbassador->getInteractionClassName(theInteraction);

         std::map<std::string, dtCore::RefPtr<InteractionToMessage> >::iterator interactionToMessageIterator;
         interactionToMessageIterator = mInteractionToMessageMap.find(classHandleString);

         if (interactionToMessageIterator == mInteractionToMessageMap.end())
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "\"%s\" does not have a registered mapping.",
                                classHandleString.c_str());
            return;
         }

         dtCore::RefPtr<InteractionToMessage> thisInteractionToMessage = interactionToMessageIterator->second;

         const dtGame::MessageType& messageType = thisInteractionToMessage->GetMessageType();
         dtCore::RefPtr<dtGame::Message> message = GetGameManager()->GetMessageFactory().CreateMessage(messageType);

         std::vector<ParameterToParameterList> currentParameterToParameterListVector = thisInteractionToMessage->GetOneToManyMappingVector();
         std::vector<ParameterToParameterList>::iterator vectorIterator;

         for (vectorIterator = currentParameterToParameterListVector.begin();
              vectorIterator != currentParameterToParameterListVector.end();
              vectorIterator++)
         {
            for (unsigned int i = 0; i < theParameters.size(); i++)
            {
               RTI::ParameterHandle handle = theParameters.getHandle(i);
               if (handle == vectorIterator->GetParameterHandle())
               {
                  unsigned long length;
                  char* buf = theParameters.getValuePointer(i, length);
                  const dtDAL::DataType& gameParameterDataType = vectorIterator->GetParameterDefinitions()[0].GetGameType();

                  const std::string& gameParameterName = vectorIterator->GetParameterDefinitions()[0].GetGameName();

                  // The about actor id and source actor ID are special cases.
                  // We create a dummy parameter to allow the mapper code to work, and
                  // then set it back to the message after the fact.
                  if (gameParameterDataType == dtDAL::DataType::ACTOR &&
                     (gameParameterName == ABOUT_ACTOR_ID ||
                     gameParameterName == SENDING_ACTOR_ID))
                  {

                     std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

                     dtCore::RefPtr<dtGame::MessageParameter> messageParameter =
                           dtGame::MessageParameter::CreateFromType(dtDAL::DataType::ACTOR,
                              gameParameterName);

                     messageParameters.push_back(messageParameter);

                     MapToMessageParameters(buf, length, messageParameters, *vectorIterator);

                     if (gameParameterName == ABOUT_ACTOR_ID)
                        message->SetAboutActorId(messageParameter->ToString());
                     else
                        message->SetSendingActorId(messageParameter->ToString());
                  }
                  else
                  {
                     dtCore::RefPtr<dtGame::MessageParameter> messageParameter =
                           message->GetParameter(gameParameterName);

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

                     std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;
                     messageParameters.push_back(messageParameter);

                     MapToMessageParameters(buf, length, messageParameters, *vectorIterator);
                  }

               }
            }
         }

         message->SetSource(*mMachineInfo);
         GetGameManager()->SendMessage(*message);
      }
      catch (const RTI::Exception&)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "RTI Error receiving interation");
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }

   }

   void HLAComponent::DispatchDelete(const dtGame::Message& message)
   {
      // Delete HLA Object, and all Mappings referencing the Actor or the Object.
      const dtCore::UniqueId& actorId = message.GetAboutActorId();
      const RTI::ObjectHandle* thisObjectHandle = mRuntimeMappings.GetHandle(actorId);

      if (thisObjectHandle != NULL)
      {
         mRuntimeMappings.Remove(actorId);
         mRTIAmbassador->deleteObjectInstance(*thisObjectHandle, "");
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                             "Ignoring actor with id \"%s\" delete because it is not published in HLA",
                             actorId.ToString().c_str());
      }
   }

   void HLAComponent::DispatchUpdate(const dtGame::Message& message)
   {
      const dtCore::UniqueId actorID = message.GetAboutActorId();

      const std::string actorName = actorID.ToString();

      // Get Actor Type
      dtGame::GameActorProxy* ga = GetGameManager()->FindGameActorById(actorID);

      dtCore::RefPtr<dtDAL::ActorType> actorType = &ga->GetActorType();

      // Lookup ObjectToActor from mActorToObjectMap
      dtCore::RefPtr<ObjectToActor> thisObjectToActor;

      thisObjectToActor = GetActorMapping(*actorType);

      if (!thisObjectToActor.valid())
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Ignoring actor with type \"%s.%s\" no mapping defined for it.",
                                actorID.ToString().c_str());
         }
         return;
      }

      //Get ClassHandle from ObjectToActor
      RTI::ObjectClassHandle classHandle = thisObjectToActor->GetObjectClassHandle();

      const RTI::ObjectHandle* tmpObjectHandle = mRuntimeMappings.GetHandle(actorID);
      RTI::ObjectHandle objectHandle;

      if (tmpObjectHandle == NULL)
      {
         try
         {
            //Pass ClassHandle to registerObjectInstance
            objectHandle = mRTIAmbassador->registerObjectInstance(classHandle,
               actorName.c_str());
         }
         catch (RTI::ObjectClassNotDefined&)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Could not find Object Class Named \"%s\"",
                                 thisObjectToActor->GetObjectClassName().c_str());
            return;

         }
         catch (RTI::ObjectClassNotPublished&)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Object Class Named \"%s\" was not published.",
                                 thisObjectToActor->GetObjectClassName().c_str());

            return;
         }
         catch (RTI::RTIinternalError&)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "RTI internal exception trying to sent and update for object class: %s",
                                 thisObjectToActor->GetObjectClassName().c_str());
            return;
         }
         catch (RTI::Exception&)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "RTI Exception trying to sent and update for object class: %s",
                                 thisObjectToActor->GetObjectClassName().c_str());
            return;
         }

         mRuntimeMappings.Put(objectHandle, actorID);
      }
      else
      {
         objectHandle = *tmpObjectHandle;
      }

      //Create AttributeHandleValuePairSet to hold the attributes.
      RTI::AttributeHandleValuePairSet* theAttributes =
            RTI::AttributeSetFactory::create(thisObjectToActor->GetOneToManyMappingVector().size() + 2);

      theAttributes->empty();

      bool useEntityId = !thisObjectToActor->GetEntityIdAttributeName().empty();

      if (useEntityId)
      {
         EntityIdentifier thisEntityId;

         const EntityIdentifier* tempEntityId = mRuntimeMappings.GetEntityId(actorID);

         if (tempEntityId != NULL)
         {
            thisEntityId = *tempEntityId;

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               std::ostringstream ss;
               ss << "Using existing entity ID \"" << thisEntityId << "\" for object " << actorID << ".";
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
            }
         }
         else
         {
            //If not, add entityIdentifier
            thisEntityId.SetApplicationIdentifier(GetApplicationIdentifier());
            thisEntityId.SetSiteIdentifier(GetSiteIdentifier());
            thisEntityId.SetEntityIdentifier(mEntityIdentifierCounter);
            mEntityIdentifierCounter++;

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               std::ostringstream ss;
               ss << "Creating new entity ID \"" << thisEntityId << "\" for object " << actorID << ".";
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
            }

            mRuntimeMappings.Put(thisEntityId, actorID);
         }

         size_t bufferSize;
         char* buffer;

         ParameterTranslator::AllocateBuffer(buffer, bufferSize, RPRAttributeType::ENTITY_IDENTIFIER_TYPE);

         thisEntityId.Encode(buffer);

         theAttributes->add(thisObjectToActor->GetEntityIdAttributeHandle(),
                            buffer,
                            bufferSize);

         ParameterTranslator::DeallocateBuffer(buffer);
      }

      if (thisObjectToActor->GetDisID() != NULL)
      {
         size_t bufferSize;
         char* buffer;

         ParameterTranslator::AllocateBuffer(buffer, bufferSize, RPRAttributeType::ENTITY_TYPE);

         thisObjectToActor->GetDisID()->Encode(buffer);

         theAttributes->add(thisObjectToActor->GetDisIDAttributeHandle(),
                            buffer,
                            bufferSize);

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Sending DIS id \"" << *thisObjectToActor->GetDisID() << "\" for object " << thisObjectToActor->GetObjectClassName() << ".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         ParameterTranslator::DeallocateBuffer(buffer);
      }

      //Make a loop that looks at all parameter to attribute mappings inside of ObjectToActor
      std::vector<AttributeToPropertyList> attributeToPropertyListVector = thisObjectToActor->GetOneToManyMappingVector();

      std::vector<AttributeToPropertyList>::iterator vectorIterator;

      std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

      for (vectorIterator = attributeToPropertyListVector.begin();
           vectorIterator != attributeToPropertyListVector.end();
           vectorIterator++)
      {
         //Check if Parameter returns NULL
         const std::string& gameName = vectorIterator->GetParameterDefinitions()[0].GetGameName();

         //First check for a regular parameter.
         dtCore::RefPtr<const dtGame::MessageParameter> messageParameter = message.GetParameter(gameName);

         if (!messageParameter.valid())
         {
            //If no regular parameter was found, check for an actor update parameter.
            messageParameter = static_cast<const dtGame::ActorUpdateMessage&>(message).GetUpdateParameter(gameName);
         }

         if (messageParameter.valid())
         {
            const AttributeType& hlaType = vectorIterator->GetHLAType();

            size_t bufferSize;
            char* buffer;

            ParameterTranslator::AllocateBuffer(buffer, bufferSize, hlaType);

            try
            {
               //Supporting mapping multiple actor properties to a single object attribute
               //is in the works.  This vector always only has one item in it for now.
               messageParameters.clear();
               messageParameters.push_back(messageParameter);

               MapFromMessageParameters(buffer, bufferSize, messageParameters, *vectorIterator);
               theAttributes->add(vectorIterator->GetAttributeHandle(), buffer, bufferSize);

               ParameterTranslator::DeallocateBuffer(buffer);
            }
            catch (...)
            {
               //be sure the buffer is deleted.
               if (buffer != NULL)
                  ParameterTranslator::DeallocateBuffer(buffer);
            }

         }
      }

      if (theAttributes->size() > 0)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Sending Update for actor with id " << actorID << ".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         mRTIAmbassador->updateAttributeValues(objectHandle,
                                               *theAttributes,
                                               "");
      }

      delete theAttributes;

      mRTIAmbassador->tick();
   }

   void HLAComponent::PrepareInteraction(const dtGame::Message& message, RTI::ParameterHandleValuePairSet& interactionParams, 
      const InteractionToMessage& interactionToMessage)
   {
      interactionParams.empty();

      //The list of message parameter to interaction parameter mappings.
      std::vector<ParameterToParameterList> paramToParamListVector = interactionToMessage.GetOneToManyMappingVector();

      std::vector<ParameterToParameterList>::const_iterator paramMappingItor;

      std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

      for (paramMappingItor = paramToParamListVector.begin();
           paramMappingItor != paramToParamListVector.end();
           paramMappingItor++)
      {
         //Check if Parameter returns NULL
         messageParameters.clear();

         for (unsigned i = 0; i < paramMappingItor->GetParameterDefinitions().size(); ++i)
         {
         
            const std::string& gameParameterName = paramMappingItor->GetParameterDefinitions()[i].GetGameName();
            const dtDAL::DataType& gameParameterType = paramMappingItor->GetParameterDefinitions()[i].GetGameType();
            
            //We map with message parameters, so if a 
            if (gameParameterType == dtDAL::DataType::ACTOR &&
               gameParameterName == ABOUT_ACTOR_ID)
            {
               dtCore::RefPtr<dtGame::MessageParameter> messageParameter = 
                  dtGame::MessageParameter::CreateFromType(dtDAL::DataType::ACTOR,
                  gameParameterName);
               
               messageParameter->FromString(message.GetAboutActorId().ToString());
               messageParameters.push_back(messageParameter.get());
            
            }
            else if (gameParameterType == dtDAL::DataType::ACTOR &&
               gameParameterName == SENDING_ACTOR_ID)
            {
               dtCore::RefPtr<dtGame::MessageParameter> messageParameter = 
                  dtGame::MessageParameter::CreateFromType(dtDAL::DataType::ACTOR,
                  gameParameterName);
               
               messageParameter->FromString(message.GetSendingActorId().ToString());
               messageParameters.push_back(messageParameter.get());
            }
            else
            {
               dtCore::RefPtr<const dtGame::MessageParameter> messageParameter = message.GetParameter(gameParameterName);
               if (messageParameter.valid())
               {
                  messageParameters.push_back(messageParameter);
               }
               else
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                     "Message to interaction specifies a message parameter named \"%\", but the message of type \"%\" has no such parameter.",
                     gameParameterName.c_str(), message.GetMessageType().GetName().c_str());            
               }
            }
            
         }
         
         if (!messageParameters.empty())
         {
            const AttributeType& hlaType = paramMappingItor->GetHLAType();

            size_t bufferSize;
            char* buffer;

            ParameterTranslator::AllocateBuffer(buffer, bufferSize, hlaType);

            try
            {
               //Supporting mapping multiple message parameters to a single interaction parameter
               //is in the works.  This vector always only has one item in it for now.
               MapFromMessageParameters(buffer, bufferSize, messageParameters, *paramMappingItor);
               interactionParams.add(paramMappingItor->GetParameterHandle(), buffer, bufferSize);

               ParameterTranslator::DeallocateBuffer(buffer);
            }
            catch (...)
            {
               //be sure the buffer is deleted.
               if (buffer != NULL)
                  ParameterTranslator::DeallocateBuffer(buffer);
            }

         }
      }
   }

   void HLAComponent::DispatchInteraction(const dtGame::Message& message)
   {      
      const InteractionToMessage* interactionToMessage = GetMessageMapping(message.GetMessageType());
      if (interactionToMessage == NULL)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "No mapping to an interaction was defined for message type: ", 
               message.GetMessageType().GetName().c_str());
         }
         return;
      }

      //Create ParameterHandleValuePairSet to hold the interaction parameters.
      RTI::ParameterHandleValuePairSet* interactionParams =
            RTI::ParameterSetFactory::create(interactionToMessage->GetOneToManyMappingVector().size());
      
      PrepareInteraction(message, *interactionParams, *interactionToMessage);

      if (interactionParams->size() > 0)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Sending interaction of type \"" << interactionToMessage->GetInteractionName() << "\" for message type \"" 
               << message.GetMessageType() << "\".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         mRTIAmbassador->sendInteraction(interactionToMessage->GetInteractionClassHandle(),
                                         *interactionParams,
                                         "");
      }

      delete interactionParams;
      
   }

   void HLAComponent::DispatchNetworkMessage(const dtGame::Message& message)
   {
      if (mExecutionName != "")
      {
         if (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_DELETED)
         {
            DispatchDelete(message);
         }
         else if ((message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_CREATED)
                  || (message.GetMessageType() == dtGame::MessageType::INFO_ACTOR_UPDATED))
         {
            DispatchUpdate(message);
         }
         else
         {
            DispatchInteraction(message);
         }
      }
   }

   void HLAComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
      {
         if (mRTIAmbassador != NULL)
            mRTIAmbassador->tick();
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_LOADED)
      {
         mRuntimeMappings.Clear();
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

