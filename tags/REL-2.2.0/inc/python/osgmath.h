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

#ifndef PYOSG_MATH
#define PYOSG_MATH 1

#include <boost/python/tuple.hpp>
#include <boost/python/extract.hpp>

#include <osg/Array>

namespace PyOSG {
osg::Array * toArray(const boost::python::tuple& vertices);

inline bool check_tuple(boost::python::object x) { return boost::python::extract<boost::python::tuple>(x).check(); }
inline bool check_Array(boost::python::object x) { return boost::python::extract<osg::Array *>(x).check(); }
inline bool check_Vec3Array(boost::python::object x) { return boost::python::extract<osg::Vec3Array *>(x).check(); }
}

#endif // PYOSG_MATH
