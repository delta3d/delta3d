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

