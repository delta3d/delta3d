// dtabcbindings.cpp: Python bindings for dtABC library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"


// The individual class bindings

void initBaseABCBindings();
void initApplicationBindings();


BOOST_PYTHON_MODULE(dtABC)
{
   initBaseABCBindings();
   initApplicationBindings();
}