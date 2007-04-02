#ifndef _dtdis_create_entity_processor_h_
#define _dtdis_create_entity_processor_h_

#include <DIS/IPacketProcessor.h>   // for base class
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>      // for export symbols


namespace dtDIS
{
   class OutgoingMessage;
   class SharedState;

   /// the plugin's model of how to respond to the DIS::CreateEntityPdu message.
   class DT_DIS_DEFAULT_EXPORT CreateEntityProcessor : public DIS::IPacketProcessor
   {
   public:
      CreateEntityProcessor(OutgoingMessage* omsg, SharedState* config);
      ~CreateEntityProcessor();

      void Process(const DIS::Pdu& p);

   private:
      CreateEntityProcessor();  ///< not implemented by design

      OutgoingMessage* mOutgoingMessage;
      SharedState* mConfig;
   };
}

#endif // _dtdis_create_entity_processor_h_
