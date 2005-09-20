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

#ifndef CONNECTIONSERVER_INCLUDE
#define CONNECTIONSERVER_INCLUDE

#include <dtCore/export.h>
#include <dtNet/netmgr.h>
#include <dtCore/refptr.h>

#include <gnelib/ServerConnectionListener.h>
#include <gnelib/ConnectionListener.h>
#include <gnelib/SmartPtr.h>

#ifdef _MSC_VER
   #pragma warning( disable:4276 )  
#endif

namespace dtNet
{

   class NetMgr;

   class  ConnectionServer : public GNE::ServerConnectionListener
   {
   protected:
      ConnectionServer(int inRate, int outRate, NetMgr *netMgr );

   public:

      virtual ~ConnectionServer(void);

      typedef GNE::SmartPtr<ConnectionServer> sptr;
      typedef GNE::WeakPtr<ConnectionServer> wptr;

      static sptr create( int inRate, int outRate, NetMgr *netMgr)
      {
         sptr ret( new ConnectionServer(inRate, outRate, netMgr) );         
         ret->setThisPointer( ret );
         return ret;
      }

      virtual void getNewConnectionParams( GNE::ConnectionParams &params );

      virtual void onListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr &listener);

      virtual void onListenSuccess(const GNE::ConnectionListener::sptr &listener);

   private:
      int mInRate;
      int mOutRate;
      dtCore::RefPtr<NetMgr> mNetMgr;
      GNE::Mutex mMutex;

   };
}


#endif //CONNECTIONSERVER_INCLUDE

