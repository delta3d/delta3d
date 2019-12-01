#include <dtAI/astarwaypointutils.h>
#include <dtAI/astarconfig.h>
#include <boost/python.hpp>

using namespace boost::python;
using namespace dtAI;


void init_AStarConfigBindings()
{   

   class_<WaypointAStar::config_type>("AStarConfig")
      .def_readwrite("mTotalCost", &WaypointAStar::config_type::mTotalCost)      
      .def_readwrite("mTotalNodesExplored", &WaypointAStar::config_type::mTotalNodesExplored)
      .def_readwrite("mTotalTime", &WaypointAStar::config_type::mTotalTime)
      .def_readwrite("mNumIterations", &WaypointAStar::config_type::mNumIterations)
      .def_readwrite("mMaxNodesExplored", &WaypointAStar::config_type::mMaxNodesExplored)
      .def_readwrite("mMaxTime", &WaypointAStar::config_type::mMaxTime)
      .def_readwrite("mMaxCost", &WaypointAStar::config_type::mMaxCost)
      ;

}
