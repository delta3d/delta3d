// dtabcbindings.cpp: Python bindings for dtABC library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"

#include "dt.h"
#include "dtabc.h"


// The individual class bindings

void initBaseABCBindings();
void initApplicationBindings();
void initWeatherBindings();

BOOST_PYTHON_MODULE(dtABC)
{
   initBaseABCBindings();
   initApplicationBindings();
   initWeatherBindings();
}
