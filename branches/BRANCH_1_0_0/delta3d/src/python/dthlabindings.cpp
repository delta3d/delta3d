// dthlabindings.cpp: Python bindings for dtHLA library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"

#include "dt.h"
#include "dthla.h"


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
