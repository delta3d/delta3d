// dtTerrainbindings.cpp: Python bindings for dtTerrain library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

using namespace boost::python;

void initGeoCoordinatesBindings();
void initTerrainBindings();

BOOST_PYTHON_MODULE(PyDtTerrain)
{
	initGeoCoordinatesBindings();
	initTerrainBindings();
}
