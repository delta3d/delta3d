// dtsoarxbindings.cpp: Python bindings for dtSOARX library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"

#include "dt.h"
#include "soarx/dtsoarx.h"


// The individual class bindings

void initSOARXTerrainBindings();


BOOST_PYTHON_MODULE(dtSOARX)
{
   initSOARXTerrainBindings();
}
