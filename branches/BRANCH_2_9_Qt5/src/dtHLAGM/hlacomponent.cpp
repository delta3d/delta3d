/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006-2010, Alion Science and Technology, BMH Operation.
 * Copyright (C) 2012, David Guthrie
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

#include <sstream>

#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/parametertranslator.h>
#include <dtHLAGM/rprparametertranslator.h>
#include <dtHLAGM/ddmregioncalculator.h>
#include <dtHLAGM/ddmregiondata.h>
#include <dtHLAGM/exceptionenum.h>
#include <dtHLAGM/rtiexception.h>

#include <dtUtil/matrixutil.h>
#include <dtUtil/mswinmacros.h>
#include <dtUtil/log.h>
#include <dtUtil/coordinates.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>

#include <dtCore/uniqueid.h>

#include <dtCore/actortype.h>
#include <dtCore/actorproperty.h>

#include <dtGame/message.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gameactorproxy.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messagefactory.h>

#include <osg/Vec3d>
#include <osg/Endian>
#include <osg/io_utils>

#include <cstdlib>
#include <algorithm>


#if !defined(DELTA_WIN32)
   #if defined(__APPLE__)
      #include <sys/socket.h>
      #include <netinet/in.h>
   #else
      #include <sys/socket.h>
      #include <linux/in.h>
   #endif

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
   #include <dtUtil/mswin.h>
   #include <WinSock.h>
   typedef int socklen_t;
#endif

namespace dtHLAGM
{
   const std::string HLAComponent::PARAM_NAME_MAPPING_NAME("MAPPING_NAME"); // Interactions
   const std::string HLAComponent::ATTR_NAME_MAPPING_NAME("MAPPING_NAME"); // Objects
   const std::string HLAComponent::ATTR_NAME_ENTITY_TYPE("ENTITY_TYPE_ID"); // Objects
   const std::string HLAComponent::ATTR_NAME_ENTITY_TYPE_DEFAULT("EntityType"); // Objects
   const std::string HLAComponent::ABOUT_ACTOR_ID("aboutActorId");
   const std::string HLAComponent::SENDING_ACTOR_ID("sendingActorId");
   const dtCore::RefPtr<dtCore::SystemComponentType> HLAComponent::TYPE(new dtCore::SystemComponentType("HLAComponent","GMComponents",
         "Loads an RTI and converts delta3d messages back and forth with HLA"));
   const std::string HLAComponent::DEFAULT_NAME("HLAComponent");

   /////////////////////////////////////////////////////////////////////////////////
   HLAComponent::HLAComponent(dtCore::SystemComponentType& type)
   : dtGame::GMComponent(type)
   , mRTIAmbassador(NULL)
   , mLocalIPAddress(0x7f000001)
   , mDDMEnabled(false)
   , mMachineInfo(new dtGame::MachineInfo)
   {
      mLogger = &dtUtil::Log::GetInstance("hlacomponent.cpp");

      mSiteIdentifier = (unsigned short)(1 + (rand() % 65535));
      mApplicationIdentifier = (unsigned short)(1 + (rand() % 65535));
      mParameterTranslators.push_back(new RPRParameterTranslator(mCoordinates, mRuntimeMappings));

      // Default the entity type attribute name.
      SetHLAEntityTypeAttributeName( ATTR_NAME_ENTITY_TYPE_DEFAULT );
   }

