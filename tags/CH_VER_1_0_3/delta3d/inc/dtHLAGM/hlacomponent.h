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

#ifndef DELTA_HLA_COMPONENT
#define DELTA_HLA_COMPONENT

//
// hlacomponent.h: Declaration of the HLACOMPONENT class.
//
///////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <map>
#include <osg/Matrix>
#include <dtUtil/coordinates.h>
#include <dtUtil/log.h>
#include <dtDAL/actortype.h>
#include <dtGame/messagetype.h>
#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>
#include <dtGame/gmcomponent.h>

#include "dtHLAGM/export.h"

#define RTI_USES_STD_FSTREAM
#include "RTI.hh"
#include "NullFederateAmbassador.hh"

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

namespace dtHLAGM
{
   class ObjectToActor;
   class AttributeToProperty;
   class AttributeType;
   class EntityType;
   class InteractionToMessage;
   class ParameterToParameter;
   class OneToOneMapping;
    
   class DT_HLAGM_EXPORT HLAComponent : public dtGame::GMComponent,
      public NullFederateAmbassador
   {
      public:
     
         HLAComponent();
                  
         /**
          * Creates/joins a federation execution.
          *
          * @param executionName the name of the federation execution to join
          * @param fedFilename the fed filename
          * @param federateName the name of this federate
          */
         void JoinFederationExecution(std::string executionName = "dtCore",
                                      std::string fedFilename = "jntc.fed",
                                      std::string federateName = "Participant");
         
         /**
          * Leaves/destroys the joined execution.
          */
         void LeaveFederationExecution();
         
         
         /**
          * Sets the DIS/RPR-FOM site identifier.
          *
          * @param siteIdentifier the new site identifier
          */
         void SetSiteIdentifier(unsigned short siteIdentifier);
         
         /**
          * Returns the DIS/RPR-FOM site identifier.
          *
          * @return the site identifier
          */
         unsigned short GetSiteIdentifier() const;
         
         /**
          * Sets the DIS/RPR-FOM application identifier.
          *
          * @param applicationIdentifier the new application identifier
          */
         void SetApplicationIdentifier(unsigned short applicationIdentifier);
         
         /**
          * Returns the DIS/RPR-FOM application identifier.
          *
          * @return the DIS/RPR-FOM application identifier
          */
         unsigned short GetApplicationIdentifier() const;
         
         virtual void discoverObjectInstance(RTI::ObjectHandle theObject,
                                             RTI::ObjectClassHandle theObjectClassHandle,
                                             const char* theObjectName)
            throw (RTI::CouldNotDiscover,
                   RTI::ObjectClassNotKnown,
                   RTI::FederateInternalError);
         
         /**
          * Invoked by the RTI ambassador to request that the federate provide
          * updated attribute values for the specified object.
          *
          * @param theObject the handle of the object of interest
          * @param theAttributes the set of attributes to update
          */
         virtual void provideAttributeValueUpdate(RTI::ObjectHandle theObject,
                                                  const RTI::AttributeHandleSet& theAttributes)
            throw (RTI::ObjectNotKnown,
                   RTI::AttributeNotKnown,
                   RTI::AttributeNotOwned,
                   RTI::FederateInternalError);
         
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
         virtual void reflectAttributeValues(RTI::ObjectHandle theObject,
                                             const RTI::AttributeHandleValuePairSet& theAttributes,
                                             const RTI::FedTime& theTime,
                                             const char *theTag,
                                             RTI::EventRetractionHandle theHandle)
            throw (RTI::ObjectNotKnown,
                   RTI::AttributeNotKnown,
                   RTI::FederateOwnsAttributes,
                   RTI::InvalidFederationTime,
                   RTI::FederateInternalError);
         
         /**
          * Invoked by the RTI ambassador to notify the federate of updated object
          * attribute values.
          *
          * @param theObject the handle of the modified object
          * @param theAttributes the new attribute values
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void reflectAttributeValues(RTI::ObjectHandle theObject,
                                             const RTI::AttributeHandleValuePairSet& theAttributes,
                                             const char *theTag)
            throw (RTI::ObjectNotKnown,
                   RTI::AttributeNotKnown,
                   RTI::FederateOwnsAttributes,
                   RTI::FederateInternalError);
         
         /**
          * Invoked by the RTI ambassador to notify the federate of a deleted object
          * instance.
          *
          * @param theObject the handle of the removed object
          * @param theTime the event timestamp
          * @param theTag the user-supplied tag associated with the event
          * @param theHandle the event retraction handle
          */
         virtual void removeObjectInstance(RTI::ObjectHandle theObject,
                                           const RTI::FedTime& theTime,
                                           const char *theTag,
                                           RTI::EventRetractionHandle theHandle)
            throw (RTI::ObjectNotKnown,
                   RTI::InvalidFederationTime,
                   RTI::FederateInternalError);
         
         /**
          * Invoked by the RTI ambassador to notify the federate of a removed object
          * instance.
          *
          * @param theObject the handle of the removed object
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void removeObjectInstance(RTI::ObjectHandle theObject,
                                           const char *theTag)
            throw (RTI::ObjectNotKnown,
                   RTI::FederateInternalError);
         
         /**
          * Invoked by the RTI ambassador to notify the federate of a received
          * interaction.
          *
          * @param theInteraction the handle of the received interaction
          * @param theParameters the parameters of the interaction
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void receiveInteraction(RTI::InteractionClassHandle,
                                         const RTI::ParameterHandleValuePairSet& theParameters,
                                         const char *theTag)
            throw (RTI::InteractionClassNotKnown,
                   RTI::InteractionParameterNotKnown,
                   RTI::FederateInternalError);
         
         const ObjectToActor* GetActorMapping(dtDAL::ActorType &type) const;
         
         const ObjectToActor* GetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID) const;
         
         /**
          * Called to Register an Object to Actor mapping.
          *
          * @param type the ActorType
          * @param objTypeName the name of the object
          * @param thisDisID the DIS ID of the object
          * @param oneToOneActorVector a vector of Attribute to Property mappings for this Object to Actor mapping
          * @param remoteOnly true if this mapping should only apply to remotely simulate objects.  Otherwise
          *        it will apply to mappings in both directions.  Only one mapping per actor type may be non-remote. 
          */
         void RegisterActorMapping(dtDAL::ActorType &type,
                                   const std::string& objTypeName,
                                   const EntityType* thisDisID,
                                   std::vector<AttributeToProperty> &oneToOneActorVector,
                                   bool remoteOnly = false);

         /**
          * Called to Register an Object to Actor mapping.
          *
          * @param objectToActor the mapping object register.
          * @note Only one mapping per actor type may be have the RemoteOnly property set to false. 
          */
         void RegisterActorMapping(ObjectToActor& objectToActor);
         
         /**
          * Called to Unregister an Object to Actor mapping
          *
          * @param type the ActorType of the mapping to be removed
          */
         void UnregisterActorMapping(dtDAL::ActorType &type);
         
         /**
          * Called to Unregister an Object to Actor Mapping
          *
          * @param objTypName the object name of the mapping to be removed
          * @param thisDisID the dis id of the object type mapping or NULL if it's not used.
          */
         void UnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisDisID);
         
