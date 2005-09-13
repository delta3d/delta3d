#include <dtNet/netmgr.h>
#include <dtUtil/log.h>

#include <gnelib.h>

using namespace dtNet;
using namespace dtUtil;


NetMgr::NetMgr(void)
{
   if (GNE::initGNE(NL_IP, atexit) )
   {
      LOG_ERROR("Can't initialize network");
   }
}

NetMgr::~NetMgr(void)
{
   GNE::shutdownGNE();
}