   /////////////////////////////////////////////////////////////////////////////////
   HLAComponent::~HLAComponent()
   {
      LeaveFederationExecution();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::SetHLAEntityTypeAttributeName( const std::string& name )
   {
      mHLAEntityTypeAttrName = name;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const std::string& HLAComponent::GetHLAEntityTypeAttributeName() const
   {
      return mHLAEntityTypeAttrName;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool HLAComponent::IsEntityTypeAttribute(const std::string& attribName) const
   {
      return mHLAEntityTypeOtherAttrNames.count(attribName) > 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::RegisterObjectToActorWithRTI(ObjectToActor& objectToActor)
   {
      const std::string& thisObjectClassString = objectToActor.GetObjectClassName();
      dtCore::RefPtr<RTIObjectClassHandle> thisObjectClassHandle;

      try
      {
         thisObjectClassHandle = mRTIAmbassador->GetObjectClassHandle(thisObjectClassString);
      }
      catch (const RTIException& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                              "Could not find Object Class Name: %s - Message: %s",
                              thisObjectClassString.c_str(), ex.ToString().c_str());
         return;
      }

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Object class handle of object class %s is valid? %u.",
               thisObjectClassString.c_str(), thisObjectClassHandle.valid());
      }

      objectToActor.SetObjectClassHandle(thisObjectClassHandle);
      std::vector<AttributeToPropertyList>& thisAttributeToPropertyListVector = objectToActor.GetOneToManyMappingVector();

      RTIAttributeHandleSet ahs;

      std::vector<AttributeToPropertyList>::iterator attributeToPropertyListIterator = thisAttributeToPropertyListVector.begin();

      //if we have a name for the entity id attribute, subscribe to it and save the handle.
      if (!objectToActor.GetEntityIdAttributeName().empty())
      {
         try
         {
            dtCore::RefPtr<RTIAttributeHandle> entityIdentifierAttributeHandle =
               mRTIAmbassador->GetAttributeHandle(objectToActor.GetEntityIdAttributeName(), *thisObjectClassHandle);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "AttributeHandle for entity identifier on object class %s is valid: %u.",
                                   thisObjectClassString.c_str(), entityIdentifierAttributeHandle.valid());
            }

            objectToActor.SetEntityIdAttributeHandle(entityIdentifierAttributeHandle);

            ahs.insert(entityIdentifierAttributeHandle);
         }
         catch (const RTIException& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "Could not find Attribute '%s' for Object Class Name: '%s'. '%s'",
                                 objectToActor.GetEntityIdAttributeName().c_str(),
                                 thisObjectClassString.c_str(), ex.ToString().c_str());
         }

      }

      //if we have a name for the entity type attribute, subscribe to it and save the handle.
      if (objectToActor.GetEntityType() != NULL)
      {
         std::string entityTypeAttrName = GetHLAEntityTypeAttributeName();
         if (!objectToActor.GetEntityTypeAttributeName().empty())
         {
            entityTypeAttrName = objectToActor.GetEntityTypeAttributeName();
            mHLAEntityTypeOtherAttrNames.insert(entityTypeAttrName);
         }

         try
         {
            dtCore::RefPtr<RTIAttributeHandle> disIDAttributeHandle =
                  mRTIAmbassador->GetAttributeHandle(entityTypeAttrName, *thisObjectClassHandle);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Attribute handle for DIS ID on object class %s is valid: %u.",
                                   thisObjectClassString.c_str(), disIDAttributeHandle.valid());

            objectToActor.SetEntityTypeAttributeHandle(disIDAttributeHandle);

            ahs.insert(disIDAttributeHandle);
         }
         catch (const RTIException& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Could not find Attribute '%s' for Object Class Name: '%s'. '%s'",
                                 entityTypeAttrName.c_str(),
                                 thisObjectClassString.c_str(), ex.ToString().c_str());
         }
      }

      while (attributeToPropertyListIterator != thisAttributeToPropertyListVector.end())
      {
         AttributeToPropertyList& thisAttributeToPropertyList = *attributeToPropertyListIterator;
         std::string thisAttributeHandleString = thisAttributeToPropertyList.GetHLAName();

         if (!(thisAttributeHandleString.empty()))
         {
            // Avoid subscribing/publishing special attributes that are already
            // handled, such as the mapping name...
            if( thisAttributeHandleString == ATTR_NAME_MAPPING_NAME )
            {
               thisAttributeToPropertyList.SetSpecial( true );
            }
            // ...and avoid the entity type...
            else if( thisAttributeHandleString == ATTR_NAME_ENTITY_TYPE )
            {
               thisAttributeToPropertyList.SetSpecial( true );

               // Get the handle of the entity type and set it on this attribute to property mapping.
               thisAttributeToPropertyList.SetAttributeHandle(objectToActor.GetEntityTypeAttributeHandle());
            }
            // ...otherwise publish the attribute as usual.
            else
            {
               try
               {
                  dtCore::RefPtr<RTIAttributeHandle> entityIdentifierAttributeHandle =
                     mRTIAmbassador->GetAttributeHandle(thisAttributeHandleString, *thisObjectClassHandle);

                  if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                         "Attribute handle for %s on object class %s is valid: %c.",
                                         thisAttributeHandleString.c_str(),
                                         thisObjectClassString.c_str(),
                                         entityIdentifierAttributeHandle.valid());

                  thisAttributeToPropertyList.SetAttributeHandle(entityIdentifierAttributeHandle);

                  ahs.insert(entityIdentifierAttributeHandle);
               }
               catch (const RTIException& ex)
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                       "Could not find Attribute '%s' for Object Class Name: '%s'. '%s'",
                                       thisAttributeHandleString.c_str(),
                                       thisObjectClassString.c_str(), ex.ToString().c_str());
               }
            }

         }

         ++attributeToPropertyListIterator;
      }

      bool failed = false;
      if (!objectToActor.IsLocalOnly())
      {
         try
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                   "Subscribing to object class \"%s\".", thisObjectClassString.c_str());
            if (mDDMEnabled)
            {
               for (unsigned i = 0; i < mDDMSubscriptionRegions.size(); ++i)
               {
                  if (mDDMSubscriptionCalculators[i]->GetName() != objectToActor.GetDDMCalculatorName())
                  {
                     continue;
                  }

                  bool subscribed = false;
                  std::vector<dtCore::RefPtr<DDMRegionData> >& regionVector = mDDMSubscriptionRegions[i];
                  for (unsigned j = 0; j < regionVector.size(); ++j)
                  {
                     RTIRegion* r = regionVector[j]->GetRegion();
                     if (r != NULL)
                     {
                        if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                        {
                           mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                               "Subscribing to object class \"%s\" with region from calculator \"%s\"",
                                               thisObjectClassString.c_str(), mDDMSubscriptionCalculators[i]->GetName().c_str());
                        }
                        mRTIAmbassador->SubscribeObjectClassAttributes(*thisObjectClassHandle, ahs, r);
                        subscribed = true;
                     }
                  }

                  if (!subscribed && mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
                  {
                     mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                         "Subscribing to object class \"%s\" with region from calculator \"%s\" failed because no such calculator found.",
                                         thisObjectClassString.c_str(), mDDMSubscriptionCalculators[i]->GetName().c_str());
                  }
               }
            }
            else
            {
               mRTIAmbassador->SubscribeObjectClassAttributes(*thisObjectClassHandle, ahs);
            }
         }
         catch (const RTIException& ex)
         {
            failed = true;
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Error subscribing to object class \"%s\": \"%s\"", thisObjectClassString.c_str(), ex.ToString().c_str());
         }
      }

      if (!failed && !objectToActor.IsRemoteOnly())
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Publishing object class \"%s\" handle valid: %u.", thisObjectClassString.c_str(), thisObjectClassHandle.valid());
         }

         try
         {
            //need to put regions on this.
            mRTIAmbassador->PublishObjectClass(*thisObjectClassHandle, ahs);
         }
         catch (const RTIException& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Error publishing object class \"%s\": \"%s\"", thisObjectClassString.c_str(), ex.ToString().c_str());
         }
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::RegisterInteractionToMessageWithRTI(InteractionToMessage& interactionToMessage)
   {
      const std::string& thisInteractionClassString = interactionToMessage.GetInteractionName();
      dtCore::RefPtr<RTIInteractionClassHandle> thisInteractionClassHandle;
      try
      {

         thisInteractionClassHandle = mRTIAmbassador->GetInteractionClassHandle(thisInteractionClassString);

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "interaction class \"%s\" has handle id valid? \"%u\".",
                                thisInteractionClassString.c_str(),
                                thisInteractionClassHandle.valid());

         interactionToMessage.SetInteractionClassHandle(thisInteractionClassHandle);
         std::vector<ParameterToParameterList>& thisParameterToParameterListVector = interactionToMessage.GetOneToManyMappingVector();
         std::vector<ParameterToParameterList>::iterator parameterToParameterIterator = thisParameterToParameterListVector.begin();

         while (parameterToParameterIterator != thisParameterToParameterListVector.end())
         {
            ParameterToParameterList& thisParameterToParameterList = *parameterToParameterIterator;
            const std::string& thisParameterHandleString = thisParameterToParameterList.GetHLAName();

            // Avoid registering special case parameters that are already handled
            // elsewhere in the component, such as Interaction Name.
            if (thisParameterHandleString == PARAM_NAME_MAPPING_NAME)
            {
               thisParameterToParameterList.SetSpecial(true);
            }
            else // set the parameter handle as usual.
            {
               dtCore::RefPtr<RTIParameterHandle> thisParameterHandle = mRTIAmbassador->GetParameterHandle(thisParameterHandleString, *thisInteractionClassHandle);

               if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                  mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                      "Parameter handle for \"%s\" on interaction class \"%s\" is \"%u\".",
                                      thisParameterHandleString.c_str(),
                                      thisInteractionClassString.c_str(),
                                      thisParameterHandle.valid());

               thisParameterToParameterList.SetParameterHandle(thisParameterHandle);
            }
            ++parameterToParameterIterator;
         }
      }
      catch (const RTIException& ex)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                              "Could not register for interaction \"%s\" with error \"%s\"",
                              thisInteractionClassString.c_str(),
                              ex.ToString().c_str());
         return;
      }

      bool subscribed = false;
      try
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Subscribing to Interaction class %s.", thisInteractionClassString.c_str());
         }

         if (mDDMEnabled)
         {
            for (unsigned i = 0; i < mDDMSubscriptionRegions.size(); ++i)
            {
               if (mDDMSubscriptionCalculators[i]->GetName() != interactionToMessage.GetDDMCalculatorName())
               {
                  continue;
               }

               bool subscribed = false;
               std::vector<dtCore::RefPtr<DDMRegionData> >& regionVector = mDDMSubscriptionRegions[i];
               for (unsigned j = 0; j < regionVector.size(); ++j)
               {
                  RTIRegion* r = regionVector[j]->GetRegion();
                  if (r != NULL)
                  {
                     if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
                     {
                        mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                            "Subscribing to interaction class \"%s\" with region from calculator \"%s\"",
                                            thisInteractionClassString.c_str(), mDDMSubscriptionCalculators[i]->GetName().c_str());
                     }
                     mRTIAmbassador->SubscribeInteractionClass(*thisInteractionClassHandle, r);
                     subscribed = true;
                  }
               }

               if (!subscribed && mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                                      "Subscribing to interaction class \"%s\" with region from calculator \"%s\" failed because no such calculator exists.",
                                      thisInteractionClassString.c_str(), mDDMSubscriptionCalculators[i]->GetName().c_str());
               }
            }
         }
         else
         {
            mRTIAmbassador->SubscribeInteractionClass(*thisInteractionClassHandle);
            subscribed = true;
         }


         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                "Publishing to Interaction class %s.", thisInteractionClassString.c_str());
         }
         //Need to put regions on this.
         mRTIAmbassador->PublishInteractionClass(*thisInteractionClassHandle);
      }
      catch (const RTIException& ex)
      {
         if (!subscribed)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "Error subscribing to interaction class \"%s\".  Error: \"%s\".", thisInteractionClassString.c_str(), ex.ToString().c_str());
         }
         else
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                "Error publishing interaction class \"%s\".  Error \"%s\".", thisInteractionClassString.c_str(), ex.ToString().c_str());
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::SetDDMEnabled(bool enable)
   {
      if (mDDMEnabled == enable)
         return;

      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not register a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      mDDMEnabled = enable;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::JoinFederationExecution(const std::string& executionName,
                                              const std::vector<std::string>& fedFilenames,
                                              const std::string& federateName,
                                              const std::string& ridFile,
                                              const std::string& rtiImplementationName)
   {
      dtUtil::FileInfo fi = dtUtil::FileUtils::GetInstance().GetFileInfo(ridFile);
      if (!ridFile.empty() && fi.fileType == dtUtil::FILE_NOT_FOUND)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
               "Failed to set the RID file name to \"%s\" because the file is not found.", ridFile.c_str());
      }
      else
      {
         try
         {
            std::string path = fi.path;
            if (path.empty())
               path = "./";

            std::string absPath = dtUtil::FileUtils::GetInstance().GetAbsolutePath(path);

            dtUtil::SetEnvironment("RTI_RID_FILE", absPath + dtUtil::FileUtils::PATH_SEPARATOR + fi.baseName);
         }
         catch(const dtUtil::Exception& e)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Failed to set the RID file name to \"%s\" for the following reason: %s", ridFile.c_str(), e.ToString().c_str());
         }
      }

      SOCKET some_socket = socket(AF_INET, SOCK_DGRAM, 0);
      //
      // Code from http://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1047083789&id=1045780608
      //
      int len;
      sockaddr_in other, me;

      memset(&other, 0, sizeof(other));

      other.sin_family = AF_INET;
      other.sin_port = 8192;

      #ifdef DELTA_WIN32
      other.sin_addr.S_un.S_addr = 0x7F000001;
      #else
      other.sin_addr.s_addr = 0x7F000001;
      #endif

      if(connect(some_socket, (sockaddr*)&other, sizeof(other)) == 0)
      {
         if(getsockname(some_socket, (sockaddr*)&me, (socklen_t*)&len) == 0)
         {
            #ifdef DELTA_WIN32
            mLocalIPAddress = me.sin_addr.S_un.S_addr;
            #else
            mLocalIPAddress = me.sin_addr.s_addr;
            #endif

            if(osg::getCpuByteOrder() == osg::LittleEndian)
            {
               osg::swapBytes((char*)&mLocalIPAddress, sizeof(mLocalIPAddress));
            }

            #ifdef DELTA_WIN32
            mSiteIdentifier = me.sin_addr.S_un.S_un_w.s_w1;
            #else
            win_addr temp_addr;
            memcpy(&temp_addr,&me.sin_addr,sizeof(win_addr));
            mSiteIdentifier = temp_addr.S_un.S_un_w.s_w1;
            #endif
         }
      }

      mRTIAmbassador = RTIAmbassador::Create(rtiImplementationName);

      try
      {
         // TODO read connection specific info from somewhere.
         mRTIAmbassador->ConnectToRTI(*this, "");
         std::vector<std::string> fedFilesFound;
         fedFilesFound.reserve(fedFilenames.size());
         for (unsigned i = 0; i < fedFilenames.size(); ++i)
         {
            fedFilesFound.push_back( dtUtil::FindFileInPathList(fedFilenames[i]) );
            if (fedFilesFound.back().empty())
            {
               fedFilesFound.pop_back();
            }
         }

         if (fedFilesFound.empty())
         {
            mRTIAmbassador = NULL;

            std::ostringstream oss;
            for (unsigned i = 0; i < fedFilenames.size(); ++i)
            {
               oss << " \"" << fedFilenames[i] << "\"";
            }

            throw dtUtil::Exception(std::string("Unable to create the federation.  No fed files were found in list: ") + oss.str(), __FILE__, __LINE__);
         }

         mRTIAmbassador->CreateFederationExecution(executionName, fedFilesFound);

      }
      catch(const RTIException&)
      {
         mRTIAmbassador = NULL;
         throw;
      }

      try
      {
         mRTIAmbassador->JoinFederationExecution(federateName, executionName);
      }
      catch(const RTIException&)
      {
         mRTIAmbassador = NULL;
         throw;
      }

      mEntityIdentifierCounter = 1;
      mEventIdentifierCounter = 1;

      mExecutionName = executionName;

      if (mDDMEnabled)
      {
         try
         {
            CreateDDMSubscriptionRegions();
         }
         catch (const dtUtil::Exception&)
         {
            LeaveFederationExecution();
            //rethrow the exception.
            throw;
         }
      }

      PublishSubscribe();

   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::LeaveFederationExecution()  //this is kind of broken
   {
      if (!mExecutionName.empty())
      {
         if (GetGameManager() != NULL)
         {
            // send delete messages for all actors created by the hla component
            std::vector<dtCore::UniqueId> allActorsCreatedByHLA;
            mRuntimeMappings.GetAllActorIds(allActorsCreatedByHLA);

            for (unsigned i = 0; i < allActorsCreatedByHLA.size(); ++i)
            {
               DeleteActor(allActorsCreatedByHLA[i]);
            }
         }
         //drop all instance mapping data.
         mRuntimeMappings.Clear();
         mObjectRegQueue.clear();

         if (mDDMEnabled)
         {
            DestroyDDMSubscriptionRegions();
         }

         try
         {
            mRTIAmbassador->ResignFederationExecution(mExecutionName);
         }
         catch(RTIException& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "Could not resign or destroy the federation execution: ", ex.ToString().c_str());
         }


         mExecutionName.clear();
      }

      mRTIAmbassador = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::PublishSubscribe()
   {
      // Clear it so we can collect all the valid names.
      mHLAEntityTypeOtherAttrNames.clear();
      mHLAEntityTypeOtherAttrNames.insert(mHLAEntityTypeAttrName);

      std::vector<ObjectToActor*> otaVec;
      GetAllObjectToActorMappings(otaVec);

      std::vector<ObjectToActor*>::iterator objectToActorIterator, otaIterEnd;

      objectToActorIterator = otaVec.begin();
      otaIterEnd = otaVec.end();

      while (objectToActorIterator != otaIterEnd)
      {
         ObjectToActor& thisObjectToActor = **objectToActorIterator;
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

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::SetSiteIdentifier(unsigned short siteIdentifier)
   {
      mSiteIdentifier = siteIdentifier;
   }

   /////////////////////////////////////////////////////////////////////////////////
   unsigned short HLAComponent::GetSiteIdentifier() const
   {
      return mSiteIdentifier;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::SetApplicationIdentifier(unsigned short applicationIdentifier)
   {
      mApplicationIdentifier = applicationIdentifier;
   }

   /////////////////////////////////////////////////////////////////////////////////
   unsigned short HLAComponent::GetApplicationIdentifier() const
   {
      return mApplicationIdentifier;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::AddParameterTranslator(ParameterTranslator& newTranslator)
   {
      mParameterTranslators.push_back(&newTranslator);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::ProvideAttributeValueUpdate(RTIObjectInstanceHandle& theObject,
            const RTIAttributeHandleSet& /*theAttributes*/)
   {
      const dtCore::UniqueId* actorId = mRuntimeMappings.GetId(theObject);
      if (actorId != NULL)
      {
         dtGame::GameActorProxy* gap = GetGameManager()->FindGameActorById(*actorId);
         if (gap != NULL)
         {
            // This won't work if the actor is remote because it won't send the update.
            if (gap->IsRemote() && mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING,
                  "An update was requested from the RTI for actor %s with type %s.%s, but the actor is Remote.",
                  gap->GetName().c_str(), gap->GetActorType().GetCategory().c_str(), gap->GetActorType().GetName().c_str());
            }

            // We eventually need to lookup the list of properties needed using the attr handle set.
            gap->NotifyFullActorUpdate();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::RemoveObjectInstance(RTIObjectInstanceHandle& theObject,
            const std::string& /*theTag*/)
   {
      const dtCore::UniqueId* actorId = mRuntimeMappings.GetId(theObject);

      if (actorId != NULL)
      {
         DeleteActor(*actorId);
         //This is subtle, but the mapping has to be removed after sending the message because
         //the actorId is deleted when it's removed from the mappings.
         mRuntimeMappings.Remove(theObject);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::DeleteActor(const dtCore::UniqueId& toDelete)
   {
      dtCore::RefPtr<dtGame::Message> msg = GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_DELETED);
      msg->SetSource(*mMachineInfo);
      msg->SetAboutActorId(toDelete);
      GetGameManager()->SendMessage(*msg);
   }

   /////////////////////////////////////////////////////////////////////////////////
   const ObjectToActor* HLAComponent::GetActorMapping(const dtCore::ActorType &type) const
   {
      std::map<dtCore::RefPtr<const dtCore::ActorType>, dtCore::RefPtr<ObjectToActor> >::const_iterator actorToObjectIterator;
      dtCore::RefPtr<const dtCore::ActorType> refActorType = &type;
      const ObjectToActor* thisObjectToActor = NULL;

      actorToObjectIterator = mActorToObjectMap.find(refActorType);
      if (actorToObjectIterator != mActorToObjectMap.end())
      {
         thisObjectToActor = actorToObjectIterator->second.get();
      }
      return thisObjectToActor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   ObjectToActor* HLAComponent::GetActorMapping(const dtCore::ActorType& type)
   {
      std::map<dtCore::RefPtr<const dtCore::ActorType>, dtCore::RefPtr<ObjectToActor> >::iterator actorToObjectIterator;
      dtCore::RefPtr<const dtCore::ActorType> refActorType = &type;
      ObjectToActor* thisObjectToActor = NULL;

      actorToObjectIterator = mActorToObjectMap.find(refActorType);
      if (actorToObjectIterator != mActorToObjectMap.end())
      {
         thisObjectToActor = actorToObjectIterator->second.get();
      }

      return thisObjectToActor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const ObjectToActor* HLAComponent::GetObjectMapping(const std::string& objTypeName, const EntityType* thisEntityType) const
   {
      return InternalGetObjectMapping(objTypeName, thisEntityType);
   }

   /////////////////////////////////////////////////////////////////////////////////
   ObjectToActor* HLAComponent::GetObjectMapping(const std::string& objTypeName, const EntityType* thisEntityType)
   {
      return const_cast<ObjectToActor*>(InternalGetObjectMapping(objTypeName, thisEntityType));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const ObjectToActor* HLAComponent::InternalGetObjectMapping(const std::string& objTypeName, const EntityType* thisEntityType) const
   {
      std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::const_iterator objectToActorIterator;
      const ObjectToActor* thisObjectToActor = NULL;
      objectToActorIterator = mObjectToActorMap.find(objTypeName);
      if (objectToActorIterator != mObjectToActorMap.end())
      {
         if (thisEntityType != NULL)
         {
            //find the one with the right DIS ID if it exists.
            while (objectToActorIterator !=  mObjectToActorMap.end() &&
               objectToActorIterator->second->GetObjectClassName() == objTypeName)
            {
               if (objectToActorIterator->second->GetEntityType() != NULL &&
                  *objectToActorIterator->second->GetEntityType() == *thisEntityType)
                  break;

               ++objectToActorIterator;
            }

            if (objectToActorIterator ==  mObjectToActorMap.end())
            {
               return NULL;
            }

            if (objectToActorIterator->second->GetEntityType() == NULL)
               return NULL;

            if (*objectToActorIterator->second->GetEntityType() != *thisEntityType)
               return NULL;
         }

         thisObjectToActor = objectToActorIterator->second.get();
      }
      return thisObjectToActor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::RegisterActorMapping(dtCore::ActorType &type,
                                               const std::string& objTypeName,
                                               const EntityType* thisEntityType,
                                               std::vector<AttributeToPropertyList> &oneToOneActorVector,
                                               ObjectToActor::LocalOrRemoteType& localOrRemote)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not register a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }

      dtCore::RefPtr<ObjectToActor> thisActorMapping = new ObjectToActor;

      thisActorMapping->SetActorType(type);
      thisActorMapping->SetObjectClassName(objTypeName);
      thisActorMapping->SetEntityType(thisEntityType);
      thisActorMapping->SetOneToManyMappingVector(oneToOneActorVector);
      thisActorMapping->SetLocalOrRemoteType(localOrRemote);

      dtCore::RefPtr<dtCore::ActorType> refActorType = &type;

      RegisterActorMapping(*thisActorMapping);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::RegisterActorMapping(ObjectToActor& objectToActor)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not register a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      if (!objectToActor.IsRemoteOnly())
      {
         if (!mActorToObjectMap.insert(std::make_pair(&objectToActor.GetActorType(), &objectToActor)).second)
         {
            if (!objectToActor.IsLocalOnly())
            {
               std::ostringstream ss;
               ss << "Unable to register local to remote mapping for actor type "
                  << objectToActor.GetActorType().GetCategory() << "."
                  << objectToActor.GetActorType().GetName() << " and classname"
                  << objectToActor.GetObjectClassName() <<  ".  "
                  << "A mapping already exists.  Set the mapping to remote only.";

               throw dtHLAGM::XmlConfigException(ss.str(), __FILE__, __LINE__);
            }
            else
            {
               std::ostringstream ss;
               ss << "Unable to register local-only mapping for actor type "
                  << objectToActor.GetActorType().GetCategory() << "."
                  << objectToActor.GetActorType().GetName() << " and classname"
                  << objectToActor.GetObjectClassName() <<  ".  "
                  << "A mapping already exists.  Mapping will be ignored.";

               throw dtHLAGM::XmlConfigException(ss.str(), __FILE__, __LINE__);
            }
         }
      }

      if (!objectToActor.IsLocalOnly())
      {
         if (GetObjectMapping(objectToActor.GetObjectClassName(), objectToActor.GetEntityType()) != NULL)
         {
            std::ostringstream ss;
            ss << "Unable to register object mapping \""
               << objectToActor.GetActorType().GetFullName() << ", classname \""
               << objectToActor.GetObjectClassName();
               if (objectToActor.GetEntityType() != NULL)
               {
                  ss <<  "\" and DIS ID \"" << *objectToActor.GetEntityType();

               }
               ss << ".\"  A mapping with both the same object name and DIS ID (if enabled) exists.  "
               << "Set one to <localOnly>true</localOnly> if both mappings are required.";

            throw dtHLAGM::XmlConfigException(ss.str(), __FILE__, __LINE__);
         }
         mObjectToActorMap.insert(std::make_pair(objectToActor.GetObjectClassName(), &objectToActor));
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::UnregisterActorMapping(dtCore::ActorType &type)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not unregister a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      dtCore::RefPtr<ObjectToActor> otoa =  InternalUnregisterActorMapping(type);
      if (otoa.valid() && !otoa->IsLocalOnly())
         InternalUnregisterObjectMapping(otoa->GetObjectClassName(), otoa->GetEntityType());
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::UnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisEntityType)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not unregister a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      dtCore::RefPtr<ObjectToActor> otoa =  InternalUnregisterObjectMapping(objTypeName, thisEntityType);
      if (otoa.valid() && !otoa->IsRemoteOnly())
         InternalUnregisterActorMapping(otoa->GetActorType());
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ObjectToActor> HLAComponent::InternalUnregisterActorMapping(const dtCore::ActorType &type)
   {
      std::map<dtCore::RefPtr<const dtCore::ActorType>, dtCore::RefPtr<ObjectToActor> >::iterator actorToObjectIterator;
      dtCore::RefPtr<ObjectToActor> thisObjectToActor;

      actorToObjectIterator = mActorToObjectMap.find(&type);
      if (actorToObjectIterator != mActorToObjectMap.end())
      {
         thisObjectToActor = actorToObjectIterator->second;

         mActorToObjectMap.erase(actorToObjectIterator);

      }
      return thisObjectToActor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ObjectToActor> HLAComponent::InternalUnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisEntityType)
   {
      std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::iterator objectToActorIterator;
      dtCore::RefPtr<ObjectToActor> thisObjectToActor;

      objectToActorIterator = mObjectToActorMap.find(objTypeName);
      if (objectToActorIterator != mObjectToActorMap.end())
      {
         if (thisEntityType != NULL)
         {
            //find the one with the right DIS ID if it exists.
            while (objectToActorIterator !=  mObjectToActorMap.end() &&
               objectToActorIterator->second->GetObjectClassName() == objTypeName)
            {
               if (objectToActorIterator->second->GetEntityType() != NULL &&
                  *objectToActorIterator->second->GetEntityType() == *thisEntityType)
                  break;

               ++objectToActorIterator;
            }

            if (objectToActorIterator->second->GetEntityType() == NULL)
               return NULL;

            if (*objectToActorIterator->second->GetEntityType() != *thisEntityType)
               return NULL;
         }
         thisObjectToActor = objectToActorIterator->second;

         mObjectToActorMap.erase(objectToActorIterator);
      }
      return thisObjectToActor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const InteractionToMessage* HLAComponent::GetMessageMapping(const dtGame::MessageType& type) const
   {
      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::const_iterator messageToInteractionIterator;
      const InteractionToMessage* thisInteractionToMessage = NULL;

      if ((messageToInteractionIterator = mMessageToInteractionMap.find(&type)) != (mMessageToInteractionMap.end()))
      {
         thisInteractionToMessage = messageToInteractionIterator->second.get();
      }
      return thisInteractionToMessage;
   }

   /////////////////////////////////////////////////////////////////////////////////
   InteractionToMessage* HLAComponent::GetMessageMapping(const dtGame::MessageType& type)
   {
      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::iterator messageToInteractionIterator;
      InteractionToMessage* thisInteractionToMessage = NULL;

      if ((messageToInteractionIterator = mMessageToInteractionMap.find(&type)) != (mMessageToInteractionMap.end()))
      {
         thisInteractionToMessage = messageToInteractionIterator->second.get();
      }
      return thisInteractionToMessage;
   }

   /////////////////////////////////////////////////////////////////////////////////
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

   /////////////////////////////////////////////////////////////////////////////////
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

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::RegisterMessageMapping(InteractionToMessage& interactionToMessage)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not register a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      mMessageToInteractionMap.insert(std::make_pair(&interactionToMessage.GetMessageType(), &interactionToMessage));

      mInteractionToMessageMap.insert(std::make_pair(interactionToMessage.GetInteractionName(), &interactionToMessage));
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::RegisterMessageMapping(const dtGame::MessageType &type,
                                                 const std::string& interactionTypeName,
                                                 std::vector<ParameterToParameterList>& oneToOneMessageVector)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not register a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      dtCore::RefPtr<InteractionToMessage> thisMessageMapping = new InteractionToMessage;

      thisMessageMapping->SetMessageType(type);
      thisMessageMapping->SetInteractionName(interactionTypeName);
      thisMessageMapping->SetOneToManyMappingVector(oneToOneMessageVector);

      RegisterMessageMapping(*thisMessageMapping);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::UnregisterMessageMapping(const dtGame::MessageType& type)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not unregister a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      dtCore::RefPtr<InteractionToMessage> itom = InternalUnregisterMessageMapping(type);
      if (itom.valid())
         InternalUnregisterInteractionMapping(itom->GetInteractionName());
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::UnregisterInteractionMapping(const std::string& interName)
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent may not unregister a mapping because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      dtCore::RefPtr<InteractionToMessage> itom = InternalUnregisterInteractionMapping(interName);
      if (itom.valid())
         InternalUnregisterInteractionMapping(itom->GetInteractionName());
   }

   /////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<InteractionToMessage> HLAComponent::InternalUnregisterMessageMapping(const dtGame::MessageType& type)
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

   /////////////////////////////////////////////////////////////////////////////////
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

   /////////////////////////////////////////////////////////////////////////////////
   template <typename MappingObject, typename mapType, typename mapTypeIterator>
   void GetAllMappings(std::vector<MappingObject*>& toFill, mapType& readFrom, mapTypeIterator beginIt,
            bool (MappingObject::* acceptMethod)(void) const = NULL)
   {
      //Adding the beginIt parameter for the begin iterator makes the template able to figure out what type
      //it should be.

      mapTypeIterator mappingIterator = beginIt;

      while (mappingIterator != readFrom.end())
      {
         MappingObject* thisMapping = (mappingIterator->second).get();
         if (acceptMethod == NULL || (thisMapping->*acceptMethod)())
         {
            toFill.push_back(thisMapping);
         }
         ++mappingIterator;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::GetAllObjectToActorMappings(std::vector<ObjectToActor*>& toFill)
   {
      toFill.clear();
      //This will probably always be too big, but it is the max possible size
      toFill.reserve(mObjectToActorMap.size() + mActorToObjectMap.size());
      GetAllMappings(toFill, mObjectToActorMap, mObjectToActorMap.begin());
      GetAllMappings(toFill, mActorToObjectMap, mActorToObjectMap.begin(), &ObjectToActor::IsLocalOnly);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::GetAllObjectToActorMappings(std::vector<const ObjectToActor*>& toFill) const
   {
      toFill.clear();
      //This will probably always be too big, but it is the max possible size
      toFill.reserve(mObjectToActorMap.size() + mActorToObjectMap.size());
      GetAllMappings(toFill, mObjectToActorMap, mObjectToActorMap.begin());
      GetAllMappings<const ObjectToActor,
         const std::map<dtCore::RefPtr<const dtCore::ActorType>, dtCore::RefPtr<ObjectToActor> >,
         std::map<dtCore::RefPtr<const dtCore::ActorType>, dtCore::RefPtr<ObjectToActor> >::const_iterator>
         (toFill, mActorToObjectMap, mActorToObjectMap.begin(), &ObjectToActor::IsLocalOnly);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::GetAllInteractionToMessageMappings(std::vector<InteractionToMessage*>& toFill)
   {
      toFill.clear();
      toFill.reserve(mInteractionToMessageMap.size());
      GetAllMappings(toFill, mInteractionToMessageMap, mInteractionToMessageMap.begin());
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::GetAllInteractionToMessageMappings(std::vector<const InteractionToMessage*>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mInteractionToMessageMap.size());
      GetAllMappings(toFill, mInteractionToMessageMap, mInteractionToMessageMap.begin());
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::ClearConfiguration()
   {
      if (!mExecutionName.empty())
      {
         throw dtUtil::Exception("The HLAComponent configuration may not be cleared because it is connected to a federation.",
               __FILE__, __LINE__);
      }
      mActorToObjectMap.clear();
      mObjectToActorMap.clear();
      mMessageToInteractionMap.clear();
      mInteractionToMessageMap.clear();
      mHLAEntityTypeOtherAttrNames.clear();
   }


   /////////////////////////////////////////////////////////////////////////////////
   dtGame::MessageParameter* HLAComponent::FindOrAddMessageParameter(const std::string& name, dtCore::DataType& type, dtGame::Message& msg)
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

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::ReflectAttributeValues(RTIObjectInstanceHandle& theObject,
            const RTIAttributeHandleValueMap& theAttributes,
            const std::string& /*theTag*/)
   {
      try
      {
	      //Tag tag;
         //tag.Decode(theTag);
         //std::cout << tag.GetTag() << std::endl;

         dtCore::RefPtr<ObjectToActor> bestObjectToActor;
         const dtCore::UniqueId* currentActorId;

         currentActorId = mRuntimeMappings.GetId(theObject);

         if (currentActorId == NULL)
            return;

         bestObjectToActor = mRuntimeMappings.GetObjectToActor(theObject);

         bool needBestMapping = false;
         bool bNewObject = false;

         if (bestObjectToActor.valid())
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "An object to actor was found for object with ID \"%s\".", currentActorId->ToString().c_str());
            }
         }
         else
         {
            needBestMapping = true;
            bNewObject = true;

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Preparing to Map in new Object.  The id is \"%s\".", currentActorId->ToString().c_str());
            }
         }

         if (needBestMapping)
         {
            bool ret = DoGetBestObjectToActor(bestObjectToActor, theObject, theAttributes, currentActorId);
            if (ret == false) return;
         }

         if (bNewObject)
         {
            AddActorIDToMap(theAttributes, bestObjectToActor, currentActorId);
         }

         //USE OBJECTTOACTOR TO CREATE ACTOR UPDATE
         std::vector<AttributeToPropertyList>& currentAttributeToPropertyListVector = bestObjectToActor->GetOneToManyMappingVector();

         std::vector<AttributeToPropertyList>::iterator vectorIterator;

         dtGame::GameManager* gameManager = GetGameManager();

         dtGame::MessageFactory& factory = gameManager->GetMessageFactory();

         dtCore::RefPtr<dtGame::Message> msg;
         if (bNewObject)
            msg = factory.CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED);
         else
            msg = factory.CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);

         AttributeToPropertyList* curAttrToProp = NULL;
         for (vectorIterator = currentAttributeToPropertyListVector.begin();
               vectorIterator != currentAttributeToPropertyListVector.end();
               ++vectorIterator)
         {
            curAttrToProp = &(*vectorIterator);

            // Avoid invalid mappings.
            if( curAttrToProp->IsInvalid() )
            {
               continue;
            }

            const std::string& attributeString = curAttrToProp->GetHLAName();

            bool matched = false;

            // If attribute name is valid...
            if( ! attributeString.empty() )
            {
               // Get ready to capture an attribute buffer and its length.
               std::string buf;

               // Handle special cases...
               if( curAttrToProp->IsSpecial() )
               {
                  // Make sure that the default parameters are not used.
                  matched = true;

                  // Handle the Object Mapping Name or
                  // Entity Type if Entity Type usage is enabled.
                  // NOTE: Entity Type will be NULL on the ObjectToActor
                  // if Entity Type usage is disabled.

                  // Is this Object Mapping Name?
                  if( attributeString == ATTR_NAME_MAPPING_NAME )
                  {
                     const std::string& mappingName = bestObjectToActor->GetMappingName();
                     buf = mappingName;
                  }
                  // Is this the Entity Type?
                  // GetEntityType will not be NULL if Entity Types are being used.
                  else if( attributeString == ATTR_NAME_ENTITY_TYPE )
                  {
                     buf = GetAttributeBufferAndLength( theAttributes, *curAttrToProp);
                  }
                  else
                  {
                     // Bad match. Special parameter does not have the name of
                     // a special attribute.
                     curAttrToProp->SetInvalid( true );

                     std::ostringstream reason;
                     reason << "HLA attribute mapping \""
                        << attributeString.c_str()
                        << "\" was marked as SPECIAL but does NOT match the name of a special type HLA attribute."
                        << std::endl;
                     LogMappingError( *curAttrToProp, reason.str() );
                  }

               }
               else
               {
                  buf = GetAttributeBufferAndLength( theAttributes, *curAttrToProp);

                  matched = !buf.empty();
               }

               // If an attribute was found to match, its buffer and length will
               // have been obtained and can be used to create the message parameters.
               if( matched && !buf.empty() )
               {
                  bool success = CreateMessageParameters(
                     buf, *curAttrToProp, *msg, true );

                  if( ! success )
                  {
                     // One or more parameter mappings failed.
                     curAttrToProp->SetInvalid( true );

                     LogMappingError( *curAttrToProp, "Mapping was not successful, it was not able to create the correct message parameters." );
                  }
               }
            }

            //If this attribute mapping is not in the HLA message,
            //use defaults for all parameters that need them.
            if (!matched)
            {
               SetDefaultParameters(vectorIterator, bNewObject, msg.get());
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

   /////////////////////////////////////////////////////////////////////////////////
   std::string HLAComponent::GetAttributeBufferAndLength( const RTIAttributeHandleValueMap& attributeSet,
      AttributeToPropertyList& curAttrToProp )
   {
      if( ! curAttrToProp.GetParameterDefinitions().empty() )
      {
         RTIAttributeHandleValueMap::const_iterator i, iend;
         i = attributeSet.begin();
         iend = attributeSet.end();
         for(; i != iend; ++i )
         {
            RTIAttributeHandle* handle = i->first;

            if( handle != NULL && curAttrToProp.GetAttributeHandle() != NULL &&
                     *handle == *curAttrToProp.GetAttributeHandle() )
            {
               return i->second.mData;
            }
         }
      }

      // The target attribute was not found.
      return std::string();
   }

   /////////////////////////////////////////////////////////////////////////////////
   ObjectToActor* HLAComponent::GetBestObjectToActor(RTIObjectInstanceHandle& theObject,
      const RTIAttributeHandleValueMap& theAttributes, bool& hadEntityTypeProperty)
   {
      int bestRank = -1;
      ObjectToActor* bestObjectToActor = NULL;

      dtCore::RefPtr<RTIObjectClassHandle> classHandle = mRTIAmbassador->GetObjectClassForInstance(theObject);

      std::string classHandleString = mRTIAmbassador->GetObjectClassName(*classHandle);

      hadEntityTypeProperty = false;
      EntityType currentEntityType;

      RTIAttributeHandleValueMap::const_iterator i, iend;
      i = theAttributes.begin();
      iend = theAttributes.end();
      for (;i != iend; ++i)
      {
         RTIAttributeHandle* handle = i->first;

         std::string attribName = std::string(mRTIAmbassador->GetAttributeName(*handle, *classHandle));

         if (IsEntityTypeAttribute(attribName))
         {
            std::string buf = i->second.mData;

            hadEntityTypeProperty = true;
            currentEntityType.Decode(buf.c_str());

            std::pair<ObjectToActorMapIter, ObjectToActorMapIter> iterPair;
            iterPair = mObjectToActorMap.equal_range(classHandleString);

            ObjectToActorMapIter i, iend;
            i = iterPair.first;
            iend = iterPair.second;
            for (; i != iend; ++i)
            {
               ObjectToActor& thisObjectToActor = *i->second;

               if (thisObjectToActor.GetEntityType() == NULL ||
                        (!thisObjectToActor.GetEntityTypeAttributeName().empty() && attribName != thisObjectToActor.GetEntityTypeAttributeName())
                        )
               {
                  continue;
               }

               int thisRank = currentEntityType.RankMatch(*thisObjectToActor.GetEntityType());
               if (thisRank > bestRank)
               {
                  bestRank = thisRank;
                  bestObjectToActor = &thisObjectToActor;
               }
            }
            // Took the break out because Physical Entity has an alterate entity type field, and someone MAY want
            // to key off of that, so we keep looking and doing the best ranking match.
            // break;
         }
      }

      if (bestObjectToActor != NULL)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << currentEntityType;
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Creating actor of type \"%s\" for DIS ID [%s] and object class \"%s\".",
               bestObjectToActor->GetActorType().GetFullName().c_str(),
               ss.str().c_str(), classHandleString.c_str());
         }

      }
      else
      {
         if (hadEntityTypeProperty)
         {
            std::ostringstream ss;
            ss << currentEntityType;
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_WARNING))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                     "Could not find valid actor to match [%s] and object class \"%s\".",
                     ss.str().c_str(), classHandleString.c_str());
            }
         }
         else
         {
            std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::iterator i;

            // if we didn't have an entity type and we have a mapping that uses no entity type
            // we assign that as the best object to actor.  This would break in the case where
            // a user tried to add mappings both with and without an entity type and actors didn't
            // always send their entity type in an update.
            for (i = mObjectToActorMap.find(classHandleString); i != mObjectToActorMap.end()
               && i->first == classHandleString;
                 ++i)
            {
               ObjectToActor& thisObjectToActor = *i->second;
               if (thisObjectToActor.GetEntityType() == NULL)
               {
                  bestObjectToActor = &thisObjectToActor;
                  break;
               }
            }

            //If we still don't have a mapping
            if (bestObjectToActor == NULL && mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                  "Ignoring entity update with no entity type.");
         }

      }

      return bestObjectToActor;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool HLAComponent::DoGetBestObjectToActor( dtCore::RefPtr<ObjectToActor>& bestObjectToActor,
                                             RTIObjectInstanceHandle& theObject,
                                             const RTIAttributeHandleValueMap& theAttributes,
                                             const dtCore::UniqueId* currentActorId )
   {
      bool hadEntityTypeProperty;
      bestObjectToActor = GetBestObjectToActor(theObject, theAttributes, hadEntityTypeProperty);


      if (bestObjectToActor.valid())
      {
         bool result = mRuntimeMappings.Put(theObject, *bestObjectToActor);
         if (result == false)
         {
            std::string className = mRTIAmbassador->GetObjectClassNameForInstance(theObject);
            mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
               "Unable to map in object with class \"%s\". ObjectToActor struct may already be mapped or corrupt.", className.c_str());
         }
      }
      else
      {
         //support for the useEntityId concept needs to be added.
         if (hadEntityTypeProperty) //|| !useEntityId)
         {
            // Test clean up.  This is not a complete solution as we don't want to return
            // an error everytime we get a partial update
            mRuntimeMappings.Remove(*currentActorId);

            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                  "Unable to map in object, no object to actor found.");
            }

            mRuntimeMappings.Remove(theObject);
         }
         else
         {
            if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "No mapping was found, but the PDU had no entity type data.  Waiting for a full update.");
            }
         }

         return false;
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::AddActorIDToMap( const RTIAttributeHandleValueMap& theAttributes, dtCore::RefPtr<ObjectToActor> bestObjectToActor, const dtCore::UniqueId* currentActorId )
   {
      RTIAttributeHandleValueMap::const_iterator i, iend;
      i = theAttributes.begin();
      iend = theAttributes.end();
      for (; i != iend; ++i)
      {
         RTIAttributeHandle* handle = i->first;
         if (handle == bestObjectToActor->GetEntityIdAttributeHandle())
         {
            EntityIdentifier ei;

            std::string buffer = i->second.mData;
            if (buffer.length() < size_t(ei.EncodedLength()))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Expected Entity id attribute to have length %u, but it has length %u.  Id will be ignored",
                  ei.EncodedLength(), buffer.length());
            }
            else
            {
               ei.Decode(buffer.c_str());
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

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::DiscoverObjectInstance(RTIObjectInstanceHandle& theObject,
            RTIObjectClassHandle& /*theObjectClassHandle*/,
            const std::string& theObjectName)
   {
      dtCore::UniqueId newId;
      mRuntimeMappings.Put(theObject, newId);

      // Set an RTI ID mapping if the RTI ID is valid
      if (!theObjectName.empty())
      {
         mRuntimeMappings.Put(theObjectName, newId);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
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

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::MapToMessageParameters(const char* buffer,
                                             size_t size,
                                             std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters,
                                             const OneToManyMapping& mapping) const
   {
      const ParameterTranslator* pt = FindTranslatorForAttributeType(mapping.GetHLAType());

      if (pt != NULL)
         pt->MapToMessageParameters(buffer, size, parameters, mapping);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::MapFromMessageParameters(char* buffer,
                                 size_t& maxSize,
                                 std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters,
                                 const OneToManyMapping& mapping) const
   {
      const ParameterTranslator* pt = FindTranslatorForAttributeType(mapping.GetHLAType());

      if (pt != NULL)
      {
         pt->MapFromMessageParameters(buffer, maxSize, parameters, mapping);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::ReceiveInteraction(RTIInteractionClassHandle& interactionClassHandle,
            const RTIParameterHandleValueMap& theParameters,
            const std::string& /*theTag*/)
   {
      try
      {
         // Compare RTI InteractionClassHandle to InteractionToMessageMap Class Handle
         std::string classHandleString = mRTIAmbassador->GetInteractionClassName(interactionClassHandle);

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

         ParameterToParameterList* curParamMapping = NULL;
         std::vector<ParameterToParameterList> currentParameterToParameterListVector = thisInteractionToMessage->GetOneToManyMappingVector();
         std::vector<ParameterToParameterList>::iterator vectorIterator;

         for (vectorIterator = currentParameterToParameterListVector.begin();
              vectorIterator != currentParameterToParameterListVector.end();
              ++vectorIterator)
         {
            curParamMapping = &(*vectorIterator);

            // Avoid invalid mappings.
            if( curParamMapping->IsInvalid() )
            {
               LogMappingError( *curParamMapping, "ReceiveInteraction found it to be INVALID. Ignoring." );
               continue;
            }

            // Handle special case parameter mappings.
            // Mapped name of the interaction is one such case.
            if( curParamMapping->IsSpecial() )
            {
               if( curParamMapping->GetHLAName() != PARAM_NAME_MAPPING_NAME )
               {
                  curParamMapping->SetInvalid( true );
                  LogMappingError( *curParamMapping,
                           "Mapping is marked special, but doesn't have a special HLA parameter name." );
                  continue;
               }

               const std::string& mappingName = thisInteractionToMessage->GetMappingName();

               CreateMessageParameters(
                  mappingName,       // Interaction Parameter Name
                  *curParamMapping,  // Interaction Param to Message Param Mapping Object
                  *message,          // Game Message to have parameters added
                  false,             // Do NOT add parameters that are not found
                  classHandleString  // HLA Interaction Name
                  );
            }
            // Handle the parameters as normal.
            else
            {
               RTIParameterHandleValueMap::const_iterator i, iend;
               i = theParameters.begin();
               iend = theParameters.end();
               for (; i != iend; ++i)
               {
                  RTIParameterHandle* handle = i->first;
                  if (*handle == *curParamMapping->GetParameterHandle() &&
                        ! curParamMapping->GetParameterDefinitions().empty())
                  {
                     std::string buf = i->second.mData;

                     CreateMessageParameters(
                        buf,               // Interaction Parameter Name
                        *curParamMapping,  // Interaction Param to Message Param Mapping Object
                        *message,          // Game Message to have parameters added
                        false,             // Do NOT add parameters that are not found
                        classHandleString  // HLA Interaction Name
                        );
                  }
               }
            }
         }

         message->SetSource(*mMachineInfo);
         GetGameManager()->SendMessage(*message);
      }
      catch (const RTIException&)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "RTI Error receiving interaction");
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::ObjectInstanceNameReservationSucceeded(const std::string& theObjectInstanceName)
   {
      ObjectRegQueue::iterator i, iend;
      i = mObjectRegQueue.begin();
      iend = mObjectRegQueue.end();
      bool found = false;
      while (i != iend)
      {
         if (i->first == theObjectInstanceName)
         {
            if (!found)
            {
               mRuntimeMappings.Put(theObjectInstanceName, i->second->GetAboutActorId());
            }

            DispatchNetworkMessage(*i->second);

            ObjectRegQueue::iterator oldIter = i;
            ++i;
            mObjectRegQueue.erase(oldIter);
            found = true;
         }
         else
         {
            ++i;
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::ObjectInstanceNameReservationFailed(const std::string& theObjectInstanceName)
   {
      // If the name is already taken, then this has to make up a new name based on a new unique id
      // then try again.

      ObjectRegQueue::iterator i;
      i = mObjectRegQueue.begin();

      dtCore::UniqueId replacementId;
      mRTIAmbassador->ReserveObjectInstanceName(replacementId.ToString());

      while (i != mObjectRegQueue.end())
      {
         if (i->first == theObjectInstanceName)
         {
            mObjectRegQueue.push_back(std::make_pair(replacementId.ToString(), i->second));
            ObjectRegQueue::iterator oldIter = i;
            ++i;
            mObjectRegQueue.erase(oldIter);
         }
         else
         {
            ++i;
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
    bool HLAComponent::CreateMessageParametersArray(
      const std::string& paramNameBuffer,
      const OneToManyMapping& paramToParamMapping,
      dtGame::Message& message,
      bool addMissingParams,
      const std::string& classHandleString // HLA Interaction class name
      )
   {
       bool success = true;


       const std::vector<OneToManyMapping::ParameterDefinition>& paramDefList = paramToParamMapping.GetParameterDefinitions();
       if (paramDefList.size() > 1)
       {
          mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
             "Array hla types don't yet support adding multiple parameters."
             "  Use a group or container if you want this behavior.  Message type \"%s\", class handle \"%s\"",
             message.GetMessageType().GetName().c_str(),
             classHandleString.c_str());
          success = false;
       }
       else if (!paramDefList.empty())
       {
          const OneToManyMapping::ParameterDefinition& paramDef = paramDefList[0];

          // Prepare a reference for capturing parameters.
          dtCore::RefPtr<dtCore::NamedArrayParameter> arrayParameter;
          if (addMissingParams)
          {
             arrayParameter = static_cast<dtCore::NamedArrayParameter*>(FindOrAddMessageParameter(
                paramDef.GetGameName(), dtCore::DataType::ARRAY, message));

          }
          // This is an interaction. Do not add mapped parameters that do not belong.
          else
          {
             arrayParameter = dynamic_cast<dtCore::NamedArrayParameter*>(message.GetParameter(paramDef.GetGameName()));
          }

          if (!arrayParameter.valid() )
          {
             success = false;
             mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                "Unable to add or find a array parameter for a message.  Parameter: \"%s\",  Message \"%s\", HLA Class name \"%s\"",
                paramDef.GetGameName().c_str(),
                message.GetMessageType().GetName().c_str(),
                classHandleString.c_str());
          }
          else
          {
             unsigned long remainder = paramNameBuffer.length();
             unsigned long perLength = paramToParamMapping.GetHLAType().GetEncodedLength();
             const char* bufferPtr = paramNameBuffer.c_str();

             std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParams;

             while (remainder >= perLength)
             {
                messageParams.clear();
                try
                {
                   dtCore::NamedParameter* np = arrayParameter->AddParameter(paramDef.GetGameName(), paramDef.GetGameType());
                   messageParams.push_back(np);
                }
                catch (const dtCore::InvalidParameterException& ex)
                {
                   ex.LogException(dtUtil::Log::LOG_WARNING, *mLogger);
                }

                MapToMessageParameters( bufferPtr, perLength,
                   messageParams, paramToParamMapping );

                bufferPtr += perLength;
                remainder -= perLength;
             }
          }
       }
       else
       {
          success = false;
       }
       return success;
   }
   /////////////////////////////////////////////////////////////////////////////////
   bool HLAComponent::CreateMessageParameters(
      const std::string& paramNameBuffer,
      const OneToManyMapping& paramToParamMapping,
      dtGame::Message& message,
      bool addMissingParams,
      const std::string& classHandleString // HLA Interaction class name
      )
   {
      // Initiate the state of this procedure. Mapping is successful until
      // anyone of of the parameter mappings fail.
      bool success = true;

      if (paramToParamMapping.GetIsArray())
      {
         // Do the array and return.
         success = CreateMessageParametersArray(paramNameBuffer,
               paramToParamMapping, message, addMissingParams, classHandleString);
         return success;
      }

      std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParams;
      dtCore::RefPtr<dtGame::MessageParameter> aboutParameter;
      dtCore::RefPtr<dtGame::MessageParameter> sendingParameter;

      // Prepare a reference for capturing parameters.
      dtCore::RefPtr<dtGame::MessageParameter> messageParameter;

      const std::vector<OneToManyMapping::ParameterDefinition>& paramDefList = paramToParamMapping.GetParameterDefinitions();
      for (unsigned int propnum = 0; propnum < paramDefList.size(); propnum++)
      {
         // Get the parameter Game Type and Game Name.
         const OneToManyMapping::ParameterDefinition& paramDef = paramDefList[propnum];
         dtCore::DataType& gameParameterDataType = paramDef.GetGameType();
         const std::string& gameParameterName = paramDef.GetGameName();

         // This is not an HLA mapping if an HLA name has not been specified in the mapping.
         if( gameParameterName.empty() )
         {
            continue;
         }

         // The about actor id and source actor ID are special cases.
         // We create a dummy parameter to allow the mapper code to work, and
         // then set it back to the message after the fact.
         if (gameParameterDataType == dtCore::DataType::ACTOR &&
            (gameParameterName == ABOUT_ACTOR_ID ||
            gameParameterName == SENDING_ACTOR_ID))
         {
            messageParameter = dtCore::NamedParameter::CreateFromType(
               dtCore::DataType::ACTOR, gameParameterName );

            if (gameParameterName == ABOUT_ACTOR_ID)
               aboutParameter = messageParameter;
            else
               sendingParameter = messageParameter;
         }
         else
         {
            // Find the parameter or add it if it does not exist.
            // This is usually the case for an actor update message.
            if( addMissingParams )
            {
               messageParameter = FindOrAddMessageParameter(
                  gameParameterName, gameParameterDataType, message);

               if( ! messageParameter.valid() )
               {
                  success = false;
               }
            }
            // This is an interaction. Do not add mapped parameters that do not belong.
            else
            {
               messageParameter = message.GetParameter(gameParameterName);

               if (messageParameter == NULL)
               {
                  success = false;

                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                     "No message parameter named %s found in messageType %s when trying to map from HLA interaction %s",
                     gameParameterName.c_str(),
                     message.GetMessageType().GetName().c_str(),
                     classHandleString.c_str());
               }
               else if (gameParameterDataType != messageParameter->GetDataType())
               {
                  success = false;

                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                     "Message parameter named %s found in messageType %s has unexpected type %s.  %s was expected.  Attempting to set anyway.",
                     gameParameterName.c_str(),
                     message.GetMessageType().GetName().c_str(),
                     messageParameter->GetDataType().GetName().c_str(),
                     gameParameterDataType.GetName().c_str());
               }
            }
         }

         // If a message parameter was found or created then it should be added.
         if( messageParameter.valid() )
         {
            messageParams.push_back(messageParameter);
         }

         // IMPORTANT!!! Reset the current parameter for the next iteration.
         messageParameter = NULL;
      }

      MapToMessageParameters( paramNameBuffer.c_str(), paramNameBuffer.length(),
         messageParams, paramToParamMapping );

      if (aboutParameter.valid())
      {
         message.SetAboutActorId(dtCore::UniqueId(aboutParameter->ToString()));
      }

      if (sendingParameter.valid())
      {
         message.SetSendingActorId(dtCore::UniqueId(sendingParameter->ToString()));
      }

      return success;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::DispatchDelete(const dtGame::Message& message)
   {
      // Delete HLA Object, and all Mappings referencing the Actor or the Object.
      const dtCore::UniqueId& actorId = message.GetAboutActorId();
      dtCore::RefPtr<RTIObjectInstanceHandle> thisObjectHandle = mRuntimeMappings.GetHandle(actorId);

      if (thisObjectHandle.valid())
      {
         // Removal of mappings should happen first just in case an exception
         // is thrown by the RTI ambassador.
         mRuntimeMappings.Remove(actorId);

         try
         {
            mRTIAmbassador->DeleteObjectInstance(*thisObjectHandle);
         }
         // This should not happen unless RTI is broken some how.
         catch( const RTIException& ex )
         {
            std::ostringstream ss;
            ss << "RTI::Exception (" << ex.ToString() << "):\n\t"
               << "Attempted delete on actor: \"" << actorId;
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, ss.str().c_str());
         }
      }
      else
      {
         mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                             "Ignoring actor with id \"%s\" delete because it is not published in HLA",
                             actorId.ToString().c_str());
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::DispatchUpdate(const dtGame::Message& message)
   {
      if (mRuntimeMappings.GetRTIId(message.GetAboutActorId()) == NULL)
      {
         mObjectRegQueue.push_back(std::make_pair(message.GetAboutActorId().ToString(), &message));
         mRTIAmbassador->ReserveObjectInstanceName(message.GetAboutActorId().ToString());
         return;
      }

      const dtGame::ActorUpdateMessage& aum = static_cast<const dtGame::ActorUpdateMessage&>(message);
      const dtCore::UniqueId& actorID = message.GetAboutActorId();
      const std::string* actorNamePtr = mRuntimeMappings.GetRTIId(actorID);

      // This shouldn't get here, the calling code is supposed to make sure this is set beforehand.
      if (actorNamePtr == NULL)
      {
         return;
      }

      const std::string actorName = *actorNamePtr;


      // Get Actor Type
      dtCore::RefPtr<const dtCore::ActorType> actorType = GetGameManager()->FindActorType(aum.GetActorTypeCategory(), aum.GetActorTypeName());
      if (!actorType.valid())
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Ignoring actor update for actor \"%s\" because it has a NULL actor type.",
                                actorID.ToString().c_str());
            return;
         }
      }

      // Lookup ObjectToActor from mActorToObjectMap
      dtCore::RefPtr<ObjectToActor> thisObjectToActor;

      thisObjectToActor = GetActorMapping(*actorType);

      if (!thisObjectToActor.valid())
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_INFO))
         {
            mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                                "Ignoring actor '" + actorName +
                                "' with type '" + actorType->GetFullName() +
                                "', no mapping defined for it.");
         }
         return;
      }


      //Get ClassHandle from ObjectToActor
      dtCore::RefPtr<RTIObjectClassHandle> classHandle = thisObjectToActor->GetObjectClassHandle();

      dtCore::RefPtr<RTIObjectInstanceHandle> tmpObjectHandle = mRuntimeMappings.GetHandle(actorID);
      bool newObject = !tmpObjectHandle.valid();

      dtCore::RefPtr<RTIObjectInstanceHandle> objectHandle;

      if (newObject)
      {
         try
         {
            // The actor that sent an update has not yet been registered with the RTI.
            objectHandle = mRTIAmbassador->RegisterObjectInstance(*classHandle, actorName);
         }
         catch (const RTIException& /*ex*/)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "RTI Exception trying to register a new object instance: %s",
                                 thisObjectToActor->GetObjectClassName().c_str());
            return;
         }

         mRuntimeMappings.Put(*objectHandle, actorID);
      }
      else
      {
         objectHandle = tmpObjectHandle;
      }

      RTIAttributeHandleValueMap theAttributes;
      PrepareUpdate(aum, theAttributes, *thisObjectToActor, newObject);


      if (!theAttributes.empty())
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Sending Update for actor with id " << actorID << ".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         try
         {
            mRTIAmbassador->UpdateAttributeValues(*objectHandle,
               theAttributes,
               "");
         }
         catch (const RTIException& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, ex.ToString());

         }
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::UpdateRegion(DDMRegionData& regionData)
   {

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         std::ostringstream ss;
         ss << "Updating Region: \n";
         for (unsigned i = 0; i < regionData.GetNumberOfExtents(); ++i)
         {
            const DDMRegionData::DimensionValues* dv = regionData.GetDimensionValue(i);
            if (dv == NULL)
               continue;

            ss << " Extent: \"" << dv->mName << "\" "
               << " min " << dv->mMin << " max " << dv->mMax << std::endl;
         }
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str());
      }


      dtCore::RefPtr<RTIRegion> r = regionData.GetRegion();

      if (r != NULL && mRTIAmbassador->GetNumDimensions(*r) != regionData.GetNumberOfExtents())
      {
         mRTIAmbassador->DeleteRegion(*r);
         //just to be safe.
         regionData.SetRegion(NULL);
         r = NULL;
      }



      if (r == NULL && regionData.GetNumberOfExtents() > 0)
      {
         RTIDimensionHandleSet dimHandleSet;
         for (unsigned i = 0; i < regionData.GetNumberOfExtents(); ++i)
         {
            const DDMRegionData::DimensionValues* dimension = regionData.GetDimensionValue(i);
            if (dimension == NULL)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                     "Unable to create region dimension %u because the region data object has a NULL value", i);
               continue;
            }

            try
            {
               dtCore::RefPtr<RTIDimensionHandle> dimHandle = mRTIAmbassador->GetDimensionHandle(dimension->mName);
               dimHandleSet.insert(dimHandle);
            }
            catch (const RTIException& ex)
            {
               throw RTIException("Error getting dimension handle \"" + dimension->mName + "\": " + ex.ToString(), __FILE__, __LINE__);
            }
         }

         r = mRTIAmbassador->CreateRegion(dimHandleSet);
         regionData.SetRegion(r);
      }

      if (r != NULL)
      {
         RTIDimensionVector regionDimensions;
         for (unsigned i = 0; i < regionData.GetNumberOfExtents(); ++i)
         {
            const DDMRegionData::DimensionValues* dimension = regionData.GetDimensionValue(i);
            if (dimension == NULL)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                     "Unable to create region dimension %u because the region data object has a NULL value", i);
               continue;
            }

            try
            {
               RTIDimensionData dimData;
               dimData.mDimHandle = mRTIAmbassador->GetDimensionHandle(dimension->mName);

               dimData.mMin = dimension->mMin;
               dimData.mMax = dimension->mMax;
            }
            catch (const RTIException& ex)
            {
               throw RTIException("Error getting dimension handle \"" + dimension->mName + "\": " + ex.ToString(), __FILE__, __LINE__);
            }
         }
         mRTIAmbassador->SetRegionDimensions(*r, regionDimensions);
      }

      if (regionData.GetRegion() != NULL)
      {
         try
         {
            mRTIAmbassador->CommitRegionChanges(*regionData.GetRegion());
         }
         catch (const RTIException& ex)
         {
            throw RTIException("Error updating region: " + ex.ToString(), __FILE__, __LINE__);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   DDMRegionCalculatorGroup& HLAComponent::GetDDMPublishingCalculators()
   {
      return mDDMPublishingCalculators;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const DDMRegionCalculatorGroup& HLAComponent::GetDDMPublishingCalculators() const
   {
      return mDDMPublishingCalculators;
   }

   /////////////////////////////////////////////////////////////////////////////////
   DDMRegionCalculatorGroup& HLAComponent::GetDDMSubscriptionCalculators()
   {
      return mDDMSubscriptionCalculators;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const DDMRegionCalculatorGroup& HLAComponent::GetDDMSubscriptionCalculators() const
   {
      return mDDMSubscriptionCalculators;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::GetDDMSubscriptionCalculatorRegions(std::vector<std::vector<const DDMRegionData* > >& toFill) const
   {
      toFill.resize(mDDMSubscriptionRegions.size());
      for (unsigned i = 0; i < mDDMSubscriptionRegions.size(); ++i)
      {
         const std::vector<dtCore::RefPtr<DDMRegionData> >& regionVector = mDDMSubscriptionRegions[i];
         toFill[i].resize(regionVector.size());
         std::transform(regionVector.begin(), regionVector.end(), toFill[i].begin(), dtCore::ConvertToPointerUnary<DDMRegionData>());
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::UpdateDDMSubscriptions()
   {
      for (unsigned i = 0; i < mDDMSubscriptionCalculators.GetSize(); ++i)
      {
         DDMRegionCalculator& calc = *mDDMSubscriptionCalculators[i];

         // if the region is actually changed.
         if (mDDMSubscriptionRegions.size() > i)
         {
            std::vector<dtCore::RefPtr<DDMRegionData> >& regionVector = mDDMSubscriptionRegions[i];
            for (unsigned j = 0; j < regionVector.size(); ++j)
            {
               DDMRegionData& data = *regionVector[j];
               if (calc.UpdateRegionData(data))
               {
                  RTIRegion* r = data.GetRegion();
                  UpdateRegion(data);
                  if (r != data.GetRegion())
                  {
                     // TODO subscribe with new region.
                  }
               }
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::CreateDDMSubscriptionRegions()
   {
      if (mDDMSubscriptionCalculators.IsEmpty())
      {
         throw dtUtil::Exception(
               "DDM has been enabled, but no Subscription Region Calculators have been registered: ",
               __FILE__, __LINE__);
      }

      mDDMSubscriptionRegions.resize(mDDMSubscriptionCalculators.GetSize());

      for (unsigned i = 0; i < mDDMSubscriptionCalculators.GetSize(); ++i)
      {
         try
         {
            std::vector<dtCore::RefPtr<DDMRegionData> > regionVector;
            mDDMSubscriptionCalculators[i]->CreateSubscriptionRegionData(regionVector);

            for (unsigned j = 0; j < regionVector.size(); ++j)
            {
               DDMRegionData& regionData = *regionVector[j];
               mDDMSubscriptionCalculators[i]->UpdateRegionData(regionData);
               UpdateRegion(regionData);
            }

            mDDMSubscriptionRegions[i] = regionVector;
         }
         catch (const RTIException& ex)
         {
            throw dtUtil::Exception("Error creating region: " + ex.ToString(), __FILE__, __LINE__);
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::UnsubscribeRegion(const std::string& name, RTIRegion& region)
   {
      std::multimap<std::string, dtCore::RefPtr<ObjectToActor> >::iterator objectToActorIterator
        = mObjectToActorMap.begin();

      while (objectToActorIterator != mObjectToActorMap.end())
      {
         ObjectToActor& thisObjectToActor = *(objectToActorIterator->second);

         if (thisObjectToActor.GetDDMCalculatorName() == name)
         {
            try
            {
               mRTIAmbassador->UnsubscribeObjectClass(*thisObjectToActor.GetObjectClassHandle(), &region);
            }
            catch(const RTIException& ex)
            {
               throw RTIException("Error unsubscribing to region for object class "
                     + thisObjectToActor.GetObjectClassName() + ": " + ex.ToString(), __FILE__, __LINE__);
            }
         }

         ++ objectToActorIterator;
      }

      std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> >::iterator messageToInteractionIterator
        = mMessageToInteractionMap.begin();

      while (messageToInteractionIterator != mMessageToInteractionMap.end())
      {
         InteractionToMessage& thisInteractionToMessage = *messageToInteractionIterator->second;

         if (thisInteractionToMessage.GetDDMCalculatorName() == name)
         {
            try
            {
               mRTIAmbassador->UnsubscribeInteractionClass(*thisInteractionToMessage.GetInteractionClassHandle(), &region);
            }
            catch(const RTIException& ex)
            {
               throw dtUtil::Exception("Error unsubscribing to region for object class "
                     + thisInteractionToMessage.GetInteractionName() + ": " + ex.ToString(), __FILE__, __LINE__);
            }
         }

         ++messageToInteractionIterator;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::DestroyDDMSubscriptionRegions()
   {
      for (unsigned i = 0; i < mDDMSubscriptionRegions.size(); ++i)
      {

         std::vector<dtCore::RefPtr<DDMRegionData> >& regionVector = mDDMSubscriptionRegions[i];

         for (unsigned j = 0; j < regionVector.size(); ++j)
         {
            if (regionVector[j]->GetRegion() != NULL)
            {

               try
               {
                  UnsubscribeRegion(mDDMSubscriptionCalculators[i]->GetName(), *regionVector[j]->GetRegion());
                  mRTIAmbassador->DeleteRegion(*regionVector[j]->GetRegion());
               }
               catch (const RTIException& ex)
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                       "Error deleting region, possible memory leak: %s",
                                       ex.ToString().c_str());
               }
            }
         }
      }
      mDDMSubscriptionRegions.clear();
   }

   void HLAComponent::PrepareSingleUpdateParameter(AttributeToPropertyList& curAttrToProp,
         RTIAttributeHandleValueMap& updateParams,
         const dtGame::ActorUpdateMessage& message,
         bool newObject)
   {
      const AttributeType& hlaType = curAttrToProp.GetHLAType();

      if (hlaType == AttributeType::UNKNOWN)
         return;

      std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;
      bool hasAtLeastOneNonDefaultedParameter = false;

      typedef std::vector<dtHLAGM::OneToManyMapping::ParameterDefinition> ParamDefsList;
      ParamDefsList& paramDefs = curAttrToProp.GetParameterDefinitions();
      ParamDefsList::iterator curParamDef = paramDefs.begin();
      messageParameters.reserve(paramDefs.size());

      for( unsigned i = 0; curParamDef != paramDefs.end(); ++curParamDef, ++i )
      {
         const std::string& gameName = curParamDef->GetGameName();
         dtCore::DataType& gameType = curParamDef->GetGameType();
         const std::string& defaultValue = curParamDef->GetDefaultValue();

         if (gameType == dtCore::DataType::UNKNOWN)
         {
            std::ostringstream reason;
            reason << "Parameter definition \"" << curParamDef->GetGameName() << "\" ["
                  << i << "] for mapping of HLA object attribute \""
                  << curAttrToProp.GetHLAName()
                  << "\" has \"UNKNOWN\" game data type.  Ingoring." << std::endl;
            LogMappingError( curAttrToProp, reason.str() );
            continue;
         }

         //First check for a regular parameter.
         dtCore::RefPtr<const dtGame::MessageParameter> messageParameter;

         // This maps with message parameters, so if the about actor id or sending actor id is
         // needed, it needs to be copied to a message parameter.
         if (gameType == dtCore::DataType::ACTOR &&
               gameName == ABOUT_ACTOR_ID)
         {
            dtCore::RefPtr<dtGame::MessageParameter> tmpParam =
                  dtCore::NamedParameter::CreateFromType(dtCore::DataType::ACTOR,
                        gameName);

            tmpParam->FromString(message.GetAboutActorId().ToString());
            messageParameter = tmpParam.get();
         }
         else if (gameType == dtCore::DataType::ACTOR &&
               gameName == SENDING_ACTOR_ID)
         {
            dtCore::RefPtr<dtGame::MessageParameter> tmpParam =
                  dtCore::NamedParameter::CreateFromType(dtCore::DataType::ACTOR,
                        gameName);

            tmpParam->FromString(message.GetSendingActorId().ToString());
            messageParameter = tmpParam.get();
         }
         else
         {
            messageParameter = message.GetParameter(gameName);

            if (!messageParameter.valid())
            {
               //If no regular parameter was found, check for an actor update parameter.
               messageParameter = message.GetUpdateParameter(gameName);
            }
         }

         if (messageParameter.valid())
         {
            messageParameters.push_back(messageParameter);
            hasAtLeastOneNonDefaultedParameter = true;
         }
         else if (!defaultValue.empty())
         {
            //send out the default if it's required or if it's the first time this object is
            //being updated.
            if( curAttrToProp.IsRequiredForHLA() || newObject )
               hasAtLeastOneNonDefaultedParameter = true;
            //Since the default value is in terms of the game value always, create a fake parameter
            //add it to the translation list with the proper game name, and set it to the default
            //value.
            try
            {
               dtCore::RefPtr<dtGame::MessageParameter> tmpMsgParam
               = dtCore::NamedParameter::CreateFromType(gameType, gameName);
               tmpMsgParam->FromString(defaultValue);
               messageParameters.push_back(tmpMsgParam.get());
            }
            catch (const dtUtil::Exception& ex)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                     "Error creating dummy message parameter with name \"%s\" to use for mapping a default value.  Error message should follow.",
                     gameName.c_str());
               ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
               // see the else below for a description of why this needs to happen.
               messageParameters.push_back(NULL);

               continue;
            }
         }
         else
         {
            //For mappings with many parameters, we want the indices to line up with
            //the mapping parameter definitions, so just
            //pass in NULL for missing parameters
            //A NULL will, therefore, never end up as the only parameter.
            messageParameters.push_back(NULL);
         }
      }

      if (hasAtLeastOneNonDefaultedParameter)
      {
         size_t bufferSize = 0U;
         char* buffer = nullptr;

         ParameterTranslator::AllocateBuffer(buffer, bufferSize, hlaType);

         try
         {
            MapFromMessageParameters( buffer, bufferSize, messageParameters, curAttrToProp );
            if (bufferSize > 0) // A parameter may decide not to send anything without throwing an exception
            {
               RTIContainerValueData data;
               data.mData.append(buffer, bufferSize);
               updateParams.insert( std::make_pair(curAttrToProp.GetAttributeHandle(), data) );
            }

            ParameterTranslator::DeallocateBuffer(buffer);
         }
         catch (...)
         {
            //be sure the buffer is deleted.
            if (buffer != NULL)
               ParameterTranslator::DeallocateBuffer(buffer);

            throw;
         }
      }
   }

   void HLAComponent::PrepareArrayUpdateParameter(AttributeToPropertyList& curAttrToProp,
                        RTIAttributeHandleValueMap& updateParams,
                        const dtGame::ActorUpdateMessage& message,
                        bool newObject)
   {
      const AttributeType& hlaType = curAttrToProp.GetHLAType();

      if (hlaType == AttributeType::UNKNOWN)
         return;

      typedef std::vector<dtHLAGM::OneToManyMapping::ParameterDefinition> ParamDefsList;
      ParamDefsList& paramDefs = curAttrToProp.GetParameterDefinitions();

      if (paramDefs.size() > 1)
      {
         std::ostringstream reason;
         reason << "Attribute to property has multiple parameter definitions, but it also defines it as an array.  This is not supported yet.";
         LogMappingError( curAttrToProp, reason.str() );

         curAttrToProp.SetInvalid(true);
      }

      dtHLAGM::OneToManyMapping::ParameterDefinition& curParamDef = *paramDefs.begin();

      const std::string& gameName = curParamDef.GetGameName();
      dtCore::DataType& gameType = curParamDef.GetGameType();
      const std::string& defaultValue = curParamDef.GetDefaultValue();

      if (gameType == dtCore::DataType::UNKNOWN)
      {
         std::ostringstream reason;
         reason << "Parameter definition \"" << curParamDef.GetGameName() << "\" ["
               << 0 << "] for mapping of HLA object attribute \""
               << curAttrToProp.GetHLAName()
               << "\" has \"UNKNOWN\" game data type." << std::endl;
         LogMappingError( curAttrToProp, reason.str() );
         return;
      }

      dtCore::RefPtr<const dtCore::NamedParameter> messageParameter;
      dtCore::RefPtr<const dtCore::NamedArrayParameter> arrayParameter;

      //First check for a regular parameter.
      messageParameter = message.GetParameter(gameName);

      if (!messageParameter.valid())
      {
         //If no regular parameter was found, check for an actor update parameter.
         messageParameter = message.GetUpdateParameter(gameName);
      }

      bool hasAtLeastOneNonDefaultedParameter = false;

      if (messageParameter.valid())
      {
         hasAtLeastOneNonDefaultedParameter = true;

         if (messageParameter->GetDataType() != dtCore::DataType::ARRAY)
         {
            std::ostringstream reason;
            reason << "Parameter definition \"" << curParamDef.GetGameName() << "\" ["
                  << 0 << "] for mapping of HLA object attribute \""
                  << curAttrToProp.GetHLAName()
                  << "\" should be an array of defined datatype.  If you were attempting to send only one element as a value "
                  "without storing it in an array message parameter, "
                  "then you can just make the mapping as not being an array and it will simply use the first element."
                  << std::endl;
            LogMappingError( curAttrToProp, reason.str() );
            return;
         }

         arrayParameter = static_cast<const dtCore::NamedArrayParameter*>(messageParameter.get());
      }
      else if (!defaultValue.empty())
      {
         //send out the default if it's required or if it's the first time this object is
         //being updated.
         if( curAttrToProp.IsRequiredForHLA() || newObject )
            hasAtLeastOneNonDefaultedParameter = true;
         //Since the default value is in terms of the game value always, create a fake parameter
         //add it to the translation list with the proper game name, and set it to the default
         //value.
         dtCore::RefPtr<dtCore::NamedArrayParameter> newArrayParameter = new dtCore::NamedArrayParameter(gameName);
         arrayParameter = newArrayParameter;

         try
         {
            dtCore::RefPtr<dtCore::NamedParameter> tmpMsgParam
            = dtCore::NamedParameter::CreateFromType(gameType, gameName);
            tmpMsgParam->FromString(defaultValue);
            newArrayParameter->AddParameter(*tmpMsgParam);
         }
         catch (const dtUtil::Exception& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                  "Error creating dummy message parameter with name \"%s\" to use for mapping a default value.  Error message should follow.",
                  gameName.c_str());
            ex.LogException(dtUtil::Log::LOG_ERROR, *mLogger);
         }
      }

      if (hasAtLeastOneNonDefaultedParameter && arrayParameter.valid())
      {
         size_t bufferSize = 0U;
         char* buffer = nullptr;

         ParameterTranslator::AllocateBuffer(buffer, bufferSize, hlaType, arrayParameter->GetSize());

         std::vector<dtCore::RefPtr<const dtCore::NamedParameter> > messageParameters;

         size_t curSize = hlaType.GetEncodedLength();
         char* curBuf = buffer;
         size_t totalSize = 0;

         for (size_t i = 0; i < arrayParameter->GetSize() && totalSize + curSize <= (bufferSize); ++i)
         {
            messageParameters.push_back(arrayParameter->GetParameter(i));
            try
            {
               MapFromMessageParameters( curBuf, curSize, messageParameters, curAttrToProp );

               curBuf += curSize;
               totalSize += curSize;
               curSize = hlaType.GetEncodedLength();
            }
            catch (...)
            {
               //be sure the buffer is deleted.
               if (buffer != NULL)
                  ParameterTranslator::DeallocateBuffer(buffer);

               throw;
            }
         }

         if (totalSize > 0) // A parameter may decide not to send anything without throwing an exception
         {
            RTIContainerValueData data;
            data.mData.append(buffer, totalSize);
            updateParams.insert( std::make_pair(curAttrToProp.GetAttributeHandle(), data) );
         }

         ParameterTranslator::DeallocateBuffer(buffer);
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::PrepareUpdate(const dtGame::ActorUpdateMessage& message, RTIAttributeHandleValueMap& updateParams,
      const ObjectToActor& objectToActor, bool newObject)
   {
      const dtCore::UniqueId& actorID = message.GetAboutActorId();

      bool useEntityId = !objectToActor.GetEntityIdAttributeName().empty();

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

         size_t bufferSize = 0U;
         char* buffer = nullptr;

         ParameterTranslator::AllocateBuffer(buffer, bufferSize, RPRAttributeType::ENTITY_IDENTIFIER_TYPE);

         thisEntityId.Encode(buffer);

         RTIContainerValueData data;
         data.mData.append(buffer, bufferSize);
         updateParams.insert( std::make_pair(objectToActor.GetEntityIdAttributeHandle(), data) );

         ParameterTranslator::DeallocateBuffer(buffer);
      }

      if (newObject)
      {
         //Add the RTIObjectID string.  This needs to be changed to use the proper format.
         mRuntimeMappings.Put(actorID.ToString(), actorID);
      }

      if (objectToActor.GetEntityType() != NULL)
      {
         size_t bufferSize = 0U;
         char* buffer = nullptr;

         ParameterTranslator::AllocateBuffer(buffer, bufferSize, RPRAttributeType::ENTITY_TYPE);

         objectToActor.GetEntityType()->Encode(buffer);

         RTIContainerValueData data;
         data.mData.append(buffer, bufferSize);
         updateParams.insert( std::make_pair(objectToActor.GetEntityTypeAttributeHandle(), data) );

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Sending DIS id \"" << *objectToActor.GetEntityType() << "\" for object " << objectToActor.GetObjectClassName() << ".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         ParameterTranslator::DeallocateBuffer(buffer);
      }

      //Make a loop that looks at all parameter to attribute mappings inside of ObjectToActor
      std::vector<AttributeToPropertyList> attributeToPropertyListVector = objectToActor.GetOneToManyMappingVector();

      std::vector<AttributeToPropertyList>::iterator vectorIterator;

      const std::string* attrName = NULL;

      for (vectorIterator = attributeToPropertyListVector.begin();
           vectorIterator != attributeToPropertyListVector.end();
           ++vectorIterator)
      {
         // Dereference the iterator once into a simple pointer to the current mapping.
         AttributeToPropertyList& curAttrToProp = (*vectorIterator);

         // Avoid invalid mappings.
         if( curAttrToProp.IsInvalid() )
         {
            LogMappingError( curAttrToProp, "PrepareUpdate found mapping to be INVALID. Ignoring." );
            continue;
         }

         attrName = &curAttrToProp.GetHLAName();

         // Don't map attributes with no name.
         if ( attrName->empty() )
         {
            // This is NOT an error. This happens if the attribute is only a game type with
            // no HLA mapping.
            continue;
         }

         // Avoid special-case attributes that should not be sent out.
         //
         // Attributes capturing the Entity Type (if used) should not
         // be sent since they would be redundant.
         //
         // The mapped object name should not be sent as it will not
         // work as outgoing through the RTI.
         if( curAttrToProp.IsSpecial() )
         {
            continue;
         }

         if (curAttrToProp.GetIsArray())
         {
            PrepareArrayUpdateParameter(curAttrToProp, updateParams, message, newObject);
         }
         else
         {
            PrepareSingleUpdateParameter(curAttrToProp, updateParams, message, newObject);
         }

      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::PrepareInteraction(const dtGame::Message& message, RTIParameterHandleValueMap& interactionParams,
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
         messageParameters.clear();
         bool hasAtLeastOneNonDefaultedParameter = false;

         for (unsigned i = 0; i < paramMappingItor->GetParameterDefinitions().size(); ++i)
         {
            // Avoid invalid mappings.
            if( paramMappingItor->IsInvalid() )
            {
               LogMappingError( *paramMappingItor,
                  "PrepareInteraction found mapping to be INVALID. Ignoring" );
               continue;
            }

            // Avoid sending out the mapped name of the interaction, since this parameter
            // was merely used to capture the mapped name. This parameter should be incoming-only
            // and thus should not be sent out.
            if( paramMappingItor->IsSpecial() )
            {
               continue;
            }

            const ParameterToParameterList::ParameterDefinition& pd = paramMappingItor->GetParameterDefinitions()[i];

            const std::string& gameParameterName = pd.GetGameName();
            dtCore::DataType& gameParameterType = pd.GetGameType();
            const std::string& defaultValue = pd.GetDefaultValue();

            //We map with message parameters, so if a
            if (gameParameterType == dtCore::DataType::ACTOR &&
               gameParameterName == ABOUT_ACTOR_ID)
            {
               dtCore::RefPtr<dtGame::MessageParameter> messageParameter =
                        dtCore::NamedParameter::CreateFromType(dtCore::DataType::ACTOR,
                                 gameParameterName);

               messageParameter->FromString(message.GetAboutActorId().ToString());
               messageParameters.push_back(messageParameter.get());
               hasAtLeastOneNonDefaultedParameter = true;
            }
            else if (gameParameterType == dtCore::DataType::ACTOR &&
               gameParameterName == SENDING_ACTOR_ID)
            {
               dtCore::RefPtr<dtGame::MessageParameter> messageParameter =
                        dtCore::NamedParameter::CreateFromType(dtCore::DataType::ACTOR,
                                 gameParameterName);

               messageParameter->FromString(message.GetSendingActorId().ToString());
               messageParameters.push_back(messageParameter.get());
               hasAtLeastOneNonDefaultedParameter = true;
            }
            else
            {
               dtCore::RefPtr<const dtGame::MessageParameter> messageParameter
                  = message.GetParameter(gameParameterName);

               if (messageParameter.valid())
               {
                  messageParameters.push_back(messageParameter);
                  hasAtLeastOneNonDefaultedParameter = true;
               }
               else if (!defaultValue.empty())
               {
                  if (paramMappingItor->IsRequiredForHLA())
                     hasAtLeastOneNonDefaultedParameter = true;

                  //Since the default value is in terms of the game value always, create a fake parameter
                  //add it to the translation list with the proper game name, and set it to the default
                  //value.
                  dtCore::RefPtr<dtGame::MessageParameter> tmpMsgParam
                     = dtCore::NamedParameter::CreateFromType(gameParameterType, gameParameterName);
                  tmpMsgParam->FromString(defaultValue);

                  messageParameter = tmpMsgParam.get();
                  messageParameters.push_back(messageParameter);
               }
               else
               {
                  mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                     "Message to interaction specifies a message parameter named \"%s\", but "
                     "the message of type \"%s\" has no such parameter.",
                     gameParameterName.c_str(), message.GetMessageType().GetName().c_str());

                  //For mappings with many parameters, we want the indices to line up with
                  //the mapping parameter definitions, so just
                  //pass in NULL for missing parameters
                  //A NULL will, therefore, never end up as the only parameter.
                  messageParameters.push_back(NULL);
               }
            }

         }

         if (hasAtLeastOneNonDefaultedParameter)
         {
            const AttributeType& hlaType = paramMappingItor->GetHLAType();

            size_t bufferSize = 0U;
            char* buffer = nullptr;

            ParameterTranslator::AllocateBuffer(buffer, bufferSize, hlaType);

            try
            {
               //Supporting mapping multiple message parameters to a single interaction parameter
               //is in the works.  This vector always only has one item in it for now.
               MapFromMessageParameters(buffer, bufferSize, messageParameters, *paramMappingItor);
               if(bufferSize > 0)
               {
                  RTIContainerValueData data;
                  data.mData.append(buffer, bufferSize);
                  interactionParams.insert( std::make_pair(paramMappingItor->GetParameterHandle(), data));
               }

               ParameterTranslator::DeallocateBuffer(buffer);
            }
            catch (...)
            {
               //be sure the buffer is deleted.
               if (buffer != NULL)
                  ParameterTranslator::DeallocateBuffer(buffer);
               throw;
            }

         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
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
      RTIParameterHandleValueMap interactionParams;

      PrepareInteraction(message, interactionParams, *interactionToMessage);

      if (interactionParams.size() > 0)
      {
         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            std::ostringstream ss;
            ss << "Sending interaction of type \"" << interactionToMessage->GetInteractionName() << "\" for message type \""
               << message.GetMessageType() << "\".";
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, ss.str().c_str());
         }

         mRTIAmbassador->SendInteraction(*interactionToMessage->GetInteractionClassHandle(),
                                         interactionParams,
                                         "");
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::DispatchNetworkMessage(const dtGame::Message& message)
   {
      if (mExecutionName != "")
      {
         try
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
         catch(const RTIException& ex)
         {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                 "RTI Exception thrown during network message dispatch! [%s]",
                                  ex.ToString().c_str());

         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::ProcessMessage(const dtGame::Message& message)
   {
      if (message.GetMessageType() == dtGame::MessageType::TICK_LOCAL)
      {
         if (mRTIAmbassador != NULL)
         {
            try
            {
               if (IsDDMEnabled())
               {
                  UpdateDDMSubscriptions();
               }
               mRTIAmbassador->Tick();
            }
            catch(const RTIException& ex)
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
                                    "Exception thrown during rti tick! [%s]",
                                     ex.ToString().c_str());

            }
         }
      }
      else if (message.GetMessageType() == dtGame::MessageType::INFO_MAP_UNLOADED)
      {
         mRuntimeMappings.Clear();
         mObjectRegQueue.clear();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::SetDefaultParameters( std::vector<AttributeToPropertyList>::iterator vectorIterator,
                                             bool bNewObject, dtGame::Message *msg )
   {
      for (unsigned int propnum = 0; propnum < vectorIterator->GetParameterDefinitions().size(); propnum++)
      {
         OneToManyMapping::ParameterDefinition& paramDef = vectorIterator->GetParameterDefinitions()[propnum];
         if (bNewObject || paramDef.IsRequiredForGame())
         {
            const std::string& propertyString = paramDef.GetGameName();
            const std::string& defaultValue = paramDef.GetDefaultValue();
            if (!defaultValue.empty() && !propertyString.empty())
            {
               dtCore::DataType& propertyDataType = paramDef.GetGameType();

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

   /////////////////////////////////////////////////////////////////////////////////
   void HLAComponent::LogMappingError( const dtHLAGM::OneToManyMapping& mapping, const std::string& reason )
   {
      // Determine which of two mapping types the object is.
      const dtHLAGM::ParameterToParameterList* paramMappingType
         = dynamic_cast<const dtHLAGM::ParameterToParameterList*>(&mapping);

      // Get the target game types for the mapping.
      typedef std::vector<dtHLAGM::OneToManyMapping::ParameterDefinition> ParamDefsList;
      const ParamDefsList& paramDefs = mapping.GetParameterDefinitions();

      // Construct the error message.
      std::ostringstream oss;
      oss << (paramMappingType != NULL ? "HLA Interaction Parameter-To-Parameter" : "HLA Object Attribute-To-Property")
         << " mapping between\nHLA type \"" << mapping.GetHLAName().c_str()
            << "\" (" << mapping.GetHLAType().GetName().c_str() << ")";

      // --- Report the Game type mappings if they exist.
      if( ! paramDefs.empty() )
      {
         ParamDefsList::const_iterator curParamDef = paramDefs.begin();
         for( ; curParamDef != paramDefs.end(); ++curParamDef )
         {
            oss << "\nto Game type \"" << curParamDef->GetGameName().c_str()
                   << "\" (" << curParamDef->GetGameType().GetName().c_str() << ")";
         }
      }
      else
      {
         oss << "\nwith NO Game types";
      }

      // --- Finish the message with the reason.
      oss << "\nis in error for the following reason:\n\t"
         << reason.c_str() << std::endl;

      // Output the message.
      mLogger->LogMessage( dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, oss.str() );
   }

}

