// dtcharbindings.cpp: Python bindings for dtChar library.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"

#include "dt.h"
#include "dtchar.h"


// The individual class bindings

void initCharacterBindings();


BOOST_PYTHON_MODULE(dtChar)
{
   initCharacterBindings();
}