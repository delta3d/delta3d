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

#ifndef DELTA_HLA_COMPONENT
#define DELTA_HLA_COMPONENT

//
// hlacomponent.h: Declaration of the HLACOMPONENT class.
//
///////////////////////////////////////////////////////////////////////

#include <dtHLAGM/rtiambassador.h>
#include <dtHLAGM/rtifederateambassador.h>

#ifdef DELTA_WIN32
   #undef GetClassName
   #undef SendMessage
   #undef CreateFont
   #undef GetTimeFormat
   #undef FindResource //due to some windows.h include which conflicts with dtTerrain::FindResource()
#endif

#include <string>
#include <vector>
#include <map>
#include <list>
#include <osg/Matrix>

#include <dtUtil/coordinates.h>

#include <dtCore/refptr.h>

#include <dtGame/messagetype.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/messageparameter.h>
#include <dtHLAGM/export.h>
#include <dtHLAGM/objectruntimemappinginfo.h>
#include <dtHLAGM/ddmregioncalculatorgroup.h>
#include <dtHLAGM/rtihandle.h>
#include <dtHLAGM/rtiregion.h>
#include <dtHLAGM/rtiambassador.h>

namespace dtCore
{
   class UniqueId;
}

namespace dtCore
{
   class ActorType;
}

namespace dtGame
{
   class ActorUpdateMessage;
}


namespace dtHLAGM
{
   class ObjectToActor;
   class AttributeToPropertyList;
   class AttributeType;
   class EntityType;
   class EntityIdentifier;
   class InteractionToMessage;
   class ParameterToParameterList;
   class OneToManyMapping;
   class ParameterTranslator;
   class DDMRegionCalculatorGroup;
   class DDMRegionCalculator;
   class DDMRegionData;
   
