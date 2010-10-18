// transformablebindings.cpp: Transformable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/transformable.h>
#include <dtCore/transform.h>
#include <osg/MatrixTransform>
#include <dtCore/scene.h>
#include <ode/contact.h> //for dContact

using namespace boost::python;
using namespace dtCore;

class TransformableWrap : public Transformable, public wrapper<Transformable>
{
public:
   TransformableWrap (const std::string &name="Transformable")      
      : Transformable (name) {}

   TransformableWrap (TransformableNode &node, const std::string &name="Transformable")
      : Transformable (node, name) {}

   virtual void PrePhysicsStepUpdate()
   {
      if( override PrePhysicsStepUpdate = this->get_override("PrePhysicsStepUpdate") )
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<void>( PrePhysicsStepUpdate.ptr() );
         #else
         PrePhysicsStepUpdate();
         #endif
      }
      else
      {
         Transformable::PrePhysicsStepUpdate();
      }
   }

   void DefaultPrePhysicsStepUpdate()
   {
      this->Transformable::PrePhysicsStepUpdate();
   }

   virtual bool FilterContact(dContact* contact, Transformable* collider)
   {
      if( override FilterContact = this->get_override("FilterContact") )
      {
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         return call<bool>( FilterContact.ptr(), contact, collider );
         #else
         return FilterContact(contact, collider);
         #endif
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
         #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<void>( PostPhysicsStepUpdate.ptr() );
         #else
         PostPhysicsStepUpdate();
         #endif
      }
      else
      {
         Transformable::PostPhysicsStepUpdate();
      }
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

BOOST_PYTHON_OPAQUE_SPECIALIZED_TYPE_ID(dxGeom);

void initTransformableBindings()
{
   Transformable* (*TransformableGI1)(int) = &Transformable::GetInstance;
   Transformable* (*TransformableGI2)(std::string) = &Transformable::GetInstance;

   osg::Node* (Transformable::*GetOSGNode1)() = &Transformable::GetOSGNode;
   const osg::Node * (Transformable::*GetOSGNode2)() const = &Transformable::GetOSGNode;

   void (Transformable::*SetCollisionSphere1)(float) = &Transformable::SetCollisionSphere;
   void (Transformable::*SetCollisionSphere2)(osg::Node*) = &Transformable::SetCollisionSphere;

   void (Transformable::*SetCollisionBox1)(float, float, float) = &Transformable::SetCollisionBox;
   void (Transformable::*SetCollisionBox2)(osg::Node*) = &Transformable::SetCollisionBox;

   void (Transformable::*SetCollisionCappedCylinder1)(float, float) = &Transformable::SetCollisionCappedCylinder;
   void (Transformable::*SetCollisionCappedCylinder2)(osg::Node*) = &Transformable::SetCollisionCappedCylinder;
   void (Transformable::*SetTransformRef)(const Transform&, Transformable::CoordSysEnum ) = &Transformable::SetTransform;
   void (Transformable::*GetTransformRef)(Transform&, Transformable::CoordSysEnum ) const = &Transformable::GetTransform;

   scope Transformable_scope = class_< TransformableWrap, bases<DeltaDrawable>, dtCore::RefPtr<TransformableWrap>, boost::noncopyable >("Transformable", init<optional<const std::string&> >())
      .def(init<optional<const std::string&> >())
      .def(init<Transformable::TransformableNode&, optional<const std::string&> >())
      .def("GetInstanceCount", &Transformable::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TransformableGI1, return_internal_reference<>())
      .def("GetInstance", TransformableGI2, return_internal_reference<>())
      .staticmethod("GetInstance")   
      .def("AddChild", &Transformable::AddChild /*, with_custodian_and_ward<1, 2>() */)
      .def("RemoveChild", &Transformable::RemoveChild /*, &TransformableWrap2::DefaultRemoveChild */)   
      .def("SetParent", &Transformable::SetParent /*, &TransformableWrap2::DefaultSetParent */)   
      .def("SetTransform", SetTransformRef, ST_overloads())
      .def("GetTransform", GetTransformRef, GT_overloads())
      .def("RenderProxyNode", &Transformable::RenderProxyNode, RPN_overloads())
      .def("SetNormalRescaling", &Transformable::SetNormalRescaling)
      .def("GetNormalRescaling", &Transformable::GetNormalRescaling)
      .def("GetGeomID", &Transformable::GetGeomID, return_value_policy<return_opaque_pointer>())
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
      .def("GetOSGNode", GetOSGNode1, return_value_policy<reference_existing_object>())
      .def("GetOSGNode", GetOSGNode2, return_value_policy<reference_existing_object>())
      ;   

   enum_<Transformable::CoordSysEnum>("CoordSysEnum")
      .value("REL_CS", Transformable::REL_CS)
      .value("ABS_CS", Transformable::ABS_CS)
      .export_values();
}
