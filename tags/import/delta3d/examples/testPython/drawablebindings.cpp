// drawablebindings.cpp: Drawable binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

#include "drawable.h"

using namespace boost::python;
using namespace P51;

void initDrawableBindings()
{
   class_<Drawable, boost::noncopyable>("Drawable", no_init);
}