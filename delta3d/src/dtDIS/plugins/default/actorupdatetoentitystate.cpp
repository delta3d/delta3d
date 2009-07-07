#include <dtDIS/plugins/default/actorupdatetoentitystate.h>
#include <dtDIS/plugins/default/espduapplicator.h>

#include <DIS/EntityStatePdu.h>
//#include <dtGame/message.h>

#include <dtGame/actorupdatemessage.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>

using namespace dtDIS;

ActorUpdateToEntityState::ActorUpdateToEntityState(const dtDIS::SharedState* config, dtGame::GameManager* gm)
   : mConfig(config)
   , mGM(gm)
   , mPdu(new DIS::EntityStatePdu)
{
}

ActorUpdateToEntityState::~ActorUpdateToEntityState()
{
    delete mPdu;
}

DIS::Pdu* ActorUpdateToEntityState::Convert(const dtGame::Message& source)
{
   // fill a packet with data

   DIS::EntityID eid;
   eid.setApplication(mConfig->GetApplicationID());
   eid.setSite(mConfig->GetSiteID());

   //create the DIS entity ID by using some of the UniqueID
   const dtCore::UniqueId &uniqueID = source.GetAboutActorId();
   const std::string idStr = uniqueID.ToString();
   char chars[4];
   for (int i=0; i<4; i++) {chars[i] = idStr[i];}

   eid.setEntity(strtol(&chars[0], NULL, 16));

   // We know its true type since we created it in the this's ctor
   DIS::EntityStatePdu *downcastPdu = reinterpret_cast<DIS::EntityStatePdu*>(mPdu);

   // fill the packet with data
   details::FullApplicator applicator;
   applicator( static_cast<const dtGame::ActorUpdateMessage&>(source), eid, *downcastPdu, mConfig );

   return mPdu;
}

