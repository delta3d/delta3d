// physicalbindings.cpp: Physical binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/physical.h"

using namespace boost::python;
using namespace dtCore;

class PhysicalWrap : public Physical
{
   public:

      PhysicalWrap(PyObject* self)
         : mSelf(self)
      {}

      virtual void PrePhysicsStepUpdate()
      {
         call_method<void>(mSelf, "PrePhysicsStepUpdate"); 
      }

      void DefaultPrePhysicsStepUpdate()
      {
         Physical::PrePhysicsStepUpdate();
      }
      
      virtual bool FilterContact(dContact* contact, Physical* collider)
      {
         return call_method<bool>(mSelf, "FilterContact", contact, collider);
      }

      bool DefaultFilterContact(dContact* contact, Physical* collider)
      {
         return Physical::FilterContact(contact, collider);
      }
      
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

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCS_overloads, SetCollisionSphere, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCB_overloads, SetCollisionBox, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCCC_overloads, SetCollisionCappedCylinder, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCM_overloads, SetCollisionMesh, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ED_overloads, EnableDynamics, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RCG_overloads, RenderCollisionGeometry, 0, 1)

void initPhysicalBindings()
{
   void (Physical::*SetCollisionSphere1)(float) = &Physical::SetCollisionSphere;
   void (Physical::*SetCollisionSphere2)(osg::Node*) = &Physical::SetCollisionSphere;
   
   void (Physical::*SetCollisionBox1)(float, float, float) = &Physical::SetCollisionBox;
   void (Physical::*SetCollisionBox2)(osg::Node*) = &Physical::SetCollisionBox;
   
   void (Physical::*SetCollisionCappedCylinder1)(float, float) = &Physical::SetCollisionCappedCylinder;
   void (Physical::*SetCollisionCappedCylinder2)(osg::Node*) = &Physical::SetCollisionCappedCylinder;
   
   void (Physical::*SetMass1)(const dMass*) = &Physical::SetMass;
   void (Physical::*SetMass2)(float) = &Physical::SetMass;
   
   void (Physical::*GetMass1)(dMass*) const = &Physical::GetMass;
   float (Physical::*GetMass2)() const = &Physical::GetMass;

   void (Physical::*SetCenterOfGravity1)(const osg::Vec3&) = &Physical::SetCenterOfGravity;
   void (Physical::*SetCenterOfGravity2)(const sgVec3) = &Physical::SetCenterOfGravity;

   void (Physical::*GetCenterOfGravity1)(osg::Vec3&) const = &Physical::GetCenterOfGravity;
   void (Physical::*GetCenterOfGravity2)(sgVec3) const = &Physical::GetCenterOfGravity;

   void (Physical::*SetInertiaTensor1)(const osg::Matrix& inertiaTensor) = &Physical::SetInertiaTensor;
   void (Physical::*SetInertiaTensor2)(const sgMat3 inertiaTensor) = &Physical::SetInertiaTensor;

   void (Physical::*GetInertiaTensor1)(osg::Matrix& mat) const = &Physical::GetInertiaTensor;
   void (Physical::*GetInertiaTensor2)(sgMat3 dest) const = &Physical::GetInertiaTensor;

   class_<Physical, bases<Transformable>, dtCore::RefPtr<PhysicalWrap>, boost::noncopyable>("Physical", no_init)
      .def("GetGeomID", &Physical::GetGeomID, return_value_policy<return_opaque_pointer>())
      .def("SetBodyID", &Physical::SetBodyID)
      .def("GetBodyID", &Physical::GetBodyID, return_value_policy<return_opaque_pointer>())
      .def("SetCollisionGeom", &Physical::SetCollisionGeom)
      .def("SetCollisionSphere", SetCollisionSphere1)
      .def("SetCollisionSphere", SetCollisionSphere2, SCS_overloads())
      .def("SetCollisionBox", SetCollisionBox1)
      .def("SetCollisionBox", SetCollisionBox2, SCB_overloads())
      .def("SetCollisionCappedCylinder", SetCollisionCappedCylinder1)
      .def("SetCollisionCappedCylinder", SetCollisionCappedCylinder2, SCCC_overloads())
      .def("SetCollisionRay", &Physical::SetCollisionRay)
      .def("SetCollisionMesh", &Physical::SetCollisionMesh, SCM_overloads())
      .def("ClearCollisionGeometry", &Physical::ClearCollisionGeometry)
      .def("EnableDynamics", &Physical::EnableDynamics, ED_overloads())
      .def("DynamicsEnabled", &Physical::DynamicsEnabled)
      .def("SetMass", SetMass1)
      .def("GetMass", GetMass1)
      .def("SetMass", SetMass2)
      .def("GetMass", GetMass2)
      .def("SetCenterOfGravity", SetCenterOfGravity1)
      .def("SetCenterOfGravity", SetCenterOfGravity2)
      .def("GetCenterOfGravity", GetCenterOfGravity1)
      .def("GetCenterOfGravity", GetCenterOfGravity2)
      .def("SetInertiaTensor", SetInertiaTensor1)
      .def("SetInertiaTensor", SetInertiaTensor2)
      .def("GetInertiaTensor", GetInertiaTensor1)
      .def("GetInertiaTensor", GetInertiaTensor2)
      .def("PrePhysicsStepUpdate", &Physical::PrePhysicsStepUpdate, &PhysicalWrap::DefaultPrePhysicsStepUpdate)
      .def("FilterContact", &Physical::FilterContact, &PhysicalWrap::DefaultFilterContact)
      .def("PostPhysicsStepUpdate", &Physical::PostPhysicsStepUpdate, &PhysicalWrap::DefaultPostPhysicsStepUpdate)
      .def("RenderCollisionGeometry", &Physical::RenderCollisionGeometry)
      .def("GetRenderCollisionGeometry", &Physical::GetRenderCollisionGeometry);
      
}
