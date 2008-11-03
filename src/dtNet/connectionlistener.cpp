#include <dtNet/connectionlistener.h>
#include <dtNet/netmgr.h>
#include <dtUtil/log.h>

#include <gnelib/Error.h>
#include <gnelib/Connection.h>
#include <gnelib/SyncConnection.h>
#include <gnelib/PingPacket.h>

using namespace dtNet;
using namespace dtUtil;

ConnectionListener::ConnectionListener(NetMgr *netMgr):
mNetMgr(netMgr)
{
   LOG_DEBUG("ConnectionListener created.");
}

ConnectionListener::~ConnectionListener(void)
{
   LOG_DEBUG("ConnectionListener destroyed.");
}

void ConnectionListener::onDisconnect( GNE::Connection& conn )
{
   mNetMgr->OnDisconnect(conn);
}

void ConnectionListener::onExit( GNE::Connection& conn )
{
   mNetMgr->OnExit( conn );
}

void ConnectionListener::onNewConn( GNE::SyncConnection& conn)
{
   mNetMgr->OnNewConn(conn);
}

void ConnectionListener::onConnect( GNE::SyncConnection &conn )
{
   mNetMgr->OnConnect(conn);
}

void ConnectionListener::onReceive( GNE::Connection& conn )
{
   mNetMgr->OnReceive( conn );
}

void ConnectionListener::onFailure( GNE::Connection& conn, const GNE::Error& error )
{
   mNetMgr->OnFailure(conn, error);
}

void ConnectionListener::onError( GNE::Connection& conn, const GNE::Error& error )
{
   mNetMgr->OnError(conn, error);
}

void ConnectionListener::onConnectFailure( GNE::Connection &conn, const GNE::Error &error)
{
   mNetMgr->OnConnectFailure(conn, error);
}


