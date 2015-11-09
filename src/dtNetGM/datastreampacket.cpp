/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Pjotr van Amerongen
 */

#include <dtNetGM/datastreampacket.h>
#include <iostream>
#include <gnelib.h>


namespace dtNetGM
{
   DataStreamPacket::DataStreamPacket()
      : GNE::Packet(DataStreamPacket::ID)
      , mDataStreamId(0)
      , mDataStreamSize(0)
      , mPacketId(0)
      , mPacketCount(0)
      , mPayloadSize(0)
   {
      memset(&mPayloadBuffer, 0, MAX_PAYLOAD);
   }

   DataStreamPacket::DataStreamPacket(GNE::guint16 streamId, GNE::guint16 dataStreamSize, GNE::guint8 packetId)
      : GNE::Packet(DataStreamPacket::ID)
      , mDataStreamId(streamId)
      , mDataStreamSize(dataStreamSize)
      , mPacketId(packetId)
      , mPacketCount(0)
      , mPayloadSize(0)
   {
      memset(&mPayloadBuffer, 0, MAX_PAYLOAD);
   }

   DataStreamPacket::DataStreamPacket(const DataStreamPacket& dataStreamPacket)
      : GNE::Packet(DataStreamPacket::ID)
      , mDataStreamId(dataStreamPacket.mDataStreamId)
      , mDataStreamSize(dataStreamPacket.mDataStreamSize)
      , mPacketId(dataStreamPacket.mPacketId)
      , mPacketCount(dataStreamPacket.mPacketCount)
      , mPayloadSize(dataStreamPacket.mPayloadSize)
   {
      memcpy(&mPayloadBuffer, &dataStreamPacket.mPayloadBuffer, mPayloadSize);
   }

   DataStreamPacket::~DataStreamPacket(void)
   {
   }

   int DataStreamPacket::getSize() const
   {
      // return the sizes for GNE , note GNE::Buffer::getSizeOf
      // to account for delimters
      int size = (GNE::Packet::getSize()
         + GNE::Buffer::getSizeOf(mDataStreamId)
         + GNE::Buffer::getSizeOf(mDataStreamSize)
         + GNE::Buffer::getSizeOf(mPacketId)
         + GNE::Buffer::getSizeOf(mPacketCount)
         + GNE::Buffer::getSizeOf(mPayloadSize)
         + mPayloadSize
         );
      return size;
   }

   void DataStreamPacket::writePacket(GNE::Buffer& raw) const
   {
      // just write the id and strings to the buffer
      GNE::Packet::writePacket(raw);
      raw << mDataStreamId;
      raw << mDataStreamSize;
      raw << mPacketId;
      raw << mPacketCount;
      raw << mPayloadSize;
      raw.writeRaw((GNE::gbyte*)&mPayloadBuffer, mPayloadSize);
   }

   void DataStreamPacket::readPacket(GNE::Buffer& raw)
   {
      // read the id and strings from the buffer
      GNE::Packet::readPacket(raw);
      raw >> mDataStreamId;
      raw >> mDataStreamSize;
      raw >> mPacketId;
      raw >> mPacketCount;
      raw >> mPayloadSize;
      raw.readRaw((GNE::gbyte*)&mPayloadBuffer, mPayloadSize);
   }
}
