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


/** Initialize the network and setup the game parameters.  This method must be
 *  called before any other NetMgr methods.
 *  The supplied  game name and game version are used to during the connection process to 
 *  verify if the client/server match.
 *
 * @param gameName : the name of the network game
 * @param gameVersion : the version number of the game
 * @param logFile : a filename to log networking debug information
 */
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

/** Create a client and try to connect to the supplied host name.
 * @param host : the name of the host to connect to
 * @param portNum : the socket port number to use
 * @return true if successful, false otherwise
 */
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

/** Create and start the server network.
 * @param portNum : the socket port number to listen to
 * @return true if successful, false otherwise
 */
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

/** Perform a graceful shutdown of the network.  This will attempt to disconnect
 *  all currently active connections.
 */
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

/** Internal method used to store the connection in a map.  Typically gets called
 * from OnConnect() and OnNewConn() to save the connection for later use.
 * @param connection : the connection to add to the list
 */
void NetMgr::AddConnection(GNE::Connection *connection)
{
   mMutex.acquire();

   LOG_DEBUG("Storing connection to:" + connection->getRemoteAddress(true).toString() );
   mConnections[connection->getRemoteAddress(true).toString()] = connection;

   mMutex.release();
}

/** Internal method used to remove an existing connection from the list.  If 
 * the supplied connection is not in the list, it won't be removed.
 * @param connection : the connection to remove from the list
 */
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

/** Sends the supplied packet to all connections in the list.  If this is
 * a server, it will send the packet to all existing connections.  If this is
 * a client, typically there will be only one connection: to the server.
 *
 * @param address : the string representation of the address to send to or "all"
 * @param packet : the GNE::Packet to send to the world
 * @see AddConnection()
 */
void NetMgr::SendPacket( const std::string &address, GNE::Packet &packet )
{
   mMutex.acquire();

   if (address != "all")
   {
      mConnections[address]->stream().writePacket(packet, true );
   }
   else
   {
      ConnectionIterator conns = mConnections.begin();
      while (conns != mConnections.end())
      {
         //This fails if a connection is broken. We need some error checking here.
         (*conns).second->stream().writePacket(packet, true);
         ++conns;
      }
   }
   mMutex.release();
}


void NetMgr::OnListenSuccess()
{
   LOG_INFO("On Listen success");
}

/**
 * @param error : The GNE:Error describing the failure
 * @param from : The GNE::Address of the problem
 * @param listener The GNE::ConnectionListen who triggered this failure
 */
void NetMgr::OnListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener)
{
   LOG_ERROR("onListenFailure")
}

/** 
 * @param conn : the GNE::Connection that was just disconnected
 */
void NetMgr::OnDisconnect( GNE::Connection &conn)
{
   LOG_DEBUG("onDisconnect");
}

/** 
 * @param conn : the GNE::Connetion that just exited
 */
void NetMgr::OnExit( GNE::Connection &conn)
{
   RemoveConnection(&conn);
   LOG_DEBUG("onExit");
}

/**
 * Typically, this new connection gets stored in a list for future reference.
 * @param conn : the new connection
 * @see AddConnection()
 */
void NetMgr::OnNewConn( GNE::SyncConnection &conn)
{
   GNE::Connection &connection = *conn.getConnection();

   AddConnection( &connection );

   LOG_DEBUG("A new connection was received");
}

/**
* Typically, this connection gets stored in a list for future reference.
* @param conn : the new connection
* @see AddConnection()
*/
void NetMgr::OnConnect( GNE::SyncConnection &conn)
{
   LOG_DEBUG("Connection to server was successfull");
   GNE::Connection &connection = *conn.getConnection();

   AddConnection( &connection );
}

/**
 * @param : conn : the GNE::Connection which contains the GNE::Packets to be read
 */
void NetMgr::OnReceive( GNE::Connection &conn)
{
   LOG_DEBUG("Received packet");

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

/** 
 * @param conn: The GNE::Connection that caused the failure
 * @param error : The error describing the failure
 */
void NetMgr::OnFailure( GNE::Connection &conn, const GNE::Error &error )
{
   LOG_DEBUG("onFailure");
}

/** 
* @param conn: The GNE::Connection that caused the failure
* @param error : The error describing the failure
*/
void NetMgr::OnError( GNE::Connection &conn, const GNE::Error &error)
{
   LOG_DEBUG("onError");
}

/** 
* @param conn: The GNE::Connection that caused the failure
* @param error : The error describing the failure
*/
void NetMgr::OnConnectFailure( GNE::Connection &conn, const GNE::Error &error)
{
   LOG_DEBUG(error.toString() + "from " + conn.getRemoteAddress(true).toString() );
}
