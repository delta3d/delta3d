#include <dtDAL/warndtdal.h>
#include <dtUtil/log.h>


//////////////////////////////////////////////////////////////////////////
WarnDTDAL::WarnDTDAL()
{
   LOGN_WARNING("dtDAL", "The dtDAL library is deprecated, and will eventually be removed.  You should change your build system not to link to this library.");
}
