#include <dtDIS/plugins/default/createentityprocessor.h>
#include <dtDIS/outgoingmessage.h>
#include <dtDIS/sharedstate.h>

#include <DIS/CreateEntityPdu.h>
#include <DIS/AcknowledgePdu.h>

#include <dtDIS/hasproperty.h>
#include <dtDIS/plugins/default/espduapplicator.h>
#include <dtDIS/propertyname.h>
#include <dtDIS/acknowledgeconstants.h>

using namespace dtDIS;

CreateEntityProcessor::CreateEntityProcessor(OutgoingMessage* omsg, SharedState* config)
   : mOutgoingMessage(omsg)
   , mConfig(config)
{
}

CreateEntityProcessor::~CreateEntityProcessor()
{
}

void CreateEntityProcessor::Process(const DIS::Pdu& packet)
{
   const DIS::CreateEntityPdu& pdu = static_cast<const DIS::CreateEntityPdu&>( packet );

   // find out if there is an actor for this ID
   const dtCore::UniqueId* actorID = mConfig->GetActiveEntityControl().GetActor( pdu.getReceivingEntityID() );

   DIS::AcknowledgePdu acknowledge;
   acknowledge.setAcknowledgeFlag( dtDIS::ACKNOWLEDGE_CREATE_ENTITY );
   if( actorID == NULL )
   {
      // delta3d's DIS support needs to have the EntityType to know what
      // kind of actor to create, since this is not supplied in this
      // kind of Pdu, delta3d is not able to comply.
      acknowledge.setResponseFlag( dtDIS::RESPONSE_UNABLE_TO_COMPLY );
   }
   else
   {
      // the entity already existed, so make the server manager think
      // delta3d could fulfill its request.
      acknowledge.setResponseFlag( dtDIS::RESPONSE_ABLE_TO_COMPLY );
   }
   mOutgoingMessage->Handle( acknowledge );
}

