#include <dtAI/astarwaypointutils.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <vector>

using namespace boost::python;
using namespace dtAI;


typedef std::vector<const WaypointInterface*> WaypointVector;

class PythonAStar
{
public:
   void Reset1(const WaypointInterface* w1, const WaypointInterface* w2)
   {
      mAStar.Reset(w1, w2);
   }

   void Reset3(const std::vector<const WaypointInterface*>& pFrom, const std::vector<const WaypointInterface*>& pTo)
   {
      mAStar.Reset(pFrom, pTo);
   }

   void Reset2(const WaypointAStar::config_type& pConf)
   {
      mAStar.Reset(pConf);
   }

   PathFindResult FindPath()
   {
      return mAStar.FindPath();
   }

   WaypointVector GetPath()
   {
      return WaypointVector(mAStar.GetPath().begin(), mAStar.GetPath().end());
   }

   WaypointAStar::config_type& GetConfig()
   {
      return mAStar.GetConfig();
   }

private:
   WaypointAStar mAStar;
};


void init_WaypointAStarBindings()
{
   class_<WaypointVector>("WaypointVector")
      .def(vector_indexing_suite<WaypointVector>())
      ;

   scope waypointScope = class_<PythonAStar, PythonAStar*, boost::noncopyable>("WaypointAStar")
      .def("Reset", &PythonAStar::Reset1)
      .def("Reset", &PythonAStar::Reset2)
      .def("Reset", &PythonAStar::Reset3)
      .def("FindPath", &PythonAStar::FindPath)
      .def("GetPath", &PythonAStar::GetPath)
      .def("GetConfig", &PythonAStar::GetConfig, return_internal_reference<>())
   ;

   enum_<PathFindResult>("PathFindResult")
      .value("NO_PATH", NO_PATH)
      .value("PATH_FOUND", PATH_FOUND)
      .value("PARTIAL_PATH", PARTIAL_PATH)
      .export_values();
}
