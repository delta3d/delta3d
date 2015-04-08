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

#include <dtLMS/lmsclientsocket.h>
#include <dtLMS/lmsexceptionenum.h>
#include <dtLMS/lmsmessagetype.h>
#include <dtLMS/lmsmessagevalue.h>
#include <dtUtil/exception.h>
#include <ctime>
#include <sstream>

#include <dtUtil/mswinmacros.h>

#include <cstring>
#include <cstdlib>

#ifndef DELTA_WIN32
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <sys/wait.h>
   #include <unistd.h>
#endif

namespace dtLMS
{

   //////////////////////////////////////////////////////////////////////////
   LmsClientSocket::LmsClientSocket(const std::string &host, int port, bool reverseBytes)
   {
      mPort = port;
      mReverseBytes = reverseBytes;
      mEndOfMessageChar = '\n';

      //create random integer clientID to identify this instance
      mClientID = CreateRandomID();

      mClientState = &LmsConnectionState::INITIALIZING;

      #ifdef DELTA_WIN32
         //initialize winsock
         WSADATA wsaData;
         if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
         {
            mClientState = &LmsConnectionState::ERRORSTATE;
            throw dtLMS::LMSConnectionException(
               "Lms client socket failed to initialize.", __FILE__, __LINE__);
         }
      #endif

      //get host structure
      if ((mHostent = gethostbyname(host.c_str())) == NULL)
      {
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSConnectionException( "Lms client socket failed to initialize.", __FILE__, __LINE__);
      }

      //initialize socket
      if ((mSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
      {
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSConnectionException( "Lms client socket failed to initialize.", __FILE__, __LINE__);
      }

      //initialize address
      mAddress.sin_family = AF_INET;
      mAddress.sin_port = htons(mPort);
      mAddress.sin_addr = *((struct in_addr *) mHostent->h_addr);

      //clear out address memory
      memset(mAddress.sin_zero, 0, 8);

      mClientState = &LmsConnectionState::INITIALIZED;

      srand((unsigned int)0);
   }

   //////////////////////////////////////////////////////////////////////////
   void LmsClientSocket::Connect()
   {
      //connect to socket
      if (connect(mSocket, (struct sockaddr *)&mAddress, sizeof(struct sockaddr)) == -1)
      {
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSConnectionException( "Lms client socket failed to connect.", __FILE__, __LINE__);
      }

      mClientState = &LmsConnectionState::HANDSHAKING;

      ProcessHandshake();
   }

   //////////////////////////////////////////////////////////////////////////
   void LmsClientSocket::ProcessHandshake()
   {
      //PROTOCOL:
      //initial send must be one byte (0 or 1) to request reverse bytes or not
      char temp[1];
      try
      {
         if (mReverseBytes)
         {
            temp[0] = 1;
            send(mSocket, temp, 1, 0);
         }
         else
         {
            temp[0] = 0;
            send(mSocket, temp, 1, 0);
         }
      }
      catch (...)
      {
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSConnectionException( "Lms client socket failed sending ReverseBytes request.", __FILE__, __LINE__);
      }

      //PROTOCOL:
      //send initial message to server and wait for acknowledgement that server is listening
      try
      {
         LmsMessage sendHandshakeMessage(mClientID, LmsMessageType::SIMULATION, LmsMessageValue::SimulationValue::RUNNING.GetName());
         SendLmsMessage(sendHandshakeMessage);
      }
      catch (const dtUtil::Exception& e)
      {
         //write more specific error message and re-throw
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSConnectionException( "Lms client socket failed sending handshake message: " + e.What(), __FILE__, __LINE__);
      }

      //PROTOCOL:
      //wait for server to acknowledge handshake message
      LmsMessage receiveHandshakeMessage;
      try
      {
         receiveHandshakeMessage = ReceiveLmsMessage();
      }
      catch (const dtUtil::Exception& e)
      {
         //write more specific error message and re-throw
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSConnectionException( "Lms client socket failed receiving handshake message: " + e.What(), __FILE__, __LINE__);
      }

      //validate received handshake message and retrieve server ID
      if (receiveHandshakeMessage.GetMessageType() == LmsMessageType::LAUNCH_PAGE &&
         receiveHandshakeMessage.GetValue() == LmsMessageValue::LaunchPageValue::LISTENING.GetName())
      {
         mServerID = receiveHandshakeMessage.GetSenderID();
         mClientState = &LmsConnectionState::CONNECTED;
      }
      else
      {
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSInvalidMessageException( "LMS server returned invalid handshake message.", __FILE__, __LINE__);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   std::string LmsClientSocket::CreateRandomID()
   {
      const int LOW = 0;
      const int HIGH = 9999999; //10 million (not-inclusive)

      //convert seconds to an unsigned integer and seed

      //retrieve random integer
      int r = rand() % (HIGH - LOW + 1) + LOW;

      //convert int to string
      std::ostringstream s;
      s << r;

      //return string value
      return s.str();
   }
   //////////////////////////////////////////////////////////////////////////
   void LmsClientSocket::SendLmsMessage(const LmsMessage &lmsMessage)
   {
      SendString(lmsMessage.ToString());
   }
   //////////////////////////////////////////////////////////////////////////
   void LmsClientSocket::SendString(const std::string &sendString)
   {
      //sendString += mEndOfMessageChar;
      std::string tempStr = sendString + mEndOfMessageChar;

      if (send(mSocket, tempStr.c_str(), tempStr.size(), 0) == -1)
      {
         throw dtLMS::LMSConnectionException( "LMS client failed to send string: " + sendString, __FILE__, __LINE__);
      }
      //printf("Client: Sending %s", strC);
   }
   //////////////////////////////////////////////////////////////////////////
   LmsMessage LmsClientSocket::ReceiveLmsMessage()
   {
      return LmsMessage(ReceiveString());
   }
   //////////////////////////////////////////////////////////////////////////
   std::string LmsClientSocket::ReceiveString()
   {
      int numBytes = 0;
      bool theEnd = false;
      int i;
      unsigned int j;
      char str[MAX_RECEIVE_LENGTH];
      char *temp;

      // set the temp buffer to our already allocated spot
      temp = (char *) mBuffer;

      //count total number of bytes up to and including end of message character
      j = 0;

      //keep reading chunks of bytes until we reach the end of message character
      //or the maximum receive message length
      while (!theEnd)
      {
         //read a chunk of bytes
         if ((numBytes = recv(mSocket, temp, BUFFSIZE, 0)) == -1)
         {
            LOG_ERROR("LMS client failed to receive string from LMS server");
            throw dtLMS::LMSConnectionException( "LMS client failed receiving string from LMS server", __FILE__, __LINE__);
         }

         //loop through bytes in chunk and fill char buffer until we
         //encounter the end character or we reach the maximum length
         for (i=0; i<numBytes; i++)
         {
            if (temp[i] != mEndOfMessageChar ||
               j == MAX_RECEIVE_LENGTH)
            {
               str[j] = temp[i];
               j++;
            }
            else
            {
               theEnd = true;
            }
         }
      }

      str[j] = '\0'; //create null terminated string
      std::string returnString = str;

      //printf("Client: Received %s\n", returnString.c_str());

      return returnString;
   }
   //////////////////////////////////////////////////////////////////////////
   void LmsClientSocket::Disconnect(bool normal)
   {
      //are we stopping in a normal or abnormal state?
      std::string stopValue;
      if (normal)
      {
         stopValue = LmsMessageValue::SimulationValue::STOPPED_NORMAL.GetName();
      }
      else
      {
         stopValue = LmsMessageValue::SimulationValue::STOPPED_ABNORMAL.GetName();
      }

      //send message to server that we are disconnecting
      LmsMessage sendStopMessage(mClientID, LmsMessageType::SIMULATION, stopValue);
      SendLmsMessage(sendStopMessage);

      try
      {
         #ifdef DELTA_WIN32
            closesocket(mSocket);
         #else
            close(mSocket);
         #endif

         mClientState = &LmsConnectionState::FINISHED;
      }
      catch (...)
      {
         mClientState = &LmsConnectionState::ERRORSTATE;
         throw dtLMS::LMSConnectionException( "LMS client failed closing the socket", __FILE__, __LINE__);
      }

   }
   //////////////////////////////////////////////////////////////////////////
   LmsClientSocket::~LmsClientSocket()
   {
      #ifdef DELTA_WIN32
         WSACleanup();
      #endif
   }
   //////////////////////////////////////////////////////////////////////////
}
