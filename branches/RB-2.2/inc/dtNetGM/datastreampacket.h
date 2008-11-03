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
 * Coded with an italian keyboard and an italian Visual Studio (I am Dutch and don't speak and read italian)
 */

#ifndef DELTA_DATASTREAMPACKET
#define DELTA_DATASTREAMPACKET

#ifdef _MSC_VER
#pragma warning ( disable : 4275 )
#endif

#include <dtNetGM/export.h>
#include <gnelib.h>
#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>

// Forward declarations
namespace dtGame
{
   class Message;
   class MachineInfo;
}

namespace dtNetGM
{
   /**
    * @class DataStreamPacket
    * @brief A DataStreamPacket contains a dtGame::Message blocks to be sent over a Network ot
    * other connected dtGame::GameManager's.
    * @see dtGame::Message
    */

   class DT_NETGM_EXPORT DataStreamPacket : public GNE::Packet
   {
   public:
      // pointer used by GNE
      typedef GNE::SmartPtr<DataStreamPacket> sptr;
      // pointer used by GNE
      typedef GNE::WeakPtr<DataStreamPacket> wptr;

   public:
      static const int MAX_PAYLOAD = 500;

      /// Constructor
      DataStreamPacket();

      /// Constructor
      DataStreamPacket(GNE::guint16 streamId, GNE::guint16 dataStreamSize, GNE::guint8 packetId = 0);

      /**
       * Copy constructor, used by the GNE::PacketParser to send a copy of the DataStreamPacket
       * across the Network
       * @param messagePacket the DataStreamPacket to be copied.
       */
      DataStreamPacket(const DataStreamPacket& dataStreamPacket);

      /// Destructor, public for GNE......
      virtual ~DataStreamPacket(void);

   public:
      /**
       * ID used by GNE to identify the DataStreamPacket. The DataStreamPacket has an ID of
       * GNE::PacketParser::MIN_USER_ID
       */
      static const int ID = GNE::PacketParser::MIN_USER_ID + 1; // MIN_USER_ID used by MessagePacket

      /**
       * Writes a DataStreamPacket into a packet stream, used by GNE::PacketParser
       * @param raw The buffer to write the DataStreamPacket to
       */
      virtual void writePacket(GNE::Buffer& raw) const;

      /**
       * Reads a DataStreamPacket from a packet stream, used by GNE::PacketParser
       * @param raw The buffer to read the DataStreamPacket from
       */
      virtual void readPacket(GNE::Buffer& raw);

      /**
       * Gets the size of the DataStreamPacket, used by GNE::PacketParser
       * @return The size
       */
      virtual int getSize() const;

      GNE::gbyte* GetPayloadBuffer() { return mPayloadBuffer; };
      unsigned int GetPayloadSize() { return unsigned(mPayloadSize); };
      void SetPayloadSize(GNE::guint16 size) { mPayloadSize = size; };

      void SetDataStreamId(GNE::guint16 id) { mDataStreamId = id; };
      unsigned int GetDataStreamId() { return unsigned(mDataStreamId); };

      void SetDataStreamSize(GNE::guint16 size) { mDataStreamSize = size; };
      unsigned int GetDataStreamSize() { return unsigned(mDataStreamSize); };

      void SetPacketCount(GNE::guint8 count) { mPacketCount = count; };
      unsigned int GetPacketCount() { return unsigned(mPacketCount); };

      void SetIndex(GNE::guint8 index) { mPacketId = index; };
      unsigned int GetIndex() { return unsigned(mPacketId); };

   private:
      GNE::guint16 mDataStreamId; // contains an ID number for the datastream, a sequence can contain several packets
      GNE::guint16 mDataStreamSize; // total bytes contained by datastream

      GNE::guint8 mPacketId; // index of this Packet in stream !!!! starts at 0
      GNE::guint8 mPacketCount; // count of total packets in stream
      GNE::guint16 mPayloadSize; // payloadsize of this packet

      GNE::gbyte mPayloadBuffer[MAX_PAYLOAD]; // dataBuffer
   };
}

#endif // DELTA_DATASTREAMPACKET
