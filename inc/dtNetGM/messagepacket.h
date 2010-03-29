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
#ifndef DELTA_MESSAGEPACKET
#define DELTA_MESSAGEPACKET

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
    * @class MessagePacket
    * @brief A MessagePacket contains a dtGame::Message to be sent over a Network ot
    * other connected dtGame::GameManager's. The MessagePacket uses the dtGame::MachineInfo::UniqueId
    * to specify its source and destination dtGame::GameManager
    * Currently it is assumed all messagecontent fits into one packet.
    * @see dtGame::Message
    */
   class DT_NETGM_EXPORT MessagePacket : public GNE::Packet
   {
   public:
      // pointer used by GNE
      typedef GNE::SmartPtr<MessagePacket> sptr;
      // pointer used by GNE
      typedef GNE::WeakPtr<MessagePacket> wptr;

   public:
      /// Constructor
      MessagePacket();

      /**
       * Copy constructor, used by the GNE::PacketParser to send a copy of the MessagePacket
       * across the Network
       * @param messagePacket the MessagePacket to be copied.
       */
      MessagePacket(const MessagePacket& messagePacket);

      /**
       * Construct a MessagePacket from a dtGame::Message
       * @param message the Message to be contained in the MessagePacket.
       */
      MessagePacket(const dtGame::Message& message);

      /// Destructor, public for GNE......
      virtual ~MessagePacket(void);

   public:
      /**
       * ID used by GNE to identify the MessagePacket. The MessagePacket has an ID of
       * GNE::PacketParser::MIN_USER_ID
       */
      static const int ID = GNE::PacketParser::MIN_USER_ID;

      /**
       * Writes a MessagePacket into a packet stream, used by GNE::PacketParser
       * @param raw The buffer to write the MessagePacket to
       */
      virtual void writePacket(GNE::Buffer& raw) const;

      /**
       * Reads a MessagePacket from a packet stream, used by GNE::PacketParser
       * @param raw The buffer to read the MessagePacket from
       */
      virtual void readPacket(GNE::Buffer& raw);

      /**
       * Gets the size of the MessagePacket, used by GNE::PacketParser
       * @return The size
       */
      virtual int getSize() const;

      /**
       * Builds a MessagePacket from a message
       * @param the message to be encapsulated by the MessagePacket
       */
      void BuildFromMessage(const dtGame::Message& message);

      /**
       * Fills the content of the encapsulated Message into a Message,
       * The message should be created using the MessageFactory
       * @param The message to write the content to.
       */
      void FillMessage(dtGame::Message& message) const;

      /**
       * Gets the MessageType::mID of the Message contained in the MessagePacket
       * This can be used to recreate the message with the Messagefactory
       * @return The id of the MessageType
       */
      const unsigned short GetMessageId() const { return mMessageId; };

      /**
       * Gets the UniqueId of the Message destination
       * @return The message destination
       */
      const dtCore::UniqueId GetDestinatonId() const { return mDestination; };

      /**
       * Gets the UniqueId of the Message source
       * @return The message source
       */
      const dtCore::UniqueId GetSourceId() const { return mSource; };

      /**
       * Gets the message parameters as string
       * @return The message paramters
       */
      const std::string GetMessageParameters() const { return mMessageParameters; };

      /**
       * Overrides the destination of the contained Message
       * @param The new destination
       */
      void OverrideDestination(const dtGame::MachineInfo& destination);

   private:
      GNE::gint16 mMessageId;

      dtCore::UniqueId mDestination;
      dtCore::UniqueId mSource;

      dtCore::UniqueId mSendingActor;
      dtCore::UniqueId mAboutActor;

      std::string mMessageParameters;
   };
}

#endif // DELTA_MESSAGEPACKET
