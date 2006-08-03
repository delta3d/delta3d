#include <dtAI/planner.h>
#include <dtAI/plannerhelper.h>
#include <dtAI/npcoperator.h>
#include <dtAI/worldstate.h>
#include <dtAI/plannerconfig.h>

#include <python/plannerhelperpython.h>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>

using namespace boost::python;
using namespace dtAI;


typedef std::vector<const NPCOperator*> OperatorVector;

class PythonPlanner
{
public:
   void Reset1(PlannerHelperPython* pHelper)
   {
      mPlanner.Reset(pHelper->GetHelper());
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
      if(mPlanner.GetPlan().size() == 0) return OperatorVector();
      else return OperatorVector(mPlanner.GetPlan().begin(), mPlanner.GetPlan().end());
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
      ;


   scope plannerScope = class_<PythonPlanner, PythonPlanner*, boost::noncopyable>("Planner")
      .def("Reset", &PythonPlanner::Reset1)
      .def("Reset", &PythonPlanner::Reset2)
      .def("GeneratePlan", &PythonPlanner::GeneratePlan)
      .def("GetPlan", &PythonPlanner::GetPlan)
      .def("GetConfig", &PythonPlanner::GetConfig, return_internal_reference<>())
      ;

   class_<OperatorVector>("OperatorList")
      .def(vector_indexing_suite<OperatorVector>())
      ;

   enum_<Planner::PlannerResult>("PlannerResult")
      .value("NO_PLAN", Planner::NO_PLAN)
      .value("PLAN_FOUND", Planner::PLAN_FOUND)
      .value("PARTIAL_PLAN", Planner::PARTIAL_PLAN)
      .export_values();
}
