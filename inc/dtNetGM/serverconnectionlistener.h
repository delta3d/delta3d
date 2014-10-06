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
#ifndef DELTA_SERVERCONNECTIONLISTENER
#define DELTA_SERVERCONNECTIONLISTENER

#ifdef _MSC_VER
#pragma warning ( disable : 4275 )
#pragma warning ( disable : 4251 )
#endif


#include <dtNetGM/export.h>
#include <gnelib.h>
#include <dtCore/refptr.h>

namespace dtNetGM
{
   class ServerNetworkComponent;
   /** This class is used as an interface to the GNE::Server connection.  It is
    *  used internally by the ServerNetworkComponent and is typically not used directly by the
    *  end user.
    *  This class takes in a reference to a ServerNetworkComponent and calls it's virtual methods
    *  to mimic the GNE::ServerConnectionLister's callbacks.
    */
   class DT_NETGM_EXPORT ServerConnectionListener : public GNE::ServerConnectionListener
   {
   public:
      /**
       * Construct a ServerConnectionListener
       * @param serverNetworkComp  : instance of a ServerNetworkComponent
       * @param inRate : the incoming bandwidth throttle
       * @param outRate : the outgoing bandwidth throttle
       * @param reliable : reliability of the Network connection
       */
      ServerConnectionListener(ServerNetworkComponent* serverNetworkComp, int inRate, int outRate, bool reliable);

      // Destructor, public for GNE.
      virtual ~ServerConnectionListener(void);

      // pointer used by GNE
      typedef GNE::SmartPtr<ServerConnectionListener> sptr;

      // pointer used by GNE
      typedef GNE::WeakPtr<ServerConnectionListener> wptr;

      ///Method used to create a new instance of ServerConnectionListener
      static sptr Create(ServerNetworkComponent* serverNetworkComp, int inRate, int outRate, bool reliable)
      {
         sptr ret(new ServerConnectionListener(serverNetworkComp, inRate, outRate, reliable));
         ret->setThisPointer(ret);
         return ret;
      }

      // Retrieve connection parameters
      virtual void getNewConnectionParams(GNE::ConnectionParams& params);

      // Function called by GNE
      virtual void onListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener);

      // Function called by GNE
      virtual void onListenSuccess(const GNE::ConnectionListener::sptr& listener);

   protected:
      int mInRate;    /// The incoming bandwidth rate
      int mOutRate;    /// The outgoing bandwidth rate
      bool mReliable;    /// The reliability of the connection

      // Pointer to our ServerNetworkComponent
      dtCore::RefPtr<ServerNetworkComponent> mServerNetworkcomponent;

      // Mutes
      GNE::Mutex mMutex;
   };
}

#endif //DELTA_SERVERCONNECTIONLISTENER
