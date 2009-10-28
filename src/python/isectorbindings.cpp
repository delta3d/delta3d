// isectorbindings.cpp: Isector binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/isector.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GHP_overloads, GetHitPoint, 1, 2)

void initIsectorBindings()
{
   Isector* (*IsectorGI1)(int) = &Isector::GetInstance;
   Isector* (*IsectorGI2)(std::string) = &Isector::GetInstance;

   void (Isector::*SetDirection)( const osg::Vec3& ) = &Isector::SetDirection;

   void (Isector::*SetStartPosition)( const osg::Vec3& ) = &Isector::SetStartPosition;

   void (Isector::*SetEndPosition)( const osg::Vec3& ) = &Isector::SetEndPosition;

   DeltaDrawable* (Isector::*GetClosestDeltaDrawable1)() = &Isector::GetClosestDeltaDrawable;

   const DeltaDrawable* (Isector::*GetClosestDeltaDrawable2)() const = &Isector::GetClosestDeltaDrawable;

   class_<Isector, bases<Transformable>, dtCore::RefPtr<Isector>, boost::noncopyable >("Isector", init<optional<Scene*> >() )
      .def(init<const osg::Vec3&, const osg::Vec3&, optional<Scene*> >())
      .def(init<Scene*, const osg::Vec3&, const osg::Vec3&>())
      .def("GetInstanceCount", &Isector::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", IsectorGI1, return_internal_reference<>())
      .def("GetInstance", IsectorGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetGeometry", &Isector::SetGeometry)
      .def("ClearQueryRoot", &Isector::ClearQueryRoot)
      .def("SetStartPosition", SetStartPosition)
      .def("GetStartPosition", &Isector::GetStartPosition, return_internal_reference<>())
      .def("SetEndPosition", SetEndPosition)
      .def("SetDirection", SetDirection)
      .def("GetDirection", &Isector::GetDirection, return_internal_reference<>())
      .def("SetLength", &Isector::SetLength)
      .def("GetHitPoint", &Isector::GetHitPoint, GHP_overloads())
      .def("GetNumberOfHits", &Isector::GetNumberOfHits)
      .def("Update", &Isector::Update)
      .def("Reset", &Isector::Reset)
      .def("SetScene", &Isector::SetScene)
      .def("GetClosestDeltaDrawable", GetClosestDeltaDrawable1, return_internal_reference<>())
      .def("GetClosestDeltaDrawable", GetClosestDeltaDrawable2, return_internal_reference<>())
      .def("SetGeometry", &Isector::SetGeometry);
}
