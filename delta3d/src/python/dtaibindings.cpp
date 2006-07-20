// dtaibindings.cpp: Python bindings for dtAI library.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

void init_WaypointBindings();
void init_WaypointManagerBindings();
void init_WaypointAStarBindings();
void init_WaypointPairBindings();

BOOST_PYTHON_MODULE(PyDtAI)
{
   init_WaypointBindings();
   init_WaypointPairBindings();
   init_WaypointManagerBindings();
   init_WaypointAStarBindings();
}
