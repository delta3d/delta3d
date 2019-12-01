// physicalbindings.cpp: Physical binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/physical.h>
#include <osg/MatrixTransform>
#include <ode/common.h>
#include <ode/mass.h>
#include <dtCore/odebodywrap.h>

using namespace boost::python;
using namespace dtCore;

class PhysicalWrap : public Physical
{
   public:
      PhysicalWrap(PyObject* self, const std::string &name="Physical")
         : Physical (name),
           mSelf(self) {}

      PhysicalWrap (PyObject* self, TransformableNode &node, const std::string &name="Physical")
         : Physical (node, name),
           mSelf (self) {}

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
BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(dxBody);

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

   const ODEBodyWrap* (Physical::*GetBodyWrapper1)() const = &Physical::GetBodyWrapper;
   ODEBodyWrap* (Physical::*GetBodyWrapper2) () = &Physical::GetBodyWrapper;

   class_<Physical, bases<Transformable>, dtCore::RefPtr<PhysicalWrap>, boost::noncopyable>("Physical", no_init)
      .def(init<optional<const std::string&> >())
      .def(init<Transformable::TransformableNode&, optional<const std::string&> >())
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
      .def("GetBodyWrapper", GetBodyWrapper1, return_internal_reference<>())
      .def("GetBodyWrapper", GetBodyWrapper2, return_internal_reference<>())
      ;


}
