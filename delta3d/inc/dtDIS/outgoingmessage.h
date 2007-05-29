/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_OUTGOING_MESSAGE_H__
#define __DELTA_DTDIS_OUTGOING_MESSAGE_H__

#include <map>
#include <dtGame/messagetype.h>             // for typedef, params
#include <dtDIS/dtdisexport.h>              // for export symbols
#include <DIS/DataStream.h>                 // for member

namespace dtGame
{
   class Message;
}

///@cond DOXYGEN_SHOULD_SKIP_THIS
namespace DIS
{
   class Pdu;
}
///@endcond

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

#endif  // __DELTA_DTDIS_OUTGOING_MESSAGE_H__
