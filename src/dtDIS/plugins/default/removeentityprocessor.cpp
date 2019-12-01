#include <dtDIS/plugins/default/removeentityprocessor.h>

#include <dtDIS/outgoingmessage.h>
#include <dtDIS/sharedstate.h>

#include <DIS/RemoveEntityPdu.h>
#include <DIS/AcknowledgePdu.h>
#include <dtDIS/acknowledgeconstants.h>

// should not need to include this to compile!
#include <dtCore/actorproxy.h>


using namespace dtDIS;

RemoveEntityProcessor::RemoveEntityProcessor(OutgoingMessage* omsg, SharedState* config)
   : mOutgoingMessage(omsg)
   , mConfig(config)
{
}

RemoveEntityProcessor::~RemoveEntityProcessor()
{
}

void RemoveEntityProcessor::Process(const DIS::Pdu& packet)
{
   const DIS::RemoveEntityPdu& pdu = static_cast<const DIS::RemoveEntityPdu&>( packet );

   // find out if there is an actor for this id
   const dtCore::UniqueId* actorID = mConfig->GetActiveEntityControl().GetActor( pdu.getReceivingEntityID() );

   DIS::AcknowledgePdu acknowledge;
   acknowledge.setAcknowledgeFlag( dtDIS::ACKNOWLEDGE_CREATE_ENTITY );

   if( actorID == NULL )
   {
      // the entity already existed, so remove it
      acknowledge.setResponseFlag( dtDIS::RESPONSE_UNABLE_TO_COMPLY );
   }
   else
   {
      if( mConfig->GetActiveEntityControl().RemoveEntity( pdu.getReceivingEntityID(), *actorID ) )
      {
         // delta3d knew about the entity, and was able to forget about it.
         acknowledge.setResponseFlag( dtDIS::RESPONSE_ABLE_TO_COMPLY );
      }
      else
      {
         // delta3d doesn't know about this entity
         acknowledge.setResponseFlag( dtDIS::RESPONSE_UNABLE_TO_COMPLY );
      }
   }

   mOutgoingMessage->Handle( acknowledge );
}

