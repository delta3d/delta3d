#ifndef CONNECTIONLISTENER_INCLUDE
#define CONNECTIONLISTENER_INCLUDE

#ifdef _MSC_VER
   #pragma warning( disable:4276 )  
#endif

#include <gnelib/connectionlistener.h>
#include <dtCore/export.h>
#include <dtCore/base.h>

namespace dtNet
{
   class DT_EXPORT ConnectionListener : public GNE::ConnectionListener, public dtCore::Base
   {
   public:
      typedef GNE::SmartPtr<ConnectionListener> sptr;
      typedef GNE::WeakPtr<ConnectionListener> wptr;


      virtual ~ConnectionListener(void);

      static sptr create()
      {
         return sptr( new ConnectionListener() );
      }

      virtual void onDisconnect( GNE::Connection& conn );

      virtual void onExit( GNE::Connection& conn );

      virtual void onConnect( GNE::SyncConnection &conn );

      virtual void onConnectFailure( GNE::Connection &conn, const GNE::Error &error);

      virtual void onNewConn( GNE::SyncConnection& conn2);

      virtual void onReceive( GNE::Connection& conn );

      virtual void onFailure( GNE::Connection& conn, const GNE::Error& error );
         
      virtual void onError( GNE::Connection& conn, const GNE::Error& error );
      
   protected:
      ConnectionListener();
   };
}

#endif //CONNECTIONLISTENER_INCLUDE
