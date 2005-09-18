#include <dtNet/netmgr.h>
#include <dtNet/connectionlistener.h>
#include <dtNet/connectionserver.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

#include <gnelib.h>

using namespace dtNet;
using namespace dtUtil;




NetMgr::NetMgr():
mInitialized(false),
mIsServer(true)
{
   SetName("NetMgr");
}

NetMgr::~NetMgr(void)
{
   LOG_DEBUG("Shutting down network...");

   GNE::ServerConnectionListener::closeAllListeners();
   GNE::Connection::disconnectAll();
   GNE::Timer::stopAll();
   GNE::Thread::requestAllShutdown( GNE::Thread::USER ); 
}


void NetMgr::InitializeGame(const std::string &gameName, int gameVersion, const std::string &logFile )
{

   if (GNE::initGNE(NL_IP, atexit) )
   {
      LOG_ERROR("Can't initialize network");
      return;
   }


   GNE::setGameInformation(gameName, gameVersion );

   GNE::GNEProtocolVersionNumber num = GNE::getGNEProtocolVersion();

   Log::GetInstance().LogMessage(Log::LOG_DEBUG, __FUNCTION__,     
         "Using GNE protocol: %d.%d.%d", num.version, num.subVersion, num.build );
   

   #ifdef _DEBUG
   GNE::initDebug(GNE::DLEVEL1 | GNE::DLEVEL2 | GNE::DLEVEL3 | GNE::DLEVEL4 | GNE::DLEVEL5, logFile.c_str());
   #endif

   mInitialized = true;
}

//////////////////////////////////////////////////////////////////////////
bool NetMgr::SetupClient( const std::string &host, int portNum )
{
   if (!mInitialized)
   {
      LOG_ALWAYS("Network must be initialized first");
      return false;
   }

   bool ret = true;
   mIsServer = false;

   GNE::Address address( host );
   address.setPort( portNum );

   if (!address.isValid())
   {
      LOG_ERROR("Address invalid");
      ret = false;
   }

   LOG_INFO("Connecting to server at:" + address.toString() );

   GNE::ConnectionParams params( ConnectionListener::create(this) );
   params.setUnrel(true);
   params.setInRate(0);
   params.setOutRate(0);

   GNE::ClientConnection::sptr mClient = GNE::ClientConnection::create();
   if (mClient->open( address, params))
   {
      LOG_ERROR("Can not open socket");
      ret = false;
   }

   mClient->connect();
   mClient->waitForConnect();

   if (mClient->isConnected())
   {
      LOG_INFO("Client is connected");
   }
   else
   {
      LOG_ERROR("Client connection failed.");
      ret = false;
   }

   return ret;
}

/////////////// server
bool NetMgr::SetupServer(int portNum)
{
   if (!mInitialized)
   {
      LOG_ALWAYS("Network must be initialized first");
      return false;
   }

   mMutex.acquire();

   bool ret = true;
   mIsServer = true;

   ConnectionServer::sptr mConnectionServer = ConnectionServer::create(0, 0, this);

   if (mConnectionServer->open(portNum))
   {
      LOG_ERROR("Can not open server on port" + ToString<int>(portNum) );
      ret = false;
   }

   if (mConnectionServer->listen() )
   {
      LOG_ERROR("Can not listed on server socket");
      ret = false;
   }
   else
   {
      LOG_INFO("Listening for connections");
   }

   mMutex.release();
   return ret;
}


void NetMgr::Shutdown()
{
   if (mIsServer)
      GNE::shutdownGNE();
   else
   {
      //disconnect all connections
      ConnectionIterator conns = mConnections.begin();
      while (conns != mConnections.end())
      {
         (*conns).second->disconnectSendAll();
         ++conns;
      }

      mConnections.clear();
   }
}

void NetMgr::AddConnection(GNE::Connection *connection)
{
   mMutex.acquire();

   LOG_DEBUG("Storing connection to:" + connection->getRemoteAddress(true).toString() );
   mConnections[connection->getRemoteAddress(true).toString()] = connection;

   mMutex.release();
}

void NetMgr::RemoveConnection(GNE::Connection *connection)
{
   mMutex.acquire();

   LOG_DEBUG("Removing connection from:" + connection->getRemoteAddress(true).toString() );
   
   ConnectionIterator itr = mConnections.find(connection->getRemoteAddress(true).toString());
   if (itr != mConnections.end() )
   {
      mConnections.erase(itr);
   }

   mMutex.release();
}


void NetMgr::SendPacketToAll( GNE::Packet &packet )
{
   mMutex.acquire();

   ConnectionIterator conns = mConnections.begin();
   while (conns != mConnections.end())
   {
      (*conns).second->stream().writePacket(packet, true);
      ++conns;
   }

   mMutex.release();
}

//virtual
void NetMgr::OnListenSuccess()
{
   LOG_INFO("On Listen success");
}

//virtual
void NetMgr::OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener)
{
   LOG_ERROR("onListenFailure")
}

void NetMgr::OnDisconnect( GNE::Connection &conn)
{
   //conn = server connection?
   LOG_ALWAYS("onDisconnect");
   SendMessage("onDisconnect");
}

void NetMgr::OnExit( GNE::Connection &conn)
{
   RemoveConnection(&conn);
   LOG_ALWAYS("onExit");
   SendMessage("onExit");
}

void NetMgr::OnNewConn( GNE::SyncConnection &conn)
{
   GNE::Connection &connection = *conn.getConnection();

   AddConnection( &connection );

   SendMessage("onNewConn");
   LOG_INFO("A new connection was received");
}

void NetMgr::OnConnect( GNE::SyncConnection &conn)
{
   LOG_INFO("Connection to server was successfull");
   GNE::Connection &connection = *conn.getConnection();

   AddConnection( &connection );

   SendMessage("onConnect");
}

void NetMgr::OnReceive( GNE::Connection &conn)
{
   LOG_ALWAYS("Received packet");
   SendMessage("onReceive");

   GNE::Packet *next = conn.stream().getNextPacket();

   while (next != NULL)
   {
      int type = next->getType();

      if(type == GNE::PingPacket::ID) 
      {
         GNE::PingPacket &ping = *((GNE::PingPacket*)next);
         if (ping.isRequest())
         {
            ping.makeReply();
            conn.stream().writePacket(ping, true);
         }
         else
         {
            LOG_INFO("Ping: " + ping.getPingInformation().pingTime.toString());
         }
      }

      delete next;
      next = conn.stream().getNextPacket();
   }
}

void NetMgr::OnFailure( GNE::Connection &conn, const GNE::Error &error )
{
   LOG_ALWAYS("onFailure");
   SendMessage("onFailure");
}

void NetMgr::OnError( GNE::Connection &conn, const GNE::Error &error)
{
   LOG_ALWAYS("onError");
   SendMessage("onError");
}

void NetMgr::OnConnectFailure( GNE::Connection &conn, const GNE::Error &error)
{
   LOG_ERROR(error.toString() + "from " + conn.getRemoteAddress(true).toString() );
   SendMessage("onConnectFailure");
}
