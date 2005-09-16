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

   class DT_EXPORT ConnectionListener : public GNE::ConnectionListener, public dtCore::Base
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
