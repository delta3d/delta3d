// dtdalbindings.cpp: Python bindings for dtCore library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtUtil/enumeration.h>
#include <dtCore/project.h>
#include <dtCore/map.h>

using namespace boost::python;
using namespace dtCore;

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
