#ifndef _dtdis_plugin_default_h_
#define _dtdis_plugin_default_h_

#include <dtDIS/idisplugin.h>                         // for base class
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>                        // for export symbols

namespace dtDIS
{
   class ESPduProcessor;
   class CreateEntityProcessor;
   class RemoveEntityProcessor;
   class ActorUpdateToEntityState;

   /// the plugin to support various DIS::Pdu types.
   class DT_DIS_DEFAULT_EXPORT DefaultPlugin : public dtDIS::IDISPlugin
   {
   public:
      DefaultPlugin();

      /// attaches the IPacketProcessor and IMessageToPacketAdapter
      void Start(DIS::IncomingMessage& imsg,
                 dtDIS::OutgoingMessage& omsg,
                 dtGame::GameManager* gm,
                 dtDIS::SharedState* config);

      void Finish(DIS::IncomingMessage& imsg, dtDIS::OutgoingMessage& omsg);

      ~DefaultPlugin();

   private:
      ///\todo inform the sending adapter of the new actors or modify the ActiveEntityControl.
      void OnActorAdded(dtGame::GameActorProxy* actor);

      ESPduProcessor* mESProcessor;
      CreateEntityProcessor* mCreateProcessor;
      RemoveEntityProcessor* mRemoveProcessor;
      ActorUpdateToEntityState* mSendingAdapter;
      const dtDIS::SharedState* mConfig;
   };
}

#endif  // _dtdis_plugin_default_h_

