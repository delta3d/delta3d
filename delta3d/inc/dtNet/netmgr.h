#ifndef NETMGR_INCLUDED
#define NETMGR_INCLUDED


#include <dtCore/base.h>
#include <gnelib.h>
#include <dtUtil/log.h>
#include <dtNet/connectionserver.h>

namespace dtNet
{

   class DT_EXPORT NetMgr :  public dtCore::Base
   {
   public:
      NetMgr();
      ~NetMgr();
      void InitializeGame(const std::string &gameName, int gameVersion, const std::string &logFile );
      bool SetupServer(int portNum);
      bool SetupClient(const std::string &host, int portNum );

      void Shutdown();

   private:
      //dtNet::ConnectionServer::sptr mConnectionServer;
      //GNE::ClientConnection::sptr mClient;

      bool mInitialized; ///<has the network been inititialed yet?
      bool mIsServer; ///<are we a server?
   };
   

}

#endif