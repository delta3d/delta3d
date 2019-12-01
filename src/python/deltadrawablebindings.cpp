// deltadrawablebindings.cpp: DeltaDrawable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>

#include <osg/BoundingBox>
//#include <osgViewer/GraphicsWindow>

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

      osg::BoundingBox* GetBoundingBoxWrap()
      {
         osg::BoundingBox* bb = new osg::BoundingBox();         

         *bb = this->GetBoundingBox();

         return bb;
      }

   protected:

      PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(RPN_overloads, RenderProxyNode, 0, 1)

void initDeltaDrawableBindings()
{
   DeltaDrawable* (DeltaDrawable::*GetParent1)() = &DeltaDrawable::GetParent;
   const DeltaDrawable* (DeltaDrawable::*GetParent2)() const = &DeltaDrawable::GetParent;

   Scene* (DeltaDrawable::*GetSceneParent1)() = &DeltaDrawable::GetSceneParent;
   const Scene* (DeltaDrawable::*GetSceneParent2)() const = &DeltaDrawable::GetSceneParent;

   DeltaDrawable* (DeltaDrawable::*GetChild1)(unsigned int) = &DeltaDrawable::GetChild;
   const DeltaDrawable* (DeltaDrawable::*GetChild2)(unsigned int) const = &DeltaDrawable::GetChild;

   class_<DeltaDrawable, bases<Base>, dtCore::RefPtr<DeltaDrawableWrap>, boost::noncopyable>("DeltaDrawable", no_init)
      .def("SetParent", &DeltaDrawable::SetParent/*, &DeltaDrawableWrap::DefaultSetParent*/)
      .def("AddChild", &DeltaDrawable::AddChild/*, with_custodian_and_ward<1, 2>()*/)
      .def("RemoveChild", &DeltaDrawable::RemoveChild/*, &DeltaDrawableWrap::DefaultRemoveChild*/)
      .def("GetParent", GetParent1, return_internal_reference<>())
      .def("GetParent", GetParent2, return_internal_reference<>())
      .def("GetSceneParent", GetSceneParent1, return_internal_reference<>())
      .def("GetSceneParent", GetSceneParent2, return_internal_reference<>())
      .def("GetNumChildren", &DeltaDrawable::GetNumChildren)
      .def("GetChild", GetChild1, return_internal_reference<>())
	   .def("GetChild", GetChild2, return_internal_reference<>())
      .def("GetChildIndex", &DeltaDrawable::GetChildIndex)
      .def("CanBeChild", &DeltaDrawable::CanBeChild)
      .def("RenderProxyNode", &DeltaDrawable::RenderProxyNode, RPN_overloads())
      .def("Emancipate", &DeltaDrawable::Emancipate)
      .def("GetBoundingSphere", &DeltaDrawable::GetBoundingSphere)      
      .def("GetBoundingBox", &DeltaDrawableWrap::GetBoundingBoxWrap, return_internal_reference<>())
      .def("SetActive", &DeltaDrawable::SetActive)
      .def("GetActive", &DeltaDrawable::GetActive)
      ;
}
