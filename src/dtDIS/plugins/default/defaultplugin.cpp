#include <dtDIS/plugins/default/defaultplugin.h>
#include <DIS/EntityStatePdu.h>
#include <DIS/IncomingMessage.h>
#include <dtDIS/outgoingmessage.h>

#include <dtGame/gamemanager.h>

#include <vector>
#include <dtCore/refptr.h>

#include <dtDAL/actortype.h>
#include <DIS/PDUType.h>

#include <cstddef>
#include <dtDIS/plugins/default/espduprocessor.h>     // for member
#include <dtDIS/plugins/default/createentityprocessor.h>     // for member
#include <dtDIS/plugins/default/removeentityprocessor.h>     // for member
#include <dtDIS/plugins/default/actorupdatetoentitystate.h>  // for member

using namespace dtDIS;

DefaultPlugin::DefaultPlugin()
   : mESProcessor( NULL )
   , mCreateProcessor( NULL )
   , mRemoveProcessor( NULL )
   , mSendingAdapter( NULL )
{
}

DefaultPlugin::~DefaultPlugin()
{
}

void DefaultPlugin::Start(DIS::IncomingMessage& imsg,
                                 dtDIS::OutgoingMessage& omsg,
                                 dtGame::GameManager* gm,
                                 dtDIS::SharedState* config)
{
   mESProcessor = new ESPduProcessor(gm, config);
   mCreateProcessor = new CreateEntityProcessor( &omsg, config);
   mRemoveProcessor = new RemoveEntityProcessor( &omsg, config);
   mSendingAdapter = new ActorUpdateToEntityState( config, gm );

   imsg.AddProcessor( DIS::PDU_ENTITY_STATE, mESProcessor );
   imsg.AddProcessor( DIS::PDU_CREATE_ENTITY, mCreateProcessor );
   imsg.AddProcessor( DIS::PDU_REMOVE_ENTITY, mRemoveProcessor );
   omsg.AddAdaptor( &dtGame::MessageType::INFO_ACTOR_UPDATED , mSendingAdapter );

   // use the current state of the game.
   typedef std::vector<dtGame::GameActorProxy*> ProxyVector;
   ProxyVector actors;
   gm->GetAllGameActors( actors );

   ProxyVector::iterator iter = actors.begin();
   ProxyVector::iterator enditer = actors.end();
   while( iter != enditer )
   {
      this->OnActorAdded((*iter));
      ++iter;
   }
}

void DefaultPlugin::Finish(DIS::IncomingMessage& imsg, dtDIS::OutgoingMessage& omsg)
{
   imsg.RemoveProcessor( DIS::PDU_ENTITY_STATE , mESProcessor );
   imsg.RemoveProcessor( DIS::PDU_CREATE_ENTITY , mCreateProcessor );
   imsg.RemoveProcessor( DIS::PDU_REMOVE_ENTITY , mRemoveProcessor );
   omsg.RemoveAdaptor( &dtGame::MessageType::INFO_ACTOR_UPDATED , mSendingAdapter );

   delete mESProcessor;
   delete mCreateProcessor;
   delete mRemoveProcessor;
   delete mSendingAdapter;
}

void DefaultPlugin::OnActorAdded(dtGame::GameActorProxy* actor)
{
}

