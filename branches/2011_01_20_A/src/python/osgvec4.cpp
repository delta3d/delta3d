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


#include <osg/Vec4>

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
std::string osgVec4_repr(Vec4 * self)
{
    std::ostringstream ost;
    ost << "Vec4(" << self->_v[0] << "," << self->_v[1] << "," << self->_v[2] << "," << self->_v[3] << ")";
    return ost.str();
}

std::string osgVec4_str(Vec4 * self)
{
    std::ostringstream ost;
    ost << "(" << self->_v[0] << " " << self->_v[1] << " " << self->_v[2] << " " << self->_v[3] << ")";
    return ost.str();
}

inline void set(Vec4 * self, Vec4& from) { * self = from; }

inline void __setitem__(Vec4 * self, int idx, float val) { (*self)[idx] = val; }

inline float get_x(Vec4 * self) { return self->_v[0]; }
inline void  set_x(Vec4 * self, float x) { self->_v[0] = x; }

inline float get_y(Vec4 * self) { return self->_v[1]; }
inline void  set_y(Vec4 * self, float y) { self->_v[1] = y; }

inline float get_z(Vec4 * self) { return self->_v[2]; }
inline void  set_z(Vec4 * self, float z) { self->_v[2] = z; }

inline float get_w(Vec4 * self) { return self->_v[3]; }
inline void  set_w(Vec4 * self, float z) { self->_v[3] = z; }

} // namespace

void initOSGVec4()
{
    // Complete
    class_<Vec4>("Vec4")
         .def(init<float, float, float, float>())
         .def(init<osg::Vec3&, float>())
         .def(self == self)
         .def(self != self)
         .def(self < self)
         .def("set", &Vec4::set)

         .def("__getitem__", (float &(Vec4::*)(unsigned int)) &Vec4::operator[], return_value_policy<copy_non_const_reference>())
         .def("__setitem__", &__setitem__)

         // .def("x", (float (Vec4::*)() const) &Vec4::x)
         // .def("y", (float (Vec4::*)() const) &Vec4::y)
         // .def("z", (float (Vec4::*)() const) &Vec4::z)
         // .def("w", (float (Vec4::*)() const) &Vec4::w)

         .add_property("_x", get_x, set_x)
         .add_property("_y", get_y, set_y)
         .add_property("_z", get_z, set_z)
         .add_property("_w", get_w, set_w)

         .def("asABGR", &Vec4::asABGR)
         .def("asRGBA", &Vec4::asRGBA)

         .def("valid", &Vec4::valid)
         .def("isNaN", &Vec4::isNaN)

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

         .def("length", &Vec4::length)
         .def("length2", &Vec4::length2)
         .def("normalize", &Vec4::normalize)

         .def("__str__", &osgVec4_str)
         .def("__repr__", &osgVec4_repr)
         ;
}
