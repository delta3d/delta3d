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
#ifndef DELTA_MESSAGESTREAM
#define DELTA_MESSAGESTREAM

#ifdef _MSC_VER
#pragma warning ( disable : 4275 )
#endif

#include <dtNetGM/export.h>
#include <dtGame/datastream.h>

// Forward declarations
namespace dtGame 
{
    class Message;
    class GameManager;
}

namespace dtNetGM
{
    /**
    * @class MessagePacket
    * @brief A MessagePacket contains a dtGame::Message to be sent over a Network to
    * other connected dtGame::GameManager's. The MessagePacket uses the dtGame::MachineInfo::UniqueId
    * to specify its source and destination dtGame::GameManager 
    * Currently it is assumed all messagecontent fits into one packet.
    * @see dtGame::Message
    */
    class DT_NETGM_EXPORT MessageStream : public dtGame::DataStream
    {        
    public:
        /// Constructor
        MessageStream();

        /**
        * Copy constructor, used by the GNE::PacketParser to send a copy of the MessagePacket
        * across the Network
        * @param messagePacket the MessagePacket to be copied.
        */
        MessageStream(const MessageStream &messagePacket);

        /**
        * Construct a MessagePacket from a dtGame::Message
        * @param message the Message to be contained in the MessagePacket.
        */
        MessageStream(const dtGame::Message& message);

        /// Destructor, public for GNE......
        virtual ~MessageStream(void);

        /**
        * Construct a Message from a MessageStream
        * @param gameManager the GameManager to retrieve the MessageFactory.
        */
        dtCore::RefPtr<dtGame::Message> CreateMessage(dtGame::GameManager& gameManager);

        const char* GetMessageBuffer();

    public:
    };
}

#endif // DELTA_MESSAGESTREAM