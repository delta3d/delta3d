#include <dtAI/planner.h>
#include <dtAI/plannerhelper.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>
#include <dtAI/plannerconfig.h>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>

using namespace boost::python;
using namespace dtAI;


typedef std::vector<const NPCOperator*> OperatorVector;

class PythonPlanner
{
public:
   void Reset1(const WorldState* pState, const PlannerHelper* pHelper)
   {
      mPlanner.Reset(pState, pHelper);
   }

   void Reset2(const PlannerConfig& pConf)
   {
      mPlanner.Reset(pConf);
   }

   Planner::PlannerResult GeneratePlan()
   {
      return mPlanner.GeneratePlan();
   }

   OperatorVector GetPlan()
   {
      return OperatorVector(mPlanner.GetPlan().begin(), mPlanner.GetPlan().end());
   }

   PlannerConfig& GetConfig()
   {
      return mPlanner.GetConfig();
   }

private:
   Planner mPlanner;
};


void init_PlannerBindings()
{   

   class_<PlannerConfig>("PlannerConfig")
      .def_readwrite("mMaxTimePerIteration", &PlannerConfig::mMaxTimePerIteration)
      .def_readwrite("mTotalElapsedTime", &PlannerConfig::mTotalElapsedTime)
      .def_readwrite("mCurrentElapsedTime", &PlannerConfig::mCurrentElapsedTime)
      .def_readwrite("mGoal", &PlannerConfig::mGoal)
      ;


   class_<OperatorVector>("OperatorVector")
      .def(vector_indexing_suite<OperatorVector>())
      ;

   scope plannerScope = class_<PythonPlanner, PythonPlanner*, boost::noncopyable>("Planner")
      .def("Reset", &PythonPlanner::Reset1)
      .def("Reset", &PythonPlanner::Reset2)
      .def("FindPath", &PythonPlanner::GeneratePlan)
      .def("GetPath", &PythonPlanner::GetPlan)
      .def("GetConfig", &PythonPlanner::GetConfig, return_internal_reference<>())
      ;

   enum_<Planner::PlannerResult>("PlannerResult")
      .value("NO_PLAN", Planner::NO_PLAN)
      .value("PLAN_FOUND", Planner::PLAN_FOUND)
      .value("PARTIAL_PLAN", Planner::PARTIAL_PLAN)
      .export_values();
}
