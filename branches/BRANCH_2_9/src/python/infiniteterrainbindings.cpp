// infiniteterrainbindings.cpp: InfiniteTerrain binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/infiniteterrain.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GH_overloads, GetHeight, 2, 3)

void initInfiniteTerrainBindings()
{
   InfiniteTerrain* (*InfiniteTerrainGI1)(int) = &InfiniteTerrain::GetInstance;
   InfiniteTerrain* (*InfiniteTerrainGI2)(std::string) = &InfiniteTerrain::GetInstance;

   //hmm, this won't compile with the overloaded bool default parameters, i can't call
   //BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS because there are actually 4 versions.
   //when we deprecated sg for real, we can replace these function pointers with 
   //BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS again. -osb

   //void (InfiniteTerrain::*GetNormal1)(float, float, osg::Vec3&) = &InfiniteTerrain::GetNormal;
   void (InfiniteTerrain::*GetNormal2)(float, float, osg::Vec3&, bool) = &InfiniteTerrain::GetNormal;
   //void (InfiniteTerrain::*GetNormal3)(float, float, sgVec3) = &InfiniteTerrain::GetNormal;

   class_<InfiniteTerrain, bases<Transformable>, dtCore::RefPtr<InfiniteTerrain>, boost::noncopyable >("InfiniteTerrain", init<optional<const std::string&> >())
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
      .def( "IsClearLineOfSight", &InfiniteTerrain::IsClearLineOfSight )
      .def( "SetLineOfSightSpacing", &InfiniteTerrain::SetLineOfSightSpacing )
      .def( "GetLineOfSightSpacing", &InfiniteTerrain::GetLineOfSightSpacing )
      //.def("GetNormal", GetNormal1)
      .def("GetNormal", GetNormal2);
      //.def("GetNormal", GetNormal3)
}
