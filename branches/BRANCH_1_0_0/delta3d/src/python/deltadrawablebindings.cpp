// deltadrawablebindings.cpp: DeltaDrawable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "deltadrawable.h"

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
         call_method<void>(mSelf, "AddedToScene");
      }

   protected:

      PyObject* mSelf;
};

void initDeltaDrawableBindings()
{
   class_<DeltaDrawable, DeltaDrawableWrap, boost::noncopyable>("DeltaDrawable", no_init)
      .def("AddedToScene", &DeltaDrawable::AddedToScene);
}