   class DT_HLAGM_EXPORT HLAComponent : public dtGame::GMComponent,
      public RTIFederateAmbassador
   {
      public:
         // Name of the HLA mapping object/interaction attribute that
         // should map the defined object/interaction name into an
         // actor property. A mapping name is the value as found in
         // the Object tag Name attribute within an HLA mapping file.
         // Example (Mapping Name for Object):
         //         <object name="mapping name">
         //
         // Example (Mapping Name for Object):
         //         <interaction name="mapping name">
         //
         // Example (Attribute Name for Objects and Interactions):
         //         <hlaName>Attribute Name</hlaName>
         //
         static const std::string PARAM_NAME_MAPPING_NAME; // Interactions (have parameters)
         static const std::string ATTR_NAME_MAPPING_NAME;  // Objects (have attributes)

         // Reserved HLA object attribute name to capture the incoming Entity Type.
         // --- This constant will refer to the HLA Entity Type Attribute Name set on this component.
         static const std::string ATTR_NAME_ENTITY_TYPE; // Objects
         // --- Default attribute name if one is not specified in mapping.
         static const std::string ATTR_NAME_ENTITY_TYPE_DEFAULT;

         //Constant for the name of the about actor id property on a message.
         //It's used to map data to it like a message parameter in the mapping.
         static const std::string ABOUT_ACTOR_ID;
         ///Constant for the name of the source actor id property on a message.
         //It's used to map data to it like a message parameter in the mapping.
         static const std::string SENDING_ACTOR_ID;


         static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;
         // Constant used for the component name so this component can be
         // successfully looked up in the Game Manager by name
         static const std::string DEFAULT_NAME;

         HLAComponent(dtCore::SystemComponentType& type = *TYPE);

         /**
          * Creates/joins a federation execution.
          *
          * @param executionName the name of the federation execution to join
          * @param fedFilenames a vector of fed file names.
          * @param federateName the name of this federate
          */
         void JoinFederationExecution(const std::string& executionName,
                                      const std::vector<std::string>& fedFilenames,
                                      const std::string& federateName = "Participant",
                                      const std::string& ridFile = "RTI.rid",
                                      const std::string& rtiImplementationName = RTIAmbassador::RTI13_IMPLEMENTATION
                                      );

         inline void JoinFederationExecution(const std::string& executionName = "dtCore",
                                      const std::string& fedFilename = "jntc.fed",
                                      const std::string& federateName = "Participant",
                                      const std::string& ridFile = "RTI.rid",
                                      const std::string& rtiImplementationName = RTIAmbassador::RTI13_IMPLEMENTATION
                                      )
         {
            std::vector<std::string> fedFilenames;
            fedFilenames.push_back(fedFilename);
            JoinFederationExecution(executionName, fedFilenames, federateName, ridFile, rtiImplementationName);
         }

         /**
          * Leaves/destroys the joined execution.
          */
         void LeaveFederationExecution();


         /// @return true if the HLA component has joined the federation
         bool IsConnectedToFederation() const { return mRTIAmbassador != NULL; }

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

         /// @return true if DDM is enabled for this hla component;
         bool IsDDMEnabled() const { return mDDMEnabled; }
         
         /** 
          * Sets DDM to be enabled or disabled
          * @param enable whether to enable or disable DDM
          * @throws dtUtil::Exception if component is connected to a federation.
          */
         void SetDDMEnabled(bool enable);
         
         virtual void DiscoverObjectInstance(RTIObjectInstanceHandle& theObject,
                                             RTIObjectClassHandle& theObjectClassHandle,
                                             const std::string& theObjectName);

         /**
          * Invoked by the RTI ambassador to request that the federate provide
          * updated attribute values for the specified object.
          *
          * @param theObject the handle of the object of interest
          * @param theAttributes the set of attributes to update
          */
         virtual void ProvideAttributeValueUpdate(RTIObjectInstanceHandle& theObject,
                                                  const RTIAttributeHandleSet& theAttributes);

         /**
          * Invoked by the RTI ambassador to notify the federate of updated object
          * attribute values.
          *
          * @param theObject the handle of the modified object
          * @param theAttributes the new attribute values
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void ReflectAttributeValues(RTIObjectInstanceHandle& theObject,
                                             const RTIAttributeHandleValueMap& theAttributes,
                                             const std::string& theTag);


         /**
          * Invoked by the RTI ambassador to notify the federate of a removed object
          * instance.
          *
          * @param theObject the handle of the removed object
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void RemoveObjectInstance(RTIObjectInstanceHandle& theObject,
                                           const std::string& theTag);

         /**
          * Invoked by the RTI ambassador to notify the federate of a received
          * interaction.
          *
          * @param theInteraction the handle of the received interaction
          * @param theParameters the parameters of the interaction
          * @param theTag the user-supplied tag associated with the event
          */
         virtual void ReceiveInteraction(RTIInteractionClassHandle& interactionClassHandle,
                                         const RTIParameterHandleValueMap& theParameters,
                                         const std::string& theTag);

         virtual void ObjectInstanceNameReservationSucceeded(const std::string& theObjectInstanceName);

         virtual void ObjectInstanceNameReservationFailed(const std::string& theObjectInstanceName);

         const ObjectToActor* GetActorMapping(const dtCore::ActorType &type) const;
         ObjectToActor* GetActorMapping(const dtCore::ActorType &type);

         const ObjectToActor* GetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID) const;
         ObjectToActor* GetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID);

         ///Fills a vector with all object to actor mappings currently registered.
         void GetAllObjectToActorMappings(std::vector<ObjectToActor*>& toFill);

         ///Fills a vector with all const object to actor mappings.
         void GetAllObjectToActorMappings(std::vector<const ObjectToActor*>& toFill) const;

         ///Fills a vector with all interaction to message mappings.
         void GetAllInteractionToMessageMappings(std::vector<InteractionToMessage*>& toFill);

         ///Fills a vector with all const interaction to message mappings.
         void GetAllInteractionToMessageMappings(std::vector<const InteractionToMessage*>& toFill) const;

         /**
          * Called to Register an Object to Actor mapping.
          *
          * @param type the ActorType
          * @param objTypeName the name of the object
          * @param thisDisID the DIS ID of the object
          * @param oneToOneActorVector a vector of Attribute to Property mappings for this Object to Actor mapping
          * @param localOrRemote If this mapping should only apply to remotely or locally simulated objects, or both.
          *                      It defaults to both.
          */
         void RegisterActorMapping(dtCore::ActorType &type,
                                   const std::string& objTypeName,
                                   const EntityType* thisDisID,
                                   std::vector<AttributeToPropertyList> &oneToOneActorVector,
                                   ObjectToActor::LocalOrRemoteType& localOrRemote = ObjectToActor::LocalOrRemoteType::LOCAL_AND_REMOTE);

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
         void UnregisterActorMapping(dtCore::ActorType &type);

         /**
          * Called to Unregister an Object to Actor Mapping
          *
          * @param objTypName the object name of the mapping to be removed
          * @param thisDisID the dis id of the object type mapping or NULL if it's not used.
          */
         void UnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisDisID);

         const InteractionToMessage* GetMessageMapping(const dtGame::MessageType &type) const;
         InteractionToMessage* GetMessageMapping(const dtGame::MessageType &type);

         const InteractionToMessage* GetInteractionMapping(const std::string& interName) const;
         InteractionToMessage* GetInteractionMapping(const std::string& interName);

         void RegisterMessageMapping(InteractionToMessage& interactionToMessage);

         /**
          * Called to Register an Interaction to Message mapping
          *
          * @param type the MessageType
          * @param interactionTypeName the name of the interaction
          * @param oneToOneMessageMapping the vector of ParameterToParameterList mappings
          */
         void RegisterMessageMapping(const dtGame::MessageType &type,
                                     const std::string& interactionTypeName,
                                     std::vector<ParameterToParameterList> &oneToOneMessageVector);

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

         /**
          * Clears all of the mapping configuration.
          * @throws dtUtil::Exception if component is connected to a federation.
          */
         void ClearConfiguration();

         void UpdateFromActor();

         /**
          * Overridden to receive messages for published actors so that any actors or messages
          * defined in the configuration can be translated into HLA.
          */
         virtual void DispatchNetworkMessage(const dtGame::Message& message);

         /**
          * Overridden to handle tick messages.
          */
         virtual void ProcessMessage(const dtGame::Message& message);

         dtUtil::Coordinates& GetCoordinateConverter() { return mCoordinates; };
         const dtUtil::Coordinates& GetCoordinateConverter() const { return mCoordinates; };

         ///@return the list of parameter translators used by this component.
         const std::vector<dtCore::RefPtr<ParameterTranslator> >& GetParameterTranslators() const { return mParameterTranslators; }

         /// Adds a new custom parameter translator to the hla component.
         void AddParameterTranslator(ParameterTranslator& newTranslator);

         /// @return The DDMRegionCalculators used for publishing.
         DDMRegionCalculatorGroup& GetDDMPublishingCalculators();
         /// @return The DDMRegionCalculators used for publishing as const.
         const DDMRegionCalculatorGroup& GetDDMPublishingCalculators() const;

         /// @return The DDMRegionCalculators used for subscription.
         DDMRegionCalculatorGroup& GetDDMSubscriptionCalculators();
         /// @return The DDMRegionCalculators used for subscription as const.
         const DDMRegionCalculatorGroup& GetDDMSubscriptionCalculators() const;

         /// @return The region data object for each subscription calculator.
         void GetDDMSubscriptionCalculatorRegions(std::vector<std::vector<const DDMRegionData*> >& toFill) const;
         
         ///@return the current RTIambassador instance.  This will return NULL if this component is not connected to the RTI.
         RTIAmbassador* GetRTIAmbassador() { return mRTIAmbassador; }
         const RTIAmbassador* GetRTIAmbassador() const { return mRTIAmbassador; }

         /**
          * Set the name of the HLA object attribute that is to be used as
          * the Entity Type identifier.  This is the default, but mappings can define others.
          * @param name Name of the HLA attribute being used as the Entity Type.
          */
         void SetHLAEntityTypeAttributeName( const std::string& name );

         /**
          * Get the name of the HLA object attribute that is used as the Entity Type.
          * This is the default, but mappings can define others.
          * @return name Name of the HLA attribute being used as the Entity Type.
          */
         const std::string& GetHLAEntityTypeAttributeName() const;

         /**
          * If the passed in attribute name matches either the value of HLAEntityTypeAttributeName
          * or one of the overrides in one of the object to actor mappings, then the attribute will be handled
          * as an entity type.
          */
         bool IsEntityTypeAttribute(const std::string& attribName) const;

      protected:

         /// Calls all of the subscription calculators to update their regions.
         void UpdateDDMSubscriptions();
         void CreateDDMSubscriptionRegions();
         void DestroyDDMSubscriptionRegions();

         void UpdateRegion(DDMRegionData& regionData);

         /**
          * Prepares the interaction parameters for an interaction.  This may be overridden in a subclass
          * to do one-off translations of outgoing data.
          * @param message the message that holds the data to be translated.
          * @param interactionParams a ParameterHandleValuePairSet to be filled with parameters.
          * @param interactionToMessage the mapping object specifying the mapping information.
          */
         virtual void PrepareInteraction(const dtGame::Message& message,
            RTIParameterHandleValueMap& interactionParams,
            const InteractionToMessage& interactionToMessage);

         /**
          * Prepares the attributes for an hla object update.  This may be overridden in a subclass
          * to do one-off translations of outgoing data.
          * @param message the message that holds the data to be translated.
          * @param updateParams an attributeHandleValuePairSet to be filled with attributes.
          * @param objectToActor the mapping object specifying the mapping information.
          * @param newObject true if this is the first time attributes are being send for the object,
          *                  which means defaults should be sent for data not in the message.
          */
         virtual void PrepareUpdate(const dtGame::ActorUpdateMessage& message, 
            RTIAttributeHandleValueMap& updateParams,
            const ObjectToActor& objectToActor, bool newObject);

         /**
          * Maps a value in a buffer from HLA to a message parameter.
          * @param mapping the mapping object for the given field.
          * @param buffer the buffer of data bytes that holds the value.
          * @param parameter the message parameter to set.
          */
         const ObjectToActor* InternalGetObjectMapping(const std::string& objTypeName, const EntityType* thisDisID) const;

         /**
          * Called by DispatchNetworkMessage if the message received is to delete an actor.
          */
         void DispatchDelete(const dtGame::Message& message);

         /**
          * Called by DispatchNetworkMessage if the message received is to update or create an actor.
          */
         void DispatchUpdate(const dtGame::Message& message);

         /**
          * Called by DispatchNetworkMessage when a message other than an actor lifecycle message is passed in.
          */
         void DispatchInteraction(const dtGame::Message& message);

         /**
          * Sends out the message required to delete an actor.
          */
         void DeleteActor(const dtCore::UniqueId& toDelete);

         ObjectRuntimeMappingInfo& GetRuntimeMappingInfo() { return mRuntimeMappings; }

         void PublishSubscribe();
         void UnsubscribeRegion(const std::string& name, RTIRegion& region);

         /**
          * Output an error message about a mapping that describes the mapping
          * arrangement and the reason that it is being reported in an error.
          * @param mapping Attribute-To-Property or Parameter-To-Parameter mapping
          *        that has caused an error or is invalid.
          * @param reason Message describing why the mapping is being logged in an error.
          */
         void LogMappingError( const dtHLAGM::OneToManyMapping& mapping, const std::string& reason );

         virtual ~HLAComponent();

      private:

         ObjectToActor* GetBestObjectToActor(RTIObjectInstanceHandle& theObject,
                                                   const RTIAttributeHandleValueMap& theAttributes,
                                                   bool& hadEntityTypeProperty);

         dtGame::MessageParameter* FindOrAddMessageParameter(const std::string& name, dtCore::DataType& type, dtGame::Message& msg);

         //called when connecting to the RTI to subscribe/publish all the attributes and objects in the given mapping.
         void RegisterObjectToActorWithRTI(ObjectToActor& objectToActor);

         //called when connecting to the RTI to subscribe/publish all the parameters and interactions in the given mapping.
         void RegisterInteractionToMessageWithRTI(InteractionToMessage& interactionToMessage);

         const ParameterTranslator* FindTranslatorForAttributeType(const AttributeType& type) const;

         void MapToMessageParameters(const char* buffer,
                                     size_t size,
                                     std::vector<dtCore::RefPtr<dtGame::MessageParameter> >& parameters,
                                     const OneToManyMapping& mapping) const;

         void MapFromMessageParameters(char* buffer,
                                       size_t& maxSize,
                                       std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& parameters,
                                       const OneToManyMapping& mapping) const;


         bool DoGetBestObjectToActor( dtCore::RefPtr<ObjectToActor>& bestObjectToActor,
                                    RTIObjectInstanceHandle& theObject,
                                    const RTIAttributeHandleValueMap& theAttributes,
                                    const dtCore::UniqueId* currentActorId );


         void AddActorIDToMap( const RTIAttributeHandleValueMap& theAttributes,
                              dtCore::RefPtr<ObjectToActor> bestObjectToActor,
                              const dtCore::UniqueId* currentActorId );


         void SetDefaultParameters( std::vector<AttributeToPropertyList>::iterator vectorIterator, 
                                    bool bNewObject, 
                                    dtGame::Message* msg );

         void PrepareSingleUpdateParameter(AttributeToPropertyList& curAttrToProp,
               RTIAttributeHandleValueMap& updateParams,
               const dtGame::ActorUpdateMessage& message,
               bool newObject);

         void PrepareArrayUpdateParameter(AttributeToPropertyList& curAttrToProp,
               RTIAttributeHandleValueMap& updateParams,
               const dtGame::ActorUpdateMessage& message,
               bool newObject);

         /**
          * Convenience method for creating message parameters from a mapping between
          * interaction parameters to game message parameters.
          *
          * @param paramNameBuffer Name of the interaction parameter.
          * @param paramToParamMapping Mapping between interaction to game message parameters
          * @param message Game message to have parameters added.
          * @param addMissingParams Flag used to add parameters to the message if not found. (mostly for actor update messages)
          * @param classHandleString Name of the HLA Interaction (for logging error messages)
          * @return FALSE if any of the parameter mappings failed; TRUE othewise.
          */
         bool CreateMessageParameters( 
            const std::string& paramNameBuffer,
            const OneToManyMapping& paramToParamMapping, // Interaction to Message Parameter Mapping Object
            dtGame::Message& message, // Game message to have parameters added to it.
            bool addMissingParams = false, // 
            const std::string& classHandleString = "" // HLA Interaction class name (for log output)
            );

         /**
          * Convenience method for creating message parameters from a mapping between
          * interaction parameters to game message parameters.
          *
          * Same as #CreateMessageParameters but works when the values are stored in a NamedArrayParameter
          */
         bool CreateMessageParametersArray(
           const std::string& paramNameBuffer,
           const OneToManyMapping& paramToParamMapping,
           dtGame::Message& message,
           bool addMissingParams,
           const std::string& classHandleString // HLA Interaction class name
           );

         /**
          * Helper method for obtaining an RTI attribute's buffer and length if the
          * attribute handle matches that as found on an AttributeToPropertyList.
          * NOTE: The returned buffer has the same scope as that of the specified attributeSet.
          *
          * @param attributeSet Set of RTI attributes to be searched.
          * @param curAttrToProp Attribute-to-property mapping that has the attribute handle of interest.
          * @param outBufferLength Length of the attribute buffer that is returned; 0 if not found.
          * @return Buffer associated with the found attribute; NULL if not found.
          */
         std::string GetAttributeBufferAndLength( const RTIAttributeHandleValueMap& attributeSet,
            AttributeToPropertyList& curAttrToProp );

         /**
          * The RTI ambassador.
          */
         dtCore::RefPtr<RTIAmbassador> mRTIAmbassador;

         /**
          * The named of the joined execution.
          */
         std::string mExecutionName;

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

         bool mDDMEnabled;

         dtUtil::Coordinates mCoordinates;

         ObjectRuntimeMappingInfo mRuntimeMappings;

         typedef std::list< std::pair<std::string, dtCore::RefPtr<const dtGame::Message> > > ObjectRegQueue;
         // In some RTI implementations, names must be registered and cleared with the RTI before object updates may be sent.
         ObjectRegQueue mObjectRegQueue;

         std::map<dtCore::RefPtr<const dtCore::ActorType>, dtCore::RefPtr<ObjectToActor> > mActorToObjectMap;
         typedef std::multimap<std::string, dtCore::RefPtr<ObjectToActor> > ObjectToActorMap;
         typedef ObjectToActorMap::iterator ObjectToActorMapIter;
         ObjectToActorMap mObjectToActorMap;


         std::map<const dtGame::MessageType*, dtCore::RefPtr<InteractionToMessage> > mMessageToInteractionMap;
         std::map<std::string, dtCore::RefPtr<InteractionToMessage> > mInteractionToMessageMap;

         ///Does the work of unregistering, see UnregisterActorMapping
         dtCore::RefPtr<ObjectToActor> InternalUnregisterActorMapping(const dtCore::ActorType &type);
         ///Does the work of unregistering, see UnregisterObjectMapping
         dtCore::RefPtr<ObjectToActor> InternalUnregisterObjectMapping(const std::string& objTypeName, const EntityType* thisDisID);
         ///Does the work of unregistering, see UnregisterMessageMapping
         dtCore::RefPtr<InteractionToMessage> InternalUnregisterMessageMapping(const dtGame::MessageType &type);
         ///Does the work of unregistering, see UnregisterInteractionMapping
         dtCore::RefPtr<InteractionToMessage> InternalUnregisterInteractionMapping(const std::string& interName);

         dtCore::RefPtr<dtGame::MachineInfo> mMachineInfo;

         DDMRegionCalculatorGroup mDDMSubscriptionCalculators;
         DDMRegionCalculatorGroup mDDMPublishingCalculators;

         std::vector<std::vector<dtCore::RefPtr<DDMRegionData> > > mDDMSubscriptionRegions;

         std::vector<dtCore::RefPtr<ParameterTranslator> > mParameterTranslators;

         dtCore::RefPtr<dtUtil::Log> mLogger;

         /// This is the default entity attr name.
         std::string mHLAEntityTypeAttrName;

         /**
          * object to actors are allowed to specify an override name for the entity type.
          * The component just needs a set of the possibilities.
          */
         std::set<std::string> mHLAEntityTypeOtherAttrNames;

   };

}
#endif // DELTA_HLA_FOM_TRANSLATOR
