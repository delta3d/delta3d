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


//class PythonPlanner
//{
//public:
//   void Reset1(PlannerHelperPython* pHelper)
//   {
//      mPlanner.Reset(pHelper);
//   }
//
//   void Reset2(const PlannerConfig& pConf)
//   {
//      mPlanner.Reset(pConf);
//   }
//
//   Planner::PlannerResult GeneratePlan()
//   {
//      return mPlanner.GeneratePlan();
//   }
//
//   OperatorVector GetPlan()
//   {
//      if(mPlanner.GetPlan().size() == 0) return OperatorVector();
//      else return OperatorVector(mPlanner.GetPlan().begin(), mPlanner.GetPlan().end());
//   }
//
//   PlannerConfig& GetConfig()
//   {
//      return mPlanner.GetConfig();
//   }
//
//private:
//   Planner mPlanner;
//};


void (Planner::*Reset1)(const PlannerHelper*)= &Planner::Reset;
void (Planner::*Reset2)(const PlannerConfig&)= &Planner::Reset;
PlannerConfig& (Planner::*GetConfig1)()= &Planner::GetConfig;
const PlannerConfig& (Planner::*GetConfig2)() const = &Planner::GetConfig;


void init_PlannerBindings()
{   

   class_<PlannerConfig>("PlannerConfig")
      .def_readwrite("mMaxTimePerIteration", &PlannerConfig::mMaxTimePerIteration)
      .def_readwrite("mTotalElapsedTime", &PlannerConfig::mTotalElapsedTime)
      .def_readwrite("mCurrentElapsedTime", &PlannerConfig::mCurrentElapsedTime)
      ;


   scope plannerScope = class_<Planner, Planner*, boost::noncopyable>("Planner")
      .def("Reset", Reset1, with_custodian_and_ward<1,2>())
      .def("Reset", Reset2)
      .def("GeneratePlan", &Planner::GeneratePlan)
      .def("GetPlan", &Planner::GetPlanAsVector)
      .def("GetConfig", GetConfig1, return_internal_reference<>())
      .def("GetConfig", GetConfig2, return_internal_reference<>())
      ;

   class_<Planner::OperatorVector>("OperatorVector")
      .def(vector_indexing_suite<Planner::OperatorVector>())
      ;

   enum_<Planner::PlannerResult>("PlannerResult")
      .value("NO_PLAN", Planner::NO_PLAN)
      .value("PLAN_FOUND", Planner::PLAN_FOUND)
      .value("PARTIAL_PLAN", Planner::PARTIAL_PLAN)
      .export_values();
}
