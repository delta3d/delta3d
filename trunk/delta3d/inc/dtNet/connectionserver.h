#ifndef CONNECTIONSERVER_INCLUDE
#define CONNECTIONSERVER_INCLUDE

#include <dtCore/export.h>

#include <gnelib/ServerConnectionListener.h>
#include <gnelib/ConnectionListener.h>
#include <gnelib/SmartPtr.h>

#ifdef _MSC_VER
   #pragma warning( disable:4276 )  
#endif

namespace dtNet
{

   class DT_EXPORT ConnectionServer : public GNE::ServerConnectionListener
   {
   protected:
      ConnectionServer(int inRate, int outRate );

   public:

      virtual ~ConnectionServer(void);

      typedef GNE::SmartPtr<ConnectionServer> sptr;
      typedef GNE::WeakPtr<ConnectionServer> wptr;

      static sptr create( int inRate, int outRate)
      {
         sptr ret( new ConnectionServer(inRate, outRate) );         
         ret->setThisPointer( ret );
         return ret;
      }

      virtual void getNewConnectionParams( GNE::ConnectionParams &params );

      virtual void onListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr &listener);

      virtual void onListenSuccess(const GNE::ConnectionListener::sptr &listener);

   private:
      int mInRate;
      int mOutRate;

   };
}


#endif //CONNECTIONSERVER_INCLUDE

