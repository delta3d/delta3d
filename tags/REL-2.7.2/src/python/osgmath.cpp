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
// TODO volume calculation functions
//
// Modified to work with Delta3D's bindings - Chris Osborn
// 2005-09-16

#include <sstream>

#include <osg/Math>
#include <osg/Array>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Quat>

#include <string>
#include <iostream>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/python/def.hpp>
#include <python/osgmath.h>

using namespace boost::python;

namespace {
double inRadians(double angle) { return osg::inRadians(angle); }
double clampTo(double v, double min, double max) { return osg::clampTo(v, min, max); }
double clampAbove(double v, double min) { return osg::clampAbove(v, min); }
double clampBelow(double v, double max) { return osg::clampBelow(v, max); }
double sign(double v) { return osg::sign(v); }
double square(double v) { return osg::square(v); }
double signedSquare(double v) { return osg::signedSquare(v); }
}

void initOSGMath()
{
    def("inDegrees", (double (*)(double)) &osg::inDegrees);
    def("inRadians", (double (*)(double)) &inRadians);
    def("clampTo", &clampTo);
    def("clampAbove", &clampAbove);
    def("clampBelow", &clampBelow);
    def("sign", &sign);
    def("square", &square);
    def("signedSquare", &signedSquare);
    def("DegreesToRadians", (double (*)(double)) &osg::DegreesToRadians);
    def("RadiansToDegrees", (double (*)(double)) &osg::RadiansToDegrees);
    def("isNaN", (bool (*)(float)) &osg::isNaN);

#   define SETATTR_OSG_CONST(CONST) scope().attr(#CONST) = object(osg::CONST);
    SETATTR_OSG_CONST(PI);
    SETATTR_OSG_CONST(PI_2);
    SETATTR_OSG_CONST(PI_4);
}
