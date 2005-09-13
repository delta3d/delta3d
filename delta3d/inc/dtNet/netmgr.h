#ifndef NETMGR_INCLUDED
#define NETMGR_INCLUDED



#include <dtCore/base.h>

namespace dtNet
{

   class DT_EXPORT NetMgr :  public dtCore::Base
   {
   public:
      NetMgr(void);
      ~NetMgr(void);
   };


}

#endif