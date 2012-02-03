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

// Must be first because of a hawknl conflict with osg.  This is not a directly required include, but indirectly
#include <osgDB/Serializer>

#include <dtNetGM/messagepacket.h>
#include <iostream>
#include <gnelib.h>
#include <dtGame/messagetype.h>
#include <dtGame/message.h>


namespace dtNetGM
{
   MessagePacket::MessagePacket()
      : GNE::Packet(MessagePacket::ID)
      , mDestination("")
      , mSource("")
      , mSendingActor("")
      , mAboutActor("")
   {
   }

   MessagePacket::MessagePacket(const MessagePacket& messagePacket)
      : GNE::Packet(MessagePacket::ID)
   {
      mMessageId = messagePacket.mMessageId;
      mDestination = messagePacket.mDestination;
      mSource = messagePacket.mSource;

      mSendingActor = messagePacket.mSendingActor;
      mAboutActor = messagePacket.mAboutActor;

      mMessageParameters = messagePacket.GetMessageParameters();
   }

   MessagePacket::MessagePacket(const dtGame::Message& message)
      : GNE::Packet(MessagePacket::ID)
      , mDestination("")
      , mSource(message.GetSource().GetUniqueId())
      , mSendingActor(message.GetSendingActorId())
      , mAboutActor(message.GetAboutActorId())
   {
      BuildFromMessage(message);
   }

   MessagePacket::~MessagePacket(void)
   {
      mMessageParameters.clear();
   }

   int MessagePacket::getSize() const
   {
      // return the sizes for GNE , note GNE::Buffer::getSizeOf
      // to account for delimters
      return (GNE::Packet::getSize() +
              GNE::Buffer::getSizeOf(mMessageId) +
              GNE::Buffer::getSizeOf(mDestination.ToString()) +
              GNE::Buffer::getSizeOf(mSource.ToString()) +
              GNE::Buffer::getSizeOf(mSendingActor.ToString()) +
              GNE::Buffer::getSizeOf(mAboutActor.ToString()) +
              GNE::Buffer::getSizeOf(mMessageParameters)
              );
   }

   void MessagePacket::writePacket(GNE::Buffer& raw) const
   {
      // just write the id and strings to the buffer
      GNE::Packet::writePacket(raw);
      raw << mMessageId;

      raw << mDestination.ToString();
      raw << mSource.ToString();

      raw << mSendingActor.ToString();
      raw << mAboutActor.ToString();

      raw << mMessageParameters;
   }

   void MessagePacket::readPacket(GNE::Buffer& raw)
   {
      // read the id and strings from the buffer
      GNE::Packet::readPacket(raw);
      raw >> mMessageId;

      std::string szUniqueId;
      raw >> szUniqueId;
      mDestination = dtCore::UniqueId(szUniqueId);

      raw >> szUniqueId;
      mSource = dtCore::UniqueId(szUniqueId);

      raw >> szUniqueId;
      mSendingActor = dtCore::UniqueId(szUniqueId);

      raw >> szUniqueId;
      mAboutActor = dtCore::UniqueId(szUniqueId);

      raw >> mMessageParameters;
   }

   void MessagePacket::BuildFromMessage(const dtGame::Message& message)
   {
      // Get the Id from the message
      mMessageId = message.GetMessageType().GetId();

      // If the message has a destination, encapsulate
      if (message.GetDestination() != NULL)
      {
         mDestination = message.GetDestination()->GetUniqueId();
      }

      // encapsulate source
      mSource = message.GetSource().GetUniqueId();

      // encapsulate actor Id's
      mSendingActor = message.GetSendingActorId();
      mAboutActor = message.GetAboutActorId();

      // Get the MessageParameters from the message
      message.ToString(mMessageParameters);
   }

   void MessagePacket::FillMessage(dtGame::Message& message) const
   {
      // The Destination and Source MachineInfo are set by the NetworkComponent!
      message.SetSendingActorId(mSendingActor);
      message.SetAboutActorId(mAboutActor);

      message.FromString(mMessageParameters);
   }

   void MessagePacket::OverrideDestination(const dtGame::MachineInfo& destination)
   {
      // Set the new destination
      mDestination = destination.GetUniqueId().ToString();
   }
}
