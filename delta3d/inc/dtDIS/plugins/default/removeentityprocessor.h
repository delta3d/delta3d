#ifndef _dtdis_remove_entity_processor_h_
#define _dtdis_remove_entity_processor_h_

#include <DIS/IPacketProcessor.h>   // for base class
#include <dtDIS/plugins/default/dtdisdefaultpluginexport.h>      // for export symbols


namespace dtDIS
{
   class OutgoingMessage;
   class SharedState;

   /// the plugin's model of how to respond to the DIS::RemoveEntityPdu message.
   class DT_DIS_DEFAULT_EXPORT RemoveEntityProcessor : public DIS::IPacketProcessor
   {
   public:
      RemoveEntityProcessor(OutgoingMessage* omsg, SharedState* config);
      ~RemoveEntityProcessor();

      void Process(const DIS::Pdu& p);

   private:
      RemoveEntityProcessor();  ///< not implemented by design

      OutgoingMessage* mOutgoingMessage;
      SharedState* mConfig;
   };
}

#endif // _dtdis_remove_entity_processor_h_
