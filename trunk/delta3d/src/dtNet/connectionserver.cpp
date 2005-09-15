#include <dtNet/connectionserver.h>
#include <dtNet/connectionlistener.h>
#include <dtUtil/log.h>

#include <gnelib/ConnectionParams.h>


using namespace dtNet;
using namespace dtUtil;

ConnectionServer::ConnectionServer(int inRate, int outRate):
   GNE::ServerConnectionListener(),
   mInRate(inRate),
   mOutRate(outRate)
{
}


ConnectionServer::~ConnectionServer(void)
{
   LOG_DEBUG("Destroying");
}


void ConnectionServer::onListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener)
{
   //mprintf("Connection error: %s\n", error.toString().c_str());
   //mprintf("  Error received from %s\n", from.toString().c_str());
   LOG_DEBUG("onListenFailure");
}

void ConnectionServer::onListenSuccess(const GNE::ConnectionListener::sptr &listener)
{
   LOG_DEBUG("onListenSuccess");
}

void ConnectionServer::getNewConnectionParams( GNE::ConnectionParams &params )
{
   params.setInRate(mInRate);
   params.setOutRate(mOutRate);
   params.setUnrel(true); //?
   params.setListener( ConnectionListener::create() );
}

