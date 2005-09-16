#ifndef NETMGR_INCLUDED
#define NETMGR_INCLUDED


#include <dtCore/base.h>
#include <gnelib.h>
#include <dtUtil/log.h>

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
      void AddConnection( GNE::Connection *connection);
      void SendPacketToAll( GNE::Packet &packet );

      virtual void OnListenSuccess();
      virtual void OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr &listener);

      virtual void OnDisconnect( GNE::Connection &conn);
      virtual void OnExit( GNE::Connection &conn);
      virtual void OnNewConn( GNE::SyncConnection &conn);
      virtual void OnConnect( GNE::SyncConnection &conn);
      virtual void OnReceive( GNE::Connection &conn);
      virtual void OnFailure( GNE::Connection &conn, const GNE::Error &error );
      virtual void OnError( GNE::Connection &conn, const GNE::Error &error);
      virtual void OnConnectFailure( GNE::Connection &conn, const GNE::Error &error);

   private:

      bool mInitialized; ///<has the network been inititialed yet?
      bool mIsServer; ///<are we a server?

      typedef std::map<std::string, GNE::Connection*>::iterator ConnectionIterator;

      std::map<std::string, GNE::Connection*> mConnections;

      GNE::Mutex mMutex;
   };
   

}

#endif
