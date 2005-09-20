// dtabcbindings.cpp: Python bindings for dtABC library.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"

#include "dtCore/dt.h"
#include "dtABC/dtabc.h"


// The individual class bindings

void initBaseABCBindings();
void initApplicationBindings();
void initWeatherBindings();
void initWidgetBindings();


BOOST_PYTHON_MODULE(dtABC)
{
   initBaseABCBindings();
   initApplicationBindings();
   initWeatherBindings();
   initWidgetBindings();
}
