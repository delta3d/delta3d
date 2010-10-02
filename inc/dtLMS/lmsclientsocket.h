/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Christopher DuBuc
 */

#ifndef DELTA_LMS_CLIENT_SOCKET
#define DELTA_LMS_CLIENT_SOCKET

#include <dtLMS/lmsconnectionstate.h>
#include <dtLMS/lmsmessage.h>
#include <dtUtil/mswinmacros.h>

#ifdef DELTA_WIN32
   #include <winsock.h>
#else
   #include <netinet/in.h>
   #include <netdb.h>
#endif

namespace dtLMS
{
   /**
    * This class is designed to send and receive LMS messages over a TCP socket using
    * a predetermined messaging protocol. It is a lightweight networking component that
    * uses either Winsock32 (Windows) or the Berkeley sockets C API (Linux). Much of
    * this code was derived from research done by Keith Vertanen on Java/C++ communication.
    * See http://keithv.com/project/socket.html for more information.
    */
   class LmsClientSocket
   {
      public:

         /**
          * Constructor which handles the intialization of the socket. If everything initializes
          * ok then mClientState will be set to &LmsConnectionState::INITIALIZED. Otherwise,
          * an LMS_CONNECTION_EXCEPTION will be thrown and mClientState will be set to
          * &LmsConnectionState::ERRORSTATE.
          * @param host The name of machine hosting the LmsServerSocket (generally localhost)
          * @param port The port used for communication.
          * @param reverseBytes A flag to tell the LmsServerSocket if the byte order should
          *      be reversed (big/little endian issue).
          */
         LmsClientSocket(const std::string& host, int port, bool reverseBytes);

         //mEndOfMessageChar
         char GetEndOfMessageChar() { return mEndOfMessageChar; }
         void SetEndOfMessageChar(char endOfMessageChar) { mEndOfMessageChar = endOfMessageChar; }

         //mClientState
         const LmsConnectionState* GetClientState() { return mClientState; }
         void SetClientState(const LmsConnectionState* clientState) { mClientState = clientState; }

         //mClientID
         const std::string& GetClientID() const {return mClientID;}

         void Connect();

         void SendLmsMessage(const LmsMessage& lmsMessage);
         LmsMessage ReceiveLmsMessage();

         void Disconnect(bool normal = true);

         /**
          * Destructor
          */
         virtual ~LmsClientSocket();

      //private methods
      private:

         /**
          * Private method to create a random integer ID value.
          * @return The integer ID in string form.
          */
         std::string CreateRandomID();

         /**
          * Private method that handles the sending and receiving of the initial handshake
          * messages required by the messaging protocol
          */
         void ProcessHandshake();

         /**
          * The private method that actually sends data over the socket.
          */
         void SendString(const std::string& sendString);

         /**
          * The private method that actually receives data over the socket. This method blocks
          * until data is received from the server, terminated by the mEndOfMessageChar character
          * (or until the socket is closed from the other end).
          * @return Returns the string that was received from the server.
          */
         std::string ReceiveString();

      private:

         static const unsigned int BUFFSIZE = 64000; // message buffer size
         static const unsigned int MAX_RECEIVE_LENGTH = 1024; // used in case sender forgets EndOfMessageChar

         int mPort;
         int mSocket;
         char mEndOfMessageChar; // character used by protocol to mark the end of a message
         std::string mClientID; // the ID created by the LmsClientSocket upon instantiation
         std::string mServerID; // the ID returned by the Lms server
         bool mReverseBytes; // a flag to denote if the byte order should be reversed
         hostent* mHostent; // host structure
         sockaddr_in mAddress; // socket address structure
         double mBuffer[BUFFSIZE]; // message buffer
         const LmsConnectionState* mClientState; // the state of the LmsClientSocket
   };
} // namespace dtLMS

#endif // DELTA_LMS_CLIENT_SOCKET
