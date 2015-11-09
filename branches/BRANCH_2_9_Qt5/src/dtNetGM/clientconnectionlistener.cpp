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

#include <dtNetGM/clientconnectionlistener.h>
#include <dtNetGM/networkbridge.h>
#include <dtNetGM/networkcomponent.h>
#include <dtUtil/log.h>

#include <gnelib/Error.h>
#include <gnelib/Connection.h>
#include <gnelib/SyncConnection.h>
#include <gnelib/PingPacket.h>

namespace dtNetGM
{
   ClientConnectionListener::ClientConnectionListener(NetworkBridge* netwBridge)
      : mNetworkBridge(netwBridge)
   {
      LOGN_DEBUG("dtNetGM", "ClientConnectionListener created.");
   }

   ClientConnectionListener::~ClientConnectionListener(void)
   {
      LOGN_DEBUG("dtNetGM", "ClientConnectionListener destroyed.");
   }

   void ClientConnectionListener::onDisconnect(GNE::Connection& conn)
   {
      mNetworkBridge->OnDisconnect(conn);
   }

   void ClientConnectionListener::onExit(GNE::Connection& conn)
   {
      mNetworkBridge->OnExit(conn);
   }

   void ClientConnectionListener::onNewConn(GNE::SyncConnection& conn)
   {
      mNetworkBridge->OnNewConnection(conn);
   }

   void ClientConnectionListener::onConnect(GNE::SyncConnection& conn)
   {
      mNetworkBridge->OnConnect(conn);
   }

   void ClientConnectionListener::onReceive(GNE::Connection& conn)
   {
      mNetworkBridge->OnReceive(conn);
   }

   void ClientConnectionListener::onFailure(GNE::Connection& conn, const GNE::Error& error)
   {
      mNetworkBridge->OnFailure(conn, error);
   }

   void ClientConnectionListener::onError(GNE::Connection& conn, const GNE::Error& error)
   {
      mNetworkBridge->OnError(conn, error);
   }

   void ClientConnectionListener::onConnectFailure(GNE::Connection& conn, const GNE::Error& error)
   {
      mNetworkBridge->OnConnectFailure(conn, error);
   }
};


