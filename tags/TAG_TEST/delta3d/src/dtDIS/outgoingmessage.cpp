#include <dtDIS/outgoingmessage.h>
#include <dtDIS/connection.h>
#include <dtDIS/imessagetopacketadapter.h>

#include <DIS/Pdu.h>

#include <dtGame/message.h>

#include <dtUtil/log.h>
#include <dtDIS/containerutils.h>

using namespace dtDIS;

OutgoingMessage::OutgoingMessage(DIS::Endian stream, unsigned char exercise)
   : 
     mPackedEndian(stream) 
   , mExerciseID(exercise)
   , mSenders()
{
}

void OutgoingMessage::Handle(const DIS::Pdu& pdu)
{
   DIS::DataStream dataStream(mPackedEndian);
   pdu.marshal(dataStream);
   mDataStreams.push(dataStream);
}

void OutgoingMessage::Handle(const dtGame::Message& msg)
{
   const dtGame::MessageType& mt = msg.GetMessageType();

   ///\todo use the rest of the adapters in the multimap container.
   AdapterMap::iterator iter = mSenders.find(&mt);
   if (iter != mSenders.end())
   {
      IMessageToPacketAdapter* adapter = (iter->second);

      DIS::Pdu* pdu = adapter->Convert(msg);
      if (pdu)
      {
         pdu->setExerciseID(mExerciseID);
         //pdu->setTimestamp( msg.GetSource().GetTimeStamp()); //no workie
         DIS::DataStream dataStream(mPackedEndian);
         pdu->marshal(dataStream);
         mDataStreams.push(dataStream);
      }
   }
   else
   {
      LOGN_DEBUG("DIS", "No packet adapter for Message of type '" + mt.GetName() + "'" );
   }
}

void OutgoingMessage::AddAdaptor(const dtGame::MessageType* mt, IMessageToPacketAdapter* adapter)
{
   AdapterMap::iterator matchingIterator;

   matchingIterator = dtUtil::find_multimap_pair(mSenders, mt, adapter);
   if( matchingIterator == mSenders.end() )
   {
      mSenders.insert( AdapterMap::value_type(mt,adapter) );
   }
}

void OutgoingMessage::RemoveAdaptor(const dtGame::MessageType* mt, IMessageToPacketAdapter* adapter)
{
   AdapterMap::iterator matchingIterator;

   // Find the exact matching pair (should be unique) and remove it
   matchingIterator = dtUtil::find_multimap_pair(mSenders, mt, adapter);
   if( matchingIterator != mSenders.end() )
   {
      mSenders.erase(matchingIterator);
   }
}

////////////////////////////////////////////////////////////////////////////////
OutgoingMessage::DataStreamContainer& dtDIS::OutgoingMessage::GetData()
{
   return mDataStreams;
}

////////////////////////////////////////////////////////////////////////////////
bool dtDIS::OutgoingMessage::HasData() const
{
   return !mDataStreams.empty();
}

////////////////////////////////////////////////////////////////////////////////
void OutgoingMessage::ClearData()
{
   while (!mDataStreams.empty())
   {
      mDataStreams.pop();
   }
}


//const OutgoingMessage::AdapterMap& OutgoingMessage::GetAdapters()
//{
//   return mSenders;
//}
