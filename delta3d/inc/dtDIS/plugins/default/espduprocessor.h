#ifndef _plugin_espdu_processor_h_
#define _plugin_espdu_processor_h_

#include <DIS/IPacketProcessor.h>   // for base class

#include <DIS/EntityID.h>
#include <map>
#include <dtCore/refptr.h>          // for member

#include <dtDIS/entityidcompare.h>
#include <dtGame/messagefactory.h>        // for usage

#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>      // for export symbols


namespace DIS
{
   class EntityStatePdu;
}

namespace dtDAL
{
   class ActorProxy;
}

namespace dtGame
{
   class GameManager;
   class GameActorProxy;
}

namespace dtDIS
{
   class SharedState;

   /// the interface class responsible for applying data from a DIS::EntityStatePdu packet.
   class DT_DIS_DEFAULT_EXPORT ESPduProcessor : public DIS::IPacketProcessor
   {
   public:
      ESPduProcessor(dtGame::GameManager* gm, SharedState* config);
      ~ESPduProcessor();

      void Process(const DIS::Pdu& p);

   protected:
      void SendPartialUpdate(const DIS::EntityStatePdu& pdu, const dtDAL::ActorProxy& actor);
      void ApplyFullUpdateToProxy(const DIS::EntityStatePdu& pdu, dtGame::GameActorProxy& proxy);
      void AddActor(const DIS::EntityStatePdu& pdu, dtDAL::ActorProxy* proxy);

   private:
      ESPduProcessor();  ///< not implemented by design

      dtCore::RefPtr<dtGame::GameManager> mGM;
      SharedState* mConfig;
   };
}

#endif  // _plugin_espdu_processor_h_
