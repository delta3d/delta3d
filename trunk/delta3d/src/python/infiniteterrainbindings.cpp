// infiniteterrainbindings.cpp: InfiniteTerrain binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "infiniteterrain.h"

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GH_overloads, GetHeight, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GN_overloads, GetNormal, 3, 4)

void initInfiniteTerrainBindings()
{
   InfiniteTerrain* (*InfiniteTerrainGI1)(int) = &InfiniteTerrain::GetInstance;
   InfiniteTerrain* (*InfiniteTerrainGI2)(std::string) = &InfiniteTerrain::GetInstance;

   class_<InfiniteTerrain, bases<Base, Drawable, Physical>, osg::ref_ptr<InfiniteTerrain> >("InfiniteTerrain", init<optional<std::string> >())
      .def("GetInstanceCount", &InfiniteTerrain::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", InfiniteTerrainGI1, return_internal_reference<>())
      .def("GetInstance", InfiniteTerrainGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("Regenerate", &InfiniteTerrain::Regenerate)
      .def("SetSegmentSize", &InfiniteTerrain::SetSegmentSize)
      .def("GetSegmentSize", &InfiniteTerrain::GetSegmentSize)
      .def("SetSegmentDivisions", &InfiniteTerrain::SetSegmentDivisions)
      .def("GetSegmentDivisions", &InfiniteTerrain::GetSegmentDivisions)
      .def("SetHorizontalScale", &InfiniteTerrain::SetHorizontalScale)
      .def("GetHorizontalScale", &InfiniteTerrain::GetHorizontalScale)
      .def("SetVerticalScale", &InfiniteTerrain::SetVerticalScale)
      .def("GetVerticalScale", &InfiniteTerrain::GetVerticalScale)
      .def("SetBuildDistance", &InfiniteTerrain::SetBuildDistance)
      .def("GetBuildDistance", &InfiniteTerrain::GetBuildDistance)
      .def("EnableSmoothCollisions", &InfiniteTerrain::EnableSmoothCollisions)
      .def("SmoothCollisionsEnabled", &InfiniteTerrain::SmoothCollisionsEnabled)
      .def("GetHeight", &InfiniteTerrain::GetHeight, GH_overloads())
      .def("GetNormal", &InfiniteTerrain::GetNormal, GN_overloads());
}