// drawablebindings.cpp: Drawable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "drawable.h"

using namespace boost::python;
using namespace dtCore;

void initDrawableBindings()
{
   class_<Drawable, boost::noncopyable>("Drawable", no_init)
      .def("AddedToScene", &Drawable::AddedToScene);
}
