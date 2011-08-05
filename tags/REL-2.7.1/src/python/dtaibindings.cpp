// dtaibindings.cpp: Python bindings for dtAI library.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

void init_WaypointBindings();
void init_WaypointManagerBindings();
void init_WaypointAStarBindings();
void init_WaypointPairBindings();
void init_AStarConfigBindings();

//void init_PlannerBindings();
//void init_PlannerHelperBindings();
//void init_NPCOperatorBindings();
//void init_ConditionalBindings();
//void init_WorldStateBindings();
//void init_StateVarBindings();

BOOST_PYTHON_MODULE(PyDtAI)
{
   //astar
   init_WaypointBindings();
   init_WaypointPairBindings();
   init_WaypointManagerBindings();
   init_WaypointAStarBindings();
   init_AStarConfigBindings();

   //planner
   //init_PlannerHelperBindings();
   //init_PlannerBindings();
   //init_WorldStateBindings();
   //init_NPCOperatorBindings();
   //init_ConditionalBindings();
   //init_StateVarBindings();
}
