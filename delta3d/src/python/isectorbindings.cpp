// isectorbindings.cpp: Isector binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/isector.h"

using namespace boost::python;
using namespace dtCore;

void initIsectorBindings()
{
   Isector* (*IsectorGI1)(int) = &Isector::GetInstance;
   Isector* (*IsectorGI2)(std::string) = &Isector::GetInstance;

   void (Isector::*GetHitPoint1)( osg::Vec3&, int ) const = &Isector::GetHitPoint;
   void (Isector::*GetHitPoint2)( sgVec3, int ) const = &Isector::GetHitPoint;

   void (Isector::*SetDirection1)( const osg::Vec3& ) = &Isector::SetDirection;
   void (Isector::*SetDirection2)( sgVec3 ) = &Isector::SetDirection;

   void (Isector::*SetStartPosition1)( const osg::Vec3& ) = &Isector::SetStartPosition;
   void (Isector::*SetStartPosition2)( sgVec3 ) = &Isector::SetStartPosition;

   void (Isector::*SetEndPosition1)( const osg::Vec3& ) = &Isector::SetEndPosition;
   void (Isector::*SetEndPosition2)( sgVec3 ) = &Isector::SetEndPosition;

   class_<Isector, bases<Transformable>, dtCore::RefPtr<Isector> >("Isector", init<const osg::Vec3&, const osg::Vec3&>())
      .def("GetInstanceCount", &Isector::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", IsectorGI1, return_internal_reference<>())
      .def("GetInstance", IsectorGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetLength", &Isector::SetLength)
      .def("GetHitPoint", GetHitPoint1)
      .def("GetHitPoint", GetHitPoint2)
      .def("SetDirection", SetDirection1)
      .def("SetDirection", SetDirection2)
      .def("SetStartPosition", SetStartPosition1)
      .def("SetStartPosition", SetStartPosition2)
      .def("SetEndPosition", SetEndPosition1)
      .def("SetEndPosition", SetEndPosition2)
      .def("Update", &Isector::Update)
      .def("SetGeometry", &Isector::SetGeometry);
}
