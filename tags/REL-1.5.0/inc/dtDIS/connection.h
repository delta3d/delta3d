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

#ifndef __DELTA_DTDIS_DIS_CONNECTION_H__
#define __DELTA_DTDIS_DIS_CONNECTION_H__

#include <nl.h>                          // for member & implementation
#include <cstddef>                       // for size_t definition
#include <dtDIS/dtdisexport.h>           // for library export definitions

namespace dtDIS
{
   ///\brief Makes a multicast connection to support DIS networks.
   ///\note requires the HawkNL socket library.
   /// http://www.hawksoft.com/hawknl/
   class DT_DIS_EXPORT Connection
   {
   public:
      ///\brief makes a socket connection for the specified network.
      /// @param port the serial port for the connection's network host.
      /// @param host the name of the network host.
      void Connect(unsigned int port, const char* host);

      ///\brief closes the socket connection.
      void Disconnect();

      /// \brief publishes the data to the network.
      /// @param buf the buffer to be written to with network bytes.
      /// @param numbytes the number of bytes contained in the the buffer.
      void Send(const char* buf, size_t numbytes);

      ///\brief allocates buf with size numbytes.
      /// @param buf the buffer to be written to with network bytes
      /// @param numbytes the maximum index used for the buffer (buf)
      /// @return the number of bytes read from the connection
      size_t Receive(char* buf, size_t numbytes);

   private:
      void HandleError();

      NLsocket mSocket;
   };
}

#endif  // __DELTA_DTDIS_DIS_CONNECTION_H__
