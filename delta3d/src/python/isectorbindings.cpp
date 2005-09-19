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

   void (Isector::*GetHitPoint)( osg::Vec3&, int ) const = &Isector::GetHitPoint;

   void (Isector::*SetDirection)( const osg::Vec3& ) = &Isector::SetDirection;

   void (Isector::*SetStartPosition)( const osg::Vec3& ) = &Isector::SetStartPosition;

   void (Isector::*SetEndPosition)( const osg::Vec3& ) = &Isector::SetEndPosition;

   class_<Isector, bases<Transformable>, dtCore::RefPtr<Isector> >("Isector", init<const osg::Vec3&, const osg::Vec3&>())
      .def("GetInstanceCount", &Isector::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", IsectorGI1, return_internal_reference<>())
      .def("GetInstance", IsectorGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("SetLength", &Isector::SetLength)
      .def("GetHitPoint", GetHitPoint)
      .def("SetDirection", SetDirection)
      .def("SetStartPosition", SetStartPosition)
      .def("SetEndPosition", SetEndPosition)
      .def("Update", &Isector::Update)
      .def("SetGeometry", &Isector::SetGeometry);
}
