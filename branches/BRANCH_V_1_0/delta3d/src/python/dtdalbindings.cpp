// dtdalbindings.cpp: Python bindings for dtDAL library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>

using namespace boost::python;
using namespace dtDAL;

void initActorProxyBindings();
void initMapBindings();
void initProjectBindings();

BOOST_PYTHON_MODULE(PyDtDAL)
{
   initActorProxyBindings();
   initMapBindings();
   initProjectBindings();
}
