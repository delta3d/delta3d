/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005 MOVES Institute
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
 */
#ifndef NETMGR_INCLUDED
#define NETMGR_INCLUDED

#include <dtCore/base.h>

#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_ALL_START
#include <gnelib.h>
DT_DISABLE_WARNING_END

#include <dtUtil/log.h>

namespace dtNet
{

   /** This class is used as the base of all networking applications.  It
    *  handles creating a server or a client and provides a convenient place to
    *  implement application-specific functionality.
    *
    *  It can be used as-is, but it's anticipated that the end-user will derive
    *  from this class and override the virtual methods as needed.
    *
    *  To use this class, create an instance and call InitializeGame(); this will
    *  setup the internals of the network.  Then call either SetupServer() or
    *  SetupClient() to start the networking.  To end the networking, just call
    *  the Shutdown() method.
    *
    *  To pass data packets through the network, supply a GNE::Packet to SendPacketToAll().
    *  A custom GNE::Packet must be registered with GNE after InitilizeGame() using:
    *  \code   GNE::PacketParser::defaultRegisterPacket<MyCustomPacket>(); \endcode
    *
    */
   class NetMgr : public dtCore::Base
   {
   public:
      NetMgr();
   protected:
      virtual ~NetMgr();
   public:

      /** Initialize the network and setup the game parameters.  This method must be
        * called before any other NetMgr methods. The supplied  game name and game
        * version are used to during the connection process to verify if the
        * client/server match.
        *
        * @param gameName : the name of the network game
        * @param gameVersion : the version number of the game
        * @param logFile : a filename to log networking debug information
        */
      void InitializeGame(const std::string& gameName, int gameVersion, const std::string& logFile);

      /** Create and start the server network.
       * @param portNum : the socket port number to listen to
       * @return true if successful, false otherwise
       */
      bool SetupServer(int portNum);

      /** Create a client and try to connect to the supplied host name.
       * @param host : the name of the host to connect to
       * @param portNum : the socket port number to use
       * @return true if successful, false otherwise
       */
      bool SetupClient(const std::string& host, int portNum);

      /** Perform a graceful shutdown of the network.  This will attempt to disconnect
       *  all currently active connections.
       */
      void Shutdown();

      /** Sends the supplied packet to all connections in the list.  If this is
       * a server, it will send the packet to all existing connections.  If this is
       * a client, typically there will be only one connection: to the server.
       *
       * @param address : the string representation of the address to send to or "all"
       * @param packet : the GNE::Packet to send to the world
       * @see AddConnection()
       */
      void SendPacket(const std::string& address, GNE::Packet& packet);

      ///Get the number of connections to the network
      int GetNumConnections() const { return mConnections.size(); }

      ///Is this instance setup as a server?
      bool GetIsServer() const { return mIsServer; }

      //////////////////////////////////////////////////////////////////////////
      /// virtual methods
      //////////////////////////////////////////////////////////////////////////

      ///callback to signal a connection is successful
      virtual void OnListenSuccess();

      /**
       * @param error : The GNE:Error describing the failure
       * @param from : The GNE::Address of the problem
       * @param listener The GNE::ConnectionListen who triggered this failure
       */
      virtual void OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener);

      ///The GNE::Connection has been disconnected
      virtual void OnDisconnect(GNE::Connection& conn);

      ///called when the remote has gracefully closed the connection
      virtual void OnExit(GNE::Connection& conn);

      ///called when the server receives a new connection
      virtual void OnNewConn(GNE::SyncConnection& conn);

      ///called when the client is connected to the server
      virtual void OnConnect(GNE::SyncConnection& conn);

      ///one or more GNE::Packets have been received
      virtual void OnReceive(GNE::Connection& conn);

      ///A fatal error has occurred in the connection
      virtual void OnFailure(GNE::Connection& conn, const GNE::Error& error);

      ///A non-fatal error has occurred in the connection
      virtual void OnError(GNE::Connection& conn, const GNE::Error& error);

      ///A connection failed before or during the onConnect event
      virtual void OnConnectFailure(GNE::Connection& conn, const GNE::Error& error);

   protected:

      /** Internal method used to store the connection in a map.  Typically gets called
       * from OnConnect() and OnNewConn() to save the connection for later use.
       * @param connection : the connection to add to the list
       */
      void AddConnection(GNE::Connection* connection);

      /** Internal method used to remove an existing connection from the list.  If
       * the supplied connection is not in the list, it won't be removed.
       * @param connection : the connection to remove from the list
       */
      void RemoveConnection(GNE::Connection* connection);

      bool mInitialized; ///<has the network been inititialed yet?
      bool mIsServer; ///<are we a server?

      typedef std::map<std::string, GNE::Connection*>::iterator ConnectionIterator;

      /** A map of network address strings to Connections*/
      std::map<std::string, GNE::Connection*> mConnections;

      GNE::Mutex mMutex;
   };
}

#endif
