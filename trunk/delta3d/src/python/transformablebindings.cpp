// transformablebindings.cpp: Transformable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/transformable.h"

using namespace boost::python;
using namespace dtCore;

class TransformableWrap : public Transformable
{
   public:

   TransformableWrap(PyObject* self)
      : mSelf(self)
   {}
      
   virtual void AddChild(DeltaDrawable* child)
   {
      call_method<void>(mSelf, "AddChild", child);
   }

   void DefaultAddChild(DeltaDrawable* child)
   {
      Transformable::AddChild(child);
   }

   virtual void RemoveChild( DeltaDrawable* child )
   {
      call_method<void>(mSelf, "RemoveChild", child);
   }

   void DefaultRemoveChild( DeltaDrawable* child )
   {
      Transformable::RemoveChild(child);
   }

   virtual void SetTransform( Transform *xform, CoordSysEnum cs=ABS_CS )
   {
      call_method<void>( mSelf, "SetTransform", xform, cs );
   }

   void DefaultSetTransform( Transform *xform, CoordSysEnum cs=ABS_CS )
   {
      Transformable::SetTransform( xform, cs );
   }

   virtual void GetTransform( Transform *xform, CoordSysEnum cs=ABS_CS  )
   {
      call_method<void>(mSelf, "GetTransform", xform, cs);
   }

   void DefaultGetTransform( Transform *xform, CoordSysEnum cs=ABS_CS  )
   {
      Transformable::GetTransform( xform, cs );
   }

protected:

   PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ST_overloads, SetTransform, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GT_overloads, GetTransform, 1, 2)

void initTransformableBindings()
{
   Transformable* (*TransformableGI1)(int) = &Transformable::GetInstance;
   Transformable* (*TransformableGI2)(std::string) = &Transformable::GetInstance;

   scope Transformable_scope = class_<Transformable, bases<Base>, osg::ref_ptr<TransformableWrap> >("Transformable", no_init)
      .def("GetInstanceCount", &Transformable::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", TransformableGI1, return_internal_reference<>())
      .def("GetInstance", TransformableGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("AddChild", &Transformable::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &Transformable::RemoveChild, &TransformableWrap::DefaultRemoveChild)
      .def("SetTransform", &Transformable::SetTransform, ST_overloads())
      .def("GetTransform", &Transformable::GetTransform, GT_overloads());

   enum_<Transformable::CoordSysEnum>("CoordSysEnum")
      .value("REL_CS", Transformable::REL_CS)
      .value("ABS_CS", Transformable::ABS_CS)
      .export_values();
}
