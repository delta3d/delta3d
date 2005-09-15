#include <dtNet/ConnectionListener.h>
#include <dtUtil/log.h>

using namespace dtNet;
using namespace dtUtil;

ConnectionListener::ConnectionListener()
{
   SetName("ConnectionListener");
   LOG_DEBUG("ConnectionListener created.");
}

ConnectionListener::~ConnectionListener(void)
{
}

void ConnectionListener::onDisconnect( GNE::Connection& conn )
{
   LOG_ALWAYS("onDisconnect");
   SendMessage("onDisconnect");
}

void ConnectionListener::onExit( GNE::Connection& conn )
{
   LOG_ALWAYS("onExit");
   SendMessage("onExit");
}

void ConnectionListener::onNewConn( GNE::SyncConnection& conn2)
{
   LOG_ALWAYS("OnNewConn");
   SendMessage("onNewConn");
}

void ConnectionListener::onReceive( GNE::Connection& conn )
{
   LOG_ALWAYS("onReceive");
   SendMessage("onReceive");
}

void ConnectionListener::onFailure( GNE::Connection& conn, const GNE::Error& error )
{
   LOG_ALWAYS("onFailure");
   SendMessage("onFailure");
}

void ConnectionListener::onError( GNE::Connection& conn, const GNE::Error& error )
{
   LOG_ALWAYS("onError");
   SendMessage("onError");
}

void ConnectionListener::onConnectFailure( GNE::Connection &conn, const GNE::Error &error)
{
   LOG_ALWAYS("onConnectFailure");
   SendMessage("onConnectFailure");
}

void ConnectionListener::onConnect( GNE::SyncConnection &conn )
{
   LOG_ALWAYS("onConnect");
   SendMessage("onConnect");
}
