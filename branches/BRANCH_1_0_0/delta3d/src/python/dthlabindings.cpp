// dthlabindings.cpp: Python bindings for dtHLA library.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"

#include "dtCore/dt.h"
#include "dtHLA/dthla.h"


// The individual class bindings

void initDISTypeBindings();
void initEntityBindings();
void initRTIConnectionBindings();


BOOST_PYTHON_MODULE(dtHLA)
{
   initDISTypeBindings();
   initEntityBindings();
   initRTIConnectionBindings();
}
