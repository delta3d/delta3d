#include <dtNet/connectionserver.h>
#include <dtNet/connectionlistener.h>
#include <dtUtil/log.h>

#include <gnelib/ConnectionParams.h>


using namespace dtNet;
using namespace dtUtil;

ConnectionServer::ConnectionServer(int inRate, int outRate, NetMgr *netMgr):
   GNE::ServerConnectionListener(),
   mInRate(inRate),
   mOutRate(outRate),
   mNetMgr(netMgr)
{
   LOG_DEBUG("Creating");
}


ConnectionServer::~ConnectionServer(void)
{
   LOG_DEBUG("Destroying");
}


void ConnectionServer::onListenFailure(const GNE::Error& error, const GNE::Address& from, const GNE::ConnectionListener::sptr& listener)
{
   mNetMgr->OnListenFailure( error, from, listener );
}

void ConnectionServer::onListenSuccess(const GNE::ConnectionListener::sptr &listener)
{
   if (mNetMgr.valid())  mNetMgr->OnListenSuccess();     
}

void ConnectionServer::getNewConnectionParams( GNE::ConnectionParams &params )
{
   params.setInRate(mInRate);
   params.setOutRate(mOutRate);
   params.setUnrel(true); //?
   params.setListener( ConnectionListener::create(mNetMgr.get()) );
}
