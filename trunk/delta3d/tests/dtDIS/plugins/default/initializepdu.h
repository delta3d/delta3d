#ifndef _plugin_initialize_pdu_h_
#define _plugin_initialize_pdu_h_

namespace DIS
{
   class EntityStatePdu;
}

namespace dtTest
{
   /// consisten model for initializing the pdu
   struct InitializePdu
   {
      void operator ()(DIS::EntityStatePdu& pdu) const;
   };
}

#endif  // _plugin_initialize_pdu_h_