         const InteractionToMessage* GetMessageMapping(const dtGame::MessageType &type) const;

         const InteractionToMessage* GetInteractionMapping(const std::string& interName) const;
         
         void RegisterMessageMapping(InteractionToMessage& interactionToMessage);

         /**
          * Called to Register an Interaction to Message mapping
          *
          * @param type the MessageType
          * @param interactionTypeName the name of the interaction
          * @param oneToOneMessageMapping the vector of ParameterToParameter mappings
          */
         void RegisterMessageMapping(const dtGame::MessageType &type,
                                     const std::string& interactionTypeName,
                                     std::vector<ParameterToParameter> &oneToOneMessageVector);
         
         /**
          * Called to Unregister an Interaction to Message mapping
          *
          * @param type the MessageType of the mapping to be removed
          */
         void UnregisterMessageMapping(const dtGame::MessageType &type);
         
         /**
          * Called to Unregister an Interaction to Message mapping
          *
          * @param interName the interaction name for the mapping to be removed
          */
         void UnregisterInteractionMapping(const std::string& interName);
         
         void RegisterMessageTranslator();
         
         void UnregisterMessageTranslator();
         
         void RegisterInteractionTranslator();
         
         void UnregisterInteractionTranslator();
         
         void ClearConfiguration();
         
         void UpdateFromActor();
         
         /**
          * Overridden to receive messages for published actors so that any actors or messages
          * defined in the configuration can be translated into HLA.
          */
         virtual void SendMessage(const dtGame::Message& message);

         /**
          * Overridden to handle tick messages.
          */
         virtual void ProcessMessage(const dtGame::Message& message);
         
         /**
          * Sets the location of the origin of the terrain to use for coordinate adjustment.
          * @param location the origin location
          */
         void SetOriginLocation(const osg::Vec3d& location);
         
