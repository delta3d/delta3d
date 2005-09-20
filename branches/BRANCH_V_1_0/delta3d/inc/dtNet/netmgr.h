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
#include <gnelib.h>
#include <dtUtil/log.h>

namespace dtNet
{


   class  NetMgr :  public dtCore::Base
   {
   public:
      NetMgr();
      ~NetMgr();
      void InitializeGame(const std::string &gameName, int gameVersion, const std::string &logFile );
      bool SetupServer(int portNum);
      bool SetupClient(const std::string &host, int portNum );
      void Shutdown();
      void AddConnection( GNE::Connection *connection);
      void RemoveConnection(GNE::Connection *connection);

      void SendPacketToAll( GNE::Packet &packet );

      int GetNumConnections() const {return mConnections.size(); }


      virtual void OnListenSuccess();
      virtual void OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr &listener);

      virtual void OnDisconnect( GNE::Connection &conn);

      ///called when the remote has gracefully closed the connection
      virtual void OnExit( GNE::Connection &conn);

      ///called when the server receives a new connection
      virtual void OnNewConn( GNE::SyncConnection &conn);

      ///called when the client is connected to the server
      virtual void OnConnect( GNE::SyncConnection &conn);

      virtual void OnReceive( GNE::Connection &conn);
      virtual void OnFailure( GNE::Connection &conn, const GNE::Error &error );
      virtual void OnError( GNE::Connection &conn, const GNE::Error &error);
      virtual void OnConnectFailure( GNE::Connection &conn, const GNE::Error &error);

   private:

      bool mInitialized; ///<has the network been inititialed yet?
      bool mIsServer; ///<are we a server?

      typedef std::map<std::string, GNE::Connection*>::iterator ConnectionIterator;

      std::map<std::string, GNE::Connection*> mConnections;

      GNE::Mutex mMutex;
   };
   

}

#endif
