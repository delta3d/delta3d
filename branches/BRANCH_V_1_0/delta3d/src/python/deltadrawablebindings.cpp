// deltadrawablebindings.cpp: DeltaDrawable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtCore/deltadrawable.h"

using namespace boost::python;
using namespace dtCore;

class DeltaDrawableWrap : public DeltaDrawable
{
   public:

      DeltaDrawableWrap(PyObject* self)
         : mSelf(self)
      {}

      virtual void SetParent(DeltaDrawable* parent)
      {
          call_method<void>(mSelf, "SetParent", parent);
      }
      
      void DefaultSetParent(DeltaDrawable* parent)
      {
         DeltaDrawable::SetParent(parent);
      }

      virtual bool AddChild(DeltaDrawable* child)
      {
         return call_method<bool>(mSelf, "AddChild", child);
      }

      bool DefaultAddChild(DeltaDrawable* child)
      {
         return DeltaDrawable::AddChild(child);
      }

      virtual void RemoveChild( DeltaDrawable* child )
      {
         call_method<void>(mSelf, "RemoveChild", child);
      }

      void DefaultRemoveChild( DeltaDrawable* child )
      {
         DeltaDrawable::RemoveChild(child);
      }

      /*
      virtual void RenderProxyNode( const bool enable )
      {
         call_method<void>(mSelf, "RenderProxyNode", enable);
      }

      virutal void 
      */

   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RPN_overloads, RenderProxyNode, 0, 1)

void initDeltaDrawableBindings()
{
   class_<DeltaDrawable, bases<Base>, dtCore::RefPtr<DeltaDrawableWrap>, boost::noncopyable>("DeltaDrawable", no_init)
      .def("SetParent", &DeltaDrawable::SetParent, &DeltaDrawableWrap::DefaultSetParent)
      .def("AddChild", &DeltaDrawable::AddChild, with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &DeltaDrawable::RemoveChild, &DeltaDrawableWrap::DefaultRemoveChild)
      .def("GetParent", &DeltaDrawable::GetParent, return_internal_reference<>())
      .def("GetSceneParent", &DeltaDrawable::GetSceneParent, return_internal_reference<>())
      .def("GetNumChildren", &DeltaDrawable::GetNumChildren)
      .def("GetChild", &DeltaDrawable::GetChild, return_internal_reference<>())
      .def("GetChildIndex", &DeltaDrawable::GetChildIndex)
      .def("CanBeChild", &DeltaDrawable::CanBeChild)
      .def("RenderProxyNode", &DeltaDrawable::RenderProxyNode, RPN_overloads())
      .def("Emancipate", &DeltaDrawable::Emancipate);
}
