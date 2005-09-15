#include <dtNet/netmgr.h>
#include <dtNet/connectionlistener.h>


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

   if (mIsServer)
   {
      //GNE::shutdownGNE();
     //mConnectionServer->close();
   }
   else
   {
   }

   //GNE::ServerConnectionListener::closeAllListeners();
   //GNE::Connection::disconnectAll();
   //GNE::Timer::stopAll();
   //GNE::Thread::requestAllShutdown( GNE::Thread::USER ); 
}


void NetMgr::InitializeGame(const std::string &gameName, int gameVersion, const std::string &logFile )
{

   if (GNE::initGNE(NL_IP, atexit) )
   {
      LOG_ERROR("Can't initialize network");
      return;
   }

   GNE::setGameInformation(gameName, gameVersion );

   //GNE::Console::initConsole();

   GNE::initDebug(GNE::DLEVEL1 | GNE::DLEVEL2 | GNE::DLEVEL3 | GNE::DLEVEL4 | GNE::DLEVEL5, logFile.c_str());

   mInitialized = true;
}


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

   GNE::ConnectionParams params( ConnectionListener::create() );
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

bool NetMgr::SetupServer(int portNum)
{
   if (!mInitialized)
   {
      LOG_ALWAYS("Network must be initialized first");
      return false;
   }

   bool ret = true;
   mIsServer = true;

   ConnectionServer::sptr mConnectionServer = ConnectionServer::create(0, 0);

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

   return ret;
}


void NetMgr::Shutdown()
{
   GNE::shutdownGNE();
}
