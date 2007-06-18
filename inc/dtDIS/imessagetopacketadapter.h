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

#ifndef __DELTA_DTDIS_IPACKET_ACTOR_ADAPTER_H__
#define __DELTA_DTDIS_IPACKET_ACTOR_ADAPTER_H__

#include <dtDIS/dtdisexport.h>       // for export symbols

///@cond DOXYGEN_SHOULD_SKIP_THIS
namespace DIS
{
   class Pdu;
}
///@endcond

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

#endif  // __DELTA_DTDIS_IPACKET_ACTOR_ADAPTER_H__
