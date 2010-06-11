// dtdalbindings.cpp: Python bindings for dtDAL library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtUtil/enumeration.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>

using namespace boost::python;
using namespace dtDAL;

void initActorPropertyBindings();
void initActorProxyBindings();
void initDataTypeBindings();
void initMapBindings();
void initProjectBindings();

BOOST_PYTHON_MODULE(PyDtDAL)
{
   initActorPropertyBindings();
   initActorProxyBindings();
   initDataTypeBindings();
   initMapBindings();
   initProjectBindings();
}
