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

   const DIS::EntityStatePdu& pdu = static_cast<const DIS::EntityStatePdu&>( packet );

   // find out if there is an actor for this ID
   const dtDAL::ActorProxy* prox = mConfig->GetActiveEntityControl().GetActor( pdu.getEntityID() );

   // check to see if this class knows about it
   if( prox != NULL )
   {
      // the entity is known, so update the previously created actor.
      SendPartialUpdate( pdu , *prox );
   }
   else  // check to see if the GM knows about it
   {
      // a possible optimization would be to assume that the GM does NOT
      // know about this actor.  but, in the event that an actor supports a
      // property that should associate it with this entity, let's honor that
      // association by controlling that actor rather than a newly created actor.

      // search all the proxies to know if any are posing as the Entity
      typedef std::vector<dtDAL::ActorProxy*> ProxyVector;
      ProxyVector proxies;
      mGM->GetAllActors( proxies );

      ///\todo add support for this when delta3d allows observation of the DIS properties
      dtDIS::details::HasProperty hasprop( dtDIS::EntityPropertyName::ENTITYID ) ;
      hasprop = std::for_each( proxies.begin() , proxies.end() , hasprop ) ;
      size_t matches = hasprop.GetPassedActors().size() ;

      // didn't have an actor that was posing as this Entity
      if( matches < 1 )  // need to create a new actor
      {
         // if no, then make & add an actor, and remember it for removal.
         //ActorTypeMap::iterator iter = mTypes.find( pdu.getEntityID().getEntity() );
         //if( iter != mTypes.end() )
         const dtDAL::ActorType* actortype( NULL );
         dtDIS::ActorMapConfig& emapper = mConfig->GetActorMap();
         const DIS::EntityType& etype = pdu.getEntityType();
         if( emapper.GetMappedActor(etype, actortype) )
         {
            //this isn't the correct way to add a new remote actor. 
            //dtCore::RefPtr<dtDAL::ActorProxy> proxy = mGM->CreateActor( *(actortype) );
            //AddActor( pdu, proxy.get() );

            CreateRemoteActor( *actortype, pdu);
         }
         else
         {
            std::ostringstream ss;
            ss << etype;
            LOG_ERROR("Don't know the ActorType to create for " + ss.str() )
         }
      }
      else  // at least one actor is already associated with this entity.
      {
         // start controlling the associated actor, maybe delete the others???
         dtCore::RefPtr<dtDAL::ActorProxy> proxy = hasprop.GetPassedActors().front();
         mConfig->GetActiveEntityControl().AddEntity( pdu.getEntityID(), proxy.get() );
         SendPartialUpdate( pdu , *proxy );
      }
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
   apply( pdu , *msg );

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

void ESPduProcessor::AddActor(const DIS::EntityStatePdu& pdu, dtDAL::ActorProxy* proxy)
{
   dtGame::GameActorProxy* gap = dynamic_cast<dtGame::GameActorProxy*>( proxy );
   if( gap )
   {
      ApplyFullUpdateToProxy( pdu, *gap );
      mGM->AddActor( *gap, true, false );
   }
   else
   {
      mGM->AddActor( *proxy );
   }

   // keep this actor in local memory for easy access
   mConfig->GetActiveEntityControl().AddEntity( pdu.getEntityID(), proxy );
}

void dtDIS::ESPduProcessor::CreateRemoteActor(const dtDAL::ActorType &actorType,
                                              const DIS::EntityStatePdu& pdu)
{
   dtCore::RefPtr<dtGame::ActorUpdateMessage> msg;
   mGM->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_CREATED, msg);

   msg->SetSource( *mMachineInfo );

   msg->SetActorTypeCategory( actorType.GetCategory() );
   msg->SetActorTypeName( actorType.GetName() );

   dtDIS::details::FullApplicator copyToMsg;
   copyToMsg(pdu, *msg, mConfig);

   mGM->SendMessage(*msg);
}
