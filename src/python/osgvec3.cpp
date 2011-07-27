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
//
// updated for osg cvs version march 22 2004, brett hartshorn
// added crossProduct brett oct 3 2004
//
// Modified to work with Delta3D's bindings - Chris Osborn
// 2005-09-16

#include <osg/Vec3>

#include <string>
#include <iostream>
#include <sstream>

#include <boost/python/class.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/scope.hpp>
#include <boost/python/copy_non_const_reference.hpp>
#include <python/heldptr.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
using namespace osg;
using namespace boost::python;
/// @endcond

namespace {

   std::string vec3_repr(Vec3 * self)
   {
      std::ostringstream ost;
      ost << "Vec3(" << self->_v[0] << "," << self->_v[1] << "," << self->_v[2] << ")";
      return ost.str();
   }

   std::string vec3_str(Vec3 * self)
   {
      std::ostringstream ost;
      ost << self->_v[0] << " " << self->_v[1] << " " << self->_v[2];
      return ost.str();
   }

   
   inline void __setitem__(Vec3 * self, int idx, float val) { (*self)[idx] = val; }
   
   inline float get_x(Vec3 * self) { return self->_v[0]; }
   inline void  set_x(Vec3 * self, float x) { self->_v[0] = x; }
   
   inline float get_y(Vec3 * self) { return self->_v[1]; }
   inline void  set_y(Vec3 * self, float y) { self->_v[1] = y; }
   
   inline float get_z(Vec3 * self) { return self->_v[2]; }
   inline void  set_z(Vec3 * self, float z) { self->_v[2] = z; }
   
   const Vec3 crossProduct(Vec3 * self, Vec3 * other) { return *self ^ *other; }

} // namespace

void initOSGVec3()
{
    class_<osg::Vec3> vec3("Vec3","General purpose float triple for use as vertices, vectors and normals.\n");

     vec3
        .def(init<float, float, float>())

        .def(self == self)
        .def(self != self)
        .def(self < self)

        .def("set", (void (Vec3::*)(float, float, float)) &Vec3::set)
        .def("set", (void (Vec3::*)(const Vec3&)) &Vec3::set)

        .def("__getitem__", (float &(Vec3::*)(int)) &Vec3::operator[], return_value_policy<copy_non_const_reference>())
        .def("__setitem__", &__setitem__)
	
        // note in OSG c++ this would be _x, _y, _z
        .add_property("x", get_x, set_x)
        .add_property("y", get_y, set_y)
        .add_property("z", get_z, set_z)
	
        .def("valid", &Vec3::valid)
        .def("isNaN", &Vec3::isNaN)

        .def(self * self)
        // segmentation fault?
        //.def("crossProduct", (const Vec3(Vec3::*)(const Vec3)) &Vec3::operator^)
        .def("crossProduct", &crossProduct)
        .def(self * float())
        .def(self *= float())
	
        .def(self / float())
        .def(self /= float())
	
        .def(self + self)
        .def(self += self)
	
        .def(self - self)
        .def(self -= self)
        .def(-self)
        
        .def("length", &Vec3::length, "Length of the vector = sqrt( vec . vec )\n")
        .def("length2", &Vec3::length2, "Length squared of the vector = vec . vec\n")
        .def("normalize", &Vec3::normalize, "normalize the vector so that it has length unity\n")

        .def("__str__", &vec3_str)
        .def("__repr__", &vec3_repr)
        ;

     scope().attr("X_AXIS") = osg::X_AXIS;
     scope().attr("Y_AXIS") = osg::Y_AXIS;
     scope().attr("Z_AXIS") = osg::Z_AXIS;
}

