// dtsoarxbindings.cpp: Python bindings for dtSOARX library.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"

#include "dtCore/dt.h"
#include "soarx/dtsoarx.h"


// The individual class bindings

void initSOARXTerrainBindings();


BOOST_PYTHON_MODULE(dtSOARX)
{
   initSOARXTerrainBindings();
}
