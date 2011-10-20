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

#include <dtNetGM/serverconnectionlistener.h>
#include <dtNetGM/clientconnectionlistener.h>
#include <dtNetGM/networkbridge.h>
#include <dtNetGM/networkcomponent.h>
#include <dtNetGM/servernetworkcomponent.h>
#include <gnelib/ConnectionParams.h>
#include <dtUtil/log.h>

namespace dtNetGM
{
   ServerConnectionListener::ServerConnectionListener(ServerNetworkComponent* serverNetworkComp, int inRate, int outRate, bool reliable)
      : GNE::ServerConnectionListener()
      , mInRate(inRate)
      , mOutRate(outRate)
      , mReliable(reliable)
      , mServerNetworkcomponent(serverNetworkComp)
   {
      LOG_DEBUG("Creating ServerConnectionListener");
   }

   ServerConnectionListener::~ServerConnectionListener(void)
   {
      LOG_DEBUG("Destroying ServerConnectionListener");
   }

   void ServerConnectionListener::onListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener)
   {
      if (mServerNetworkcomponent.valid())
      {
         mServerNetworkcomponent->OnListenFailure(error, from, listener);
      }
   }

   void ServerConnectionListener::onListenSuccess(const GNE::ConnectionListener::sptr& listener)
   {
      if (mServerNetworkcomponent.valid())
      {
         mServerNetworkcomponent->OnListenSuccess();
      }
   }

   void ServerConnectionListener::getNewConnectionParams(GNE::ConnectionParams& params)
   {
      params.setInRate(mInRate);
      params.setOutRate(mOutRate);
      params.setUnrel(!mReliable);
//      params.setListener(NetworkBridge::Create(mServerNetworkcomponent.get()));
      params.setListener(ClientConnectionListener::Create(new NetworkBridge(mServerNetworkcomponent.get())));
   }
}
