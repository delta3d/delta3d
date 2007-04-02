#ifndef _dt_dis_i_packet_actor_adapter_h_
#define _dt_dis_i_packet_actor_adapter_h_

#include <dtDIS/dtdisexport.h>       // for export symbols

namespace DIS
{
   class Pdu;
}

namespace dtGame
{
   class Message;
}

namespace dtDIS
{
   /// the interface for translating Delta3D messages into DIS packets.
   class DT_DIS_EXPORT IMessageToPacketAdapter
   {
   public:
      virtual ~IMessageToPacketAdapter();

      /// The interface for converting from a dtGame::Message to a DIS packet.
      /// @param source The data container
      /// @return the serialized packet instance
      virtual DIS::Pdu* Convert(const dtGame::Message& source) = 0;
   };
}

#endif  // _dt_dis_i_packet_actor_adapter_h_
