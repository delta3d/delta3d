#include <dtDIS/plugins/default/actorupdatetoentitystate.h>
#include <dtDIS/plugins/default/espduapplicator.h>
#include <dtDIS/propertyname.h>

#include <DIS/EntityStatePdu.h>
#include <DIS/EntityType.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtGame/actorupdatemessage.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/stringutils.h>

using namespace dtDIS;

ActorUpdateToEntityState::ActorUpdateToEntityState(dtDIS::SharedState* config, dtGame::GameManager* gm)
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

   //set the DIS::EntityType on the PDU
   SetEntityType(uniqueID, downcastPdu); 

   // fill the packet with data
   details::FullApplicator applicator;
   applicator( static_cast<const dtGame::ActorUpdateMessage&>(source), eid, *downcastPdu, mConfig );

   return mPdu;
}

////////////////////////////////////////////////////////////////////////////////
void ActorUpdateToEntityState::SetEntityType(const dtCore::UniqueId& uniqueID, 
                                             DIS::EntityStatePdu* downcastPdu)
{
   const dtGame::GameActorProxy* aboutProxy = mGM->FindGameActorById(uniqueID);
   if (aboutProxy == NULL) {return;}

   const dtDAL::ActorProperty* prop = aboutProxy->GetProperty(dtDIS::EntityPropertyName::ENTITY_TYPE.Get()); 
   if (prop == NULL) {return;}

   const dtDAL::StringActorProperty *entityTypeProp = dynamic_cast<const dtDAL::StringActorProperty*>(prop);
   if (entityTypeProp == NULL) {return;}

   DIS::EntityType entityType;
   const std::string entityTypeStr = entityTypeProp->GetValue();

   //TODO this is probably not very efficient
   std::vector<std::string> tokens;
   dtUtil::StringTokenizer<dtUtil::IsSlash> tokenizer;
   DTUNREFERENCED_PARAMETER(tokenizer); //to squelch "unreferenced" compiler warning

   tokenizer.tokenize(tokens, entityTypeStr);
   if (tokens.size() == 6)
   {
      entityType.setEntityKind(atoi(tokens[0].c_str()));
      entityType.setDomain(atoi(tokens[1].c_str()));
      entityType.setCountry(atoi(tokens[2].c_str()));
      entityType.setCategory(atoi(tokens[3].c_str()));
      entityType.setSubcategory(atoi(tokens[4].c_str()));
      entityType.setSpecific(atoi(tokens[5].c_str()));
   }

   downcastPdu->setEntityType(entityType);
}

