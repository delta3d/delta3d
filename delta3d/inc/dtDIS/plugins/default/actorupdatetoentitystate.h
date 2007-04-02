#ifndef _plugin_actor_update_to_entity_state_h_
#define _plugin_actor_update_to_entity_state_h_

#include <dtDIS/imessagetopacketadapter.h> // for base class
#include <dtCore/refptr.h>                 // for member
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>             // for library export symbols

namespace dtGame
{
   class GameManager;
}

namespace dtDIS
{
   class SharedState;

   /// responsible for translating ActorUpdateMessage instances to EntityStatePdu instances.
   class DT_DIS_DEFAULT_EXPORT ActorUpdateToEntityState : public dtDIS::IMessageToPacketAdapter
   {
   public:
      ActorUpdateToEntityState(const dtDIS::SharedState* config, dtGame::GameManager* gm);
      ~ActorUpdateToEntityState();

      DIS::Pdu* Convert(const dtGame::Message& source);

   private:
      ActorUpdateToEntityState();  ///< not implemented by design

      const dtDIS::SharedState* mConfig;
      dtGame::GameManager* mGM;
      DIS::Pdu* mPdu;
   };
}

#endif  // _plugin_actor_update_to_entity_state_h_

