#include <dtDIS/plugins/default/espduprocessor.h>
#include <dtDIS/plugins/default/espduapplicator.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>

#include <dtGame/actorupdatemessage.h>
#include <DIS/EntityStatePdu.h>

#include <DIS/StreamUtils.h>
#include <sstream>
#include <dtDIS/hasproperty.h>
#include <dtDIS/propertyname.h>



using namespace dtDIS;

ESPduProcessor::ESPduProcessor(dtGame::GameManager* gm, SharedState* config)
   : mGM( gm )
   , mConfig(config)
   , mMachineInfo( new dtGame::MachineInfo() )
{
}

ESPduProcessor::~ESPduProcessor()
{
}

void ESPduProcessor::Process(const DIS::Pdu& packet)
{
   if (mConfig == NULL) return;

   const DIS::EntityStatePdu& pdu = static_cast<const DIS::EntityStatePdu&>(packet);

   // find out if there is an actor for this ID
   const dtCore::UniqueId* actorID = mConfig->GetActiveEntityControl().GetActor(pdu.getEntityID());

   // check to see if this class knows about it
   if (actorID != NULL)
   {
      // the entity is known, so update the previously created actor.
      dtDAL::ActorProxy* proxy = mGM->FindActorById(*actorID);
      if (proxy)
      {
         SendPartialUpdate(pdu, *proxy);
      }     
   }
   else
   {
      //looks like we received a packet that we sent.  Just ignore it and move on
      if ((mConfig->GetApplicationID() == pdu.getEntityID().getApplication()) &&
          (mConfig->GetSiteID() == pdu.getEntityID().getSite()))
      {
         return;
      }

      CreateRemoteActor(pdu);
   }
}

void ESPduProcessor::SendPartialUpdate(const DIS::EntityStatePdu& pdu, const dtDAL::ActorProxy& actor)
{
   dtCore::RefPtr<dtGame::ActorUpdateMessage> msg;
   mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED,msg);

   // customize it for the actor
   msg->SetSendingActorId( actor.GetId() );
   msg->SetAboutActorId( actor.GetId() );
   msg->SetName( actor.GetName() );
   msg->SetActorTypeName( actor.GetActorType().GetName() );
   msg->SetActorTypeCategory( actor.GetActorType().GetCategory() );

   details::PartialApplicator apply;
   apply(pdu, *msg, mConfig);

   // send it
   mGM->SendMessage( *msg );
}

void ESPduProcessor::ApplyFullUpdateToProxy(const DIS::EntityStatePdu& pdu, dtGame::GameActorProxy& proxy)
{
   // make a message
   dtCore::RefPtr<dtGame::ActorUpdateMessage> msg;
   mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED,msg);

   // customize it for the actor
   msg->SetSendingActorId( proxy.GetId() );
   msg->SetAboutActorId( proxy.GetId() );
   msg->SetName( proxy.GetName() );
   msg->SetActorTypeName( proxy.GetActorType().GetName() );
   msg->SetActorTypeCategory( proxy.GetActorType().GetCategory() );

   details::FullApplicator apply;
   apply( pdu , *msg, mConfig );

   proxy.ApplyActorUpdate( *msg );
}

//void ESPduProcessor::AddActor(const DIS::EntityStatePdu& pdu, dtDAL::ActorProxy* proxy)
//{
//   dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>( proxy );
//   if( gap )
//   {
//      ApplyFullUpdateToProxy( pdu, *gap );
//      mGM->AddActor( *gap, true, false );
//   }
//   else
//   {
//      mGM->AddActor( *proxy );
//   }
//
//   // keep this actor in local memory for easy access
//   mConfig->GetActiveEntityControl().AddEntity( pdu.getEntityID(), proxy );
//}

void dtDIS::ESPduProcessor::CreateRemoteActor(const DIS::EntityStatePdu& pdu)
{
   const dtDAL::ActorType* actorType(NULL);
   dtDIS::ActorMapConfig& emapper = mConfig->GetActorMap();
   const DIS::EntityType& entityType = pdu.getEntityType();

   if(emapper.GetMappedActor(entityType, actorType))
   {
      dtCore::RefPtr<dtGame::ActorUpdateMessage> msg;
      mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED, msg);

      msg->SetSource(*mMachineInfo);

      msg->SetActorTypeCategory(actorType->GetCategory());
      msg->SetActorTypeName(actorType->GetName());

      dtCore::UniqueId newActorID;
      msg->SetAboutActorId(newActorID);
      msg->SetName(newActorID.ToString());

      dtDIS::details::FullApplicator copyToMsg;
      copyToMsg(pdu, *msg, mConfig);

      mGM->SendMessage(*msg);

      //store the ID for later retrieval
      mConfig->GetActiveEntityControl().AddEntity(pdu.getEntityID(), newActorID);

      //TODO SendPartialUpdate()?
   }
   else
   {
      std::ostringstream ss;
      ss << entityType;
      LOG_ERROR("Don't know the ActorType to create for " + ss.str() )
   }
}
