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

#ifndef CONNECTIONLISTENER_INCLUDE
#define CONNECTIONLISTENER_INCLUDE

#ifdef _MSC_VER
   #pragma warning( disable:4276 )  
#endif

#include <gnelib/ConnectionListener.h>
#include <dtCore/export.h>
#include <dtCore/base.h>
#include <dtCore/refptr.h>

namespace dtNet
{
   class NetMgr;

   class  ConnectionListener : public GNE::ConnectionListener, public dtCore::Base
   {
   public:
      typedef GNE::SmartPtr<ConnectionListener> sptr;
      typedef GNE::WeakPtr<ConnectionListener> wptr;


      virtual ~ConnectionListener(void);

      static sptr create( NetMgr *netMgr)
      {
         return sptr( new ConnectionListener(netMgr) );
      }

      virtual void onDisconnect( GNE::Connection& conn );

      virtual void onExit( GNE::Connection& conn );

      virtual void onConnect( GNE::SyncConnection &conn );

      virtual void onConnectFailure( GNE::Connection &conn, const GNE::Error &error);

      virtual void onNewConn( GNE::SyncConnection& conn);

      virtual void onReceive( GNE::Connection& conn );

      virtual void onFailure( GNE::Connection& conn, const GNE::Error& error );
         
      virtual void onError( GNE::Connection& conn, const GNE::Error& error );
      
   protected:
      ConnectionListener(NetMgr *netMgr);
   private:
      dtCore::RefPtr<NetMgr> mNetMgr;
   };
}

#endif //CONNECTIONLISTENER_INCLUDE
