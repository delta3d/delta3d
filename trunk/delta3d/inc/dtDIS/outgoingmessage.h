#ifndef _dt_dis_outgoing_message_h_
#define _dt_dis_outgoing_message_h_

#include <map>
#include <dtGame/messagetype.h>             // for typedef, params
#include <dtDIS/dtdisexport.h>              // for export symbols
#include <DIS/DataStream.h>                 // for member

namespace dtGame
{
   class Message;
}

namespace DIS
{
   class Pdu;
}

namespace dtDIS
{
   class IMessageToPacketAdapter;

   ///\brief A framework for translating dtGame::Message instances to PDUs.
   ///
   /// Register adapters to handle specific dtGame::MessageTypes.
   class DT_DIS_EXPORT OutgoingMessage
   {
   public:
      /// the container type for message adapters.
      typedef std::multimap<const dtGame::MessageType*,IMessageToPacketAdapter*> AdapterMap;

      /// setup the network support.
      /// @param stream the endian type to be used for the network stream.
      /// @param exercise the DIS exercise identifier for this simulator.
      OutgoingMessage(DIS::Endian stream, unsigned char exercise);

      void Handle(const dtGame::Message& msg);
      void Handle(const DIS::Pdu& pdu);

      void AddAdaptor(const dtGame::MessageType* mt, IMessageToPacketAdapter* adapter);
      void RemoveAdaptor(const dtGame::MessageType* mt, IMessageToPacketAdapter* adapter);

      const DIS::DataStream& GetData() const;

      void ClearData();

      const AdapterMap& GetAdapters() { return mSenders; }

   private:
      OutgoingMessage();   ///< not implemented by design.

      DIS::DataStream mData;
      unsigned char mExerciseID;

      AdapterMap mSenders;
   };
}

#endif  // _dt_dis_outgoing_message_h_
