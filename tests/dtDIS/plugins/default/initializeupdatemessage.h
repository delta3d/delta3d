#ifndef _plugin_initialize_update_message_h_
#define _plugin_initialize_update_message_h_

namespace dtGame
{
   class ActorUpdateMessage;
}

namespace dtTest
{
   /// consistent model for initializing the msg
   struct InitializeUpdateMessage
   {
      void operator ()(dtGame::ActorUpdateMessage& msg) const;
   };
}

#endif  // _plugin_initialize_update_message_h_