         /**
          * @return the location of the origin.
          */
         const osg::Vec3d GetOriginLocation();
     
         void SetGeoOrigin(double lat, double lon, double elevation);
    
         /**
          * Sets the rotation of the origin relative to geocentric coordinates.
          *
          * @param rotation Rotation as h,p,r in degrees.
          */
         void SetOriginRotation(const osg::Vec3& rotation);
        
         /**
          * Retrieves the rotation of the origin relative to geocentric coordinates.
          * @return a vector is the format heading, pitch, roll in degrees
          */
         const osg::Vec3 GetOriginRotation() const;
         
         dtUtil::Coordinates& GetCoordinateConverter() { return mCoordinates; };
         const dtUtil::Coordinates& GetCoordinateConverter() const { return mCoordinates; };
         
      protected:
         
         /**
          * Maps a value in a buffer from HLA to a message parameter.
          * @param mapping the mapping object for the given field.
          * @param buffer the buffer of data bytes that holds the value.
          * @param parameter the message parameter to set.
          */
         void MapHLAFieldToMessageParameter(const OneToOneMapping& mapping, const char* buffer, dtGame::MessageParameter& parameter);
         
         /**
          * Sends out the message required to delete an actor.
          */
         void DeleteActor(const dtCore::UniqueId& toDelete);
         
         virtual ~HLAComponent()
           throw (RTI::FederateInternalError);
         
      private:
         
         ObjectToActor* GetBestObjectToActor(RTI::ObjectHandle theObject,
                                                   const RTI::AttributeHandleValuePairSet& theAttributes);
         
         void SetIntegerValue(long value, dtGame::MessageParameter& parameter, const OneToOneMapping& mapping);
         dtGame::MessageParameter* FindOrAddMessageParameter(const std::string& name, const dtDAL::DataType& type, dtGame::Message& msg);

         /**
          * The RTI ambassador.
          */
         RTI::RTIambassador* mRTIAmbassador;
         //NullFederateAmbassador mRTIAmbassador;
         
         /**
          * The named of the joined execution.
          */
         std::string mExecutionName;
         
         /**
          * The entity identifier attribute handle.
          */
         RTI::AttributeHandle mEntityIdentifierAttributeHandle;
                  
         /**
          * The IP address of the local machine.          
          */
         unsigned int mLocalIPAddress;
         
         /**
          * The DIS/RPR-FOM site identifier.
          */
         unsigned short mSiteIdentifier;
         
         /**
          * The DIS/RPR-FOM application identifier.
          */
         unsigned short mApplicationIdentifier;
    
         unsigned short mEntityIdentifierCounter;
         unsigned short mEventIdentifierCounter;
         dtUtil::Coordinates mCoordinates;
         std::map<dtCore::RefPtr<dtDAL::ActorType>, dtCore::RefPtr<ObjectToActor> > mActorToObjectMap;
         std::multimap<std::string, dtCore::RefPtr<ObjectToActor> > mObjectToActorMap;
         
         std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> > mMessageToInteractionMap;
         std::map<std::string, dtCore::RefPtr<InteractionToMessage> > mInteractionToMessageMap;
         
         std::map<RTI::ObjectHandle, dtCore::UniqueId> mHLAtoActorMap;
         std::map<dtCore::UniqueId, RTI::ObjectHandle> mActorToHLAMap;
         std::map<dtCore::UniqueId, unsigned short> mUniqueIdToEntityIdentifierMap;
         std::map<RTI::ObjectHandle, dtCore::RefPtr<ObjectToActor> > mObjectHandleToClassMap;
         
         ///Does the work of unregistering, see UnregisterActorMapping
         dtCore::RefPtr<ObjectToActor> InternalUnregisterActorMapping(dtDAL::ActorType &type);
         ///Does the work of unregistering, see UnregisterObjectMapping
         dtCore::RefPtr<ObjectToActor> InternalUnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisDisID);
         ///Does the work of unregistering, see UnregisterMessageMapping
         dtCore::RefPtr<InteractionToMessage> InternalUnregisterMessageMapping(const dtGame::MessageType &type);
         ///Does the work of unregistering, see UnregisterInteractionMapping
         dtCore::RefPtr<InteractionToMessage> InternalUnregisterInteractionMapping(const std::string& interName);
         
         dtCore::RefPtr<dtGame::MachineInfo> mMachineInfo;
         
         dtCore::RefPtr<dtUtil::Log> mLogger;
         
   };
   
};
#endif // DELTA_HLA_FOM_TRANSLATOR