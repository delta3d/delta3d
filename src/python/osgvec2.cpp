// Copyright (C) 2002-2003 Gideon May (gideon@computer.org)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.


//This code has been modified by Delta3D to work within our framework
//namely removing the namespace PyOsg and some fixes to help the VS Compiler


#include <osg/Vec2>

#include <string>
#include <iostream>
#include <sstream>

#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <python/heldptr.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
using namespace osg;
using namespace boost::python;
/// @endcond

namespace {

std::string osgVec2_repr(Vec2 * self)
{
    std::ostringstream ost;
    ost << "Vec2(" << self->_v[0] << "," << self->_v[1] << ")";
    return ost.str();
}

std::string osgVec2_str(Vec2 * self)
{
    std::ostringstream ost;
    ost << "(" << self->_v[0] << " " << self->_v[1] << ")";
    return ost.str();
}

inline void set(Vec2 * self, Vec2& from) { * self = from; }

void __setitem__(Vec2 * self, int idx, float val) { (*self)[idx] = val; }

inline float get_x(Vec2 * self) { return self->_v[0]; }
inline void  set_x(Vec2 * self, float x) { self->_v[0] = x; }

inline float get_y(Vec2 * self) { return self->_v[1]; }
inline void  set_y(Vec2 * self, float y) { self->_v[1] = y; }

} // namespace


void initOSGVec2()
{
    // Complete
    class_<Vec2>("Vec2")
         .def(init<float, float>())

         .def(self == self)
         .def(self != self)
         .def(self < self)

         .def("set", &Vec2::set)
         .def("set", &set)
         .def("__getitem__", (float &(Vec2::*)(int)) &Vec2::operator[], return_value_policy<copy_non_const_reference>())
         .def("__setitem__", &__setitem__)
         .add_property("_x", get_x, set_x)
         .add_property("_y", get_y, set_y)
         //.def("x", ( float (Vec2::*)() const) &Vec2::x)
         //.def("y", ( float (Vec2::*)() const) &Vec2::y)
         .def("valid", &Vec2::valid)
         .def("isNaN", &Vec2::isNaN)
         .def(self * self)
         .def(self * float())
         .def(self *= float())
         .def(self / float())
         .def(self /= float())
         .def(self + self)
         .def(self += self)
         .def(self - self)
         .def(self -= self)
         .def(-self)
         .def("length", &Vec2::length)
         .def("length2", &Vec2::length2)
         .def("normalize", &Vec2::normalize)
         .def("__str__", &osgVec2_str)
         .def("__repr__", &osgVec2_repr)
         ;
}


