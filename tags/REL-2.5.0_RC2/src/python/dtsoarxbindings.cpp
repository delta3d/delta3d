// dtsoarxbindings.cpp: Python bindings for dtSOARX library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <soarx/dtsoarx.h>

void initSOARXTerrainBindings();

BOOST_PYTHON_MODULE(PyDtSOARX)
{
   initSOARXTerrainBindings();
}
