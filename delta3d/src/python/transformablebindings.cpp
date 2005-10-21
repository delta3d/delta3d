// transformablebindings.cpp: Transformable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/transformable.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

class TransformableWrap : public Transformable, public wrapper<Transformable>
{
public:

   virtual void PrePhysicsStepUpdate()
   {
      if( override PrePhysicsStepUpdate = this->get_override("PrePhysicsStepUpdate") )
      {
         PrePhysicsStepUpdate();
      }
      Transformable::PrePhysicsStepUpdate();
   }

   void DefaultPrePhysicsStepUpdate()
   {
      this->Transformable::PrePhysicsStepUpdate();
   }

   virtual bool FilterContact(dContact* contact, Transformable* collider)
   {
      if( override FilterContact = this->get_override("FilterContact") )
      {
         return FilterContact(contact, collider);
      }
      return Transformable::FilterContact(contact, collider);
   }

   bool DefaultFilterContact(dContact* contact, Transformable* collider)
   {
      return this->Transformable::FilterContact(contact, collider);
   }

   virtual void PostPhysicsStepUpdate()
   {
      if( override PostPhysicsStepUpdate = this->get_override("PostPhysicsStepUpdate") )
      {
         PostPhysicsStepUpdate();
      }
      Transformable::PostPhysicsStepUpdate();
   }

   void DefaultPostPhysicsStepUpdate()
   {
      this->Transformable::PostPhysicsStepUpdate();
   }

};


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ST_overloads, SetTransform, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GT_overloads, GetTransform, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RPN_overloads, RenderProxyNode, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCS_overloads, SetCollisionSphere, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCB_overloads, SetCollisionBox, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCCC_overloads, SetCollisionCappedCylinder, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SCM_overloads, SetCollisionMesh, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RCG_overloads, RenderCollisionGeometry, 0, 1)

void initTransformableBindings()
{
   Transformable* (*TransformableGI1)(int) = &Transformable::GetInstance;
   Transformable* (*TransformableGI2)(std::string) = &Transformable::GetInstance;

   void (Transformable::*SetCollisionSphere1)(float) = &Transformable::SetCollisionSphere;
   void (Transformable::*SetCollisionSphere2)(osg::Node*) = &Transformable::SetCollisionSphere;

   void (Transformable::*SetCollisionBox1)(float, float, float) = &Transformable::SetCollisionBox;
   void (Transformable::*SetCollisionBox2)(osg::Node*) = &Transformable::SetCollisionBox;

   void (Transformable::*SetCollisionCappedCylinder1)(float, float) = &Transformable::SetCollisionCappedCylinder;
   void (Transformable::*SetCollisionCappedCylinder2)(osg::Node*) = &Transformable::SetCollisionCappedCylinder;

   scope Transformable_scope = class_< TransformableWrap, bases<DeltaDrawable>, RefPtr<TransformableWrap>, boost::noncopyable >("Transformable", no_init)
      .def("GetInstanceCount", &Transformable::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TransformableGI1, return_internal_reference<>())
      .def("GetInstance", TransformableGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddChild", &Transformable::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &Transformable::RemoveChild)
      .def("SetTransform", &Transformable::SetTransform, ST_overloads())
      .def("GetTransform", &Transformable::GetTransform, GT_overloads())
      .def("RenderProxyNode", &Transformable::RenderProxyNode, RPN_overloads())
      .def("SetNormalRescaling", &Transformable::SetNormalRescaling)
      .def("GetNormalRescaling", &Transformable::GetNormalRescaling)
      .def("SetCollisionGeom", &Transformable::SetCollisionGeom)
      .def("SetCollisionSphere", SetCollisionSphere1)
      .def("SetCollisionSphere", SetCollisionSphere2, SCS_overloads())
      .def("SetCollisionBox", SetCollisionBox1)
      .def("SetCollisionBox", SetCollisionBox2, SCB_overloads())
      .def("SetCollisionCappedCylinder", SetCollisionCappedCylinder1)
      .def("SetCollisionCappedCylinder", SetCollisionCappedCylinder2, SCCC_overloads())
      .def("SetCollisionRay", &Transformable::SetCollisionRay)
      .def("SetCollisionMesh", &Transformable::SetCollisionMesh, SCM_overloads())
      .def("ClearCollisionGeometry", &Transformable::ClearCollisionGeometry)
      .def("PrePhysicsStepUpdate", &Transformable::PrePhysicsStepUpdate, &TransformableWrap::DefaultPrePhysicsStepUpdate)
      .def("FilterContact", &Transformable::FilterContact, &TransformableWrap::DefaultFilterContact)
      .def("PostPhysicsStepUpdate", &Transformable::PostPhysicsStepUpdate, &TransformableWrap::DefaultPostPhysicsStepUpdate)
      .def("RenderCollisionGeometry", &Transformable::RenderCollisionGeometry)
      .def("GetRenderCollisionGeometry", &Transformable::GetRenderCollisionGeometry)
      ;

   enum_<Transformable::CoordSysEnum>("CoordSysEnum")
      .value("REL_CS", Transformable::REL_CS)
      .value("ABS_CS", Transformable::ABS_CS)
      .export_values();
}
