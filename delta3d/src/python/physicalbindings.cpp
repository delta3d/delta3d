// physicalbindings.cpp: Physical binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/physical.h>

using namespace boost::python;
using namespace dtCore;

class PhysicalWrap : public Physical
{
   public:

      PhysicalWrap(PyObject* self)
         : mSelf(self)
      {}

      virtual void PostPhysicsStepUpdate()
      {
         call_method<void>(mSelf, "PostPhysicsStepUpdate");
      }
      
      void DefaultPostPhysicsStepUpdate()
      {
         Physical::PostPhysicsStepUpdate();
      }
      
   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ED_overloads, EnableDynamics, 0, 1)

void initPhysicalBindings()
{
   void (Physical::*SetMass1)(const dMass*) = &Physical::SetMass;
   void (Physical::*SetMass2)(float) = &Physical::SetMass;
   
   void (Physical::*GetMass1)(dMass*) const = &Physical::GetMass;
   float (Physical::*GetMass2)() const = &Physical::GetMass;

   void (Physical::*SetCenterOfGravity1)(const osg::Vec3&) = &Physical::SetCenterOfGravity;
   void (Physical::*GetCenterOfGravity1)(osg::Vec3&) const = &Physical::GetCenterOfGravity;

   void (Physical::*SetInertiaTensor1)(const osg::Matrix& inertiaTensor) = &Physical::SetInertiaTensor;
   void (Physical::*GetInertiaTensor1)(osg::Matrix& mat) const = &Physical::GetInertiaTensor;

   class_<Physical, bases<Transformable>, dtCore::RefPtr<PhysicalWrap>, boost::noncopyable>("Physical", no_init)
      .def("GetGeomID", &Physical::GetGeomID, return_value_policy<return_opaque_pointer>())
      .def("SetBodyID", &Physical::SetBodyID)
      .def("GetBodyID", &Physical::GetBodyID, return_value_policy<return_opaque_pointer>())
      .def("EnableDynamics", &Physical::EnableDynamics, ED_overloads())
      .def("DynamicsEnabled", &Physical::DynamicsEnabled)
      .def("SetMass", SetMass1)
      .def("GetMass", GetMass1)
      .def("SetMass", SetMass2)
      .def("GetMass", GetMass2)
      .def("SetCenterOfGravity", SetCenterOfGravity1)
      .def("GetCenterOfGravity", GetCenterOfGravity1)
      .def("SetInertiaTensor", SetInertiaTensor1)
      .def("GetInertiaTensor", GetInertiaTensor1)
      .def("PostPhysicsStepUpdate", &Physical::PostPhysicsStepUpdate, &PhysicalWrap::DefaultPostPhysicsStepUpdate)
      .def("RenderCollisionGeometry", &Physical::RenderCollisionGeometry)
      ;
      
}
