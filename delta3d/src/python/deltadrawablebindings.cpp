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

      virtual void AddedToScene(Scene* scene)
      {
         call_method<void>(mSelf, "AddedToScene", scene);
      }

      void DefaultAddedToScene(Scene* scene)
      {
         DeltaDrawable::AddedToScene(scene);
      }

      virtual void SetParent(DeltaDrawable* parent)
      {
          call_method<void>(mSelf, "SetParent", parent);
      }
      
      void DefaultSetParent(DeltaDrawable* parent)
      {
         DeltaDrawable::SetParent(parent);
      }

      virtual void AddChild(DeltaDrawable* child)
      {
         call_method<void>(mSelf, "AddChild", child);
      }

      void DefaultAddChild(DeltaDrawable* child)
      {
         DeltaDrawable::AddChild(child);
      }

      virtual void RemoveChild( DeltaDrawable* child )
      {
         call_method<void>(mSelf, "RemoveChild", child);
      }

      void DefaultRemoveChild( DeltaDrawable* child )
      {
         DeltaDrawable::RemoveChild(child);
      }

   protected:

      PyObject* mSelf;
};

void initDeltaDrawableBindings()
{
   class_<DeltaDrawable, bases<Base>, osg::ref_ptr<DeltaDrawableWrap>, boost::noncopyable>("DeltaDrawable", no_init)
      .def("AddedToScene", &DeltaDrawable::AddedToScene, &DeltaDrawableWrap::DefaultAddedToScene) 
      .def("SetParent", &DeltaDrawable::SetParent, &DeltaDrawableWrap::DefaultSetParent)
      .def("AddChild", &DeltaDrawable::AddChild, , with_custodian_and_ward<1, 2>())
      .def("RemoveChild", &DeltaDrawable::RemoveChild, &DeltaDrawableWrap::DefaultRemoveChild)
      .def("GetParent", &DeltaDrawable::GetParent, return_internal_reference<>())
      .def("GetSceneParent", &DeltaDrawable::GetSceneParent, return_internal_reference<>())
      .def("GetNumChildren", &DeltaDrawable::GetNumChildren)
      .def("GetChild", &DeltaDrawable::GetChild, return_internal_reference<>())
      .def("GetChildIndex", &DeltaDrawable::GetChildIndex)
      .def("CanBeChild", &DeltaDrawable::CanBeChild);
}
