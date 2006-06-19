// dthlabindings.cpp: Python bindings for dtHLA library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtCore/dt.h>
#include <dtHLA/dthla.h>

void initDISTypeBindings();
void initEntityBindings();
void initRTIConnectionBindings();

BOOST_PYTHON_MODULE(PyDtHLA)
{
   initDISTypeBindings();
   initEntityBindings();
   initRTIConnectionBindings();
}
