// drawablebindings.cpp: Drawable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "dtpython.h"
#include "drawable.h"

using namespace boost::python;
using namespace dtCore;

class DrawableWrap : public Drawable
{
   public:

      DrawableWrap(PyObject* self)
         : mSelf(self)
      {}

      virtual void AddedToScene(Scene* scene)
      {
         call_method<void>(mSelf, "AddedToScene");
      }

   protected:

      PyObject* mSelf;
};

void initDrawableBindings()
{
   class_<Drawable, DrawableWrap, boost::noncopyable>("Drawable", no_init)
      .def("AddedToScene", &Drawable::AddedToScene);
}
