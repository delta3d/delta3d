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
// Updated for osg-0.9.6 to use doubles instead of floats, 
// Added a couple missing methods, and is matrix.set working?
// Hardcoded for Matrixd, all args are doubles
// updated for OSG CVS july 6th 2004 by brett hartshorn
//
// Modified to work with Delta3D's bindings - Chris Osborn
// 2005-09-16


#include <osg/Matrix>
#include <osg/Quat>

#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility.hpp>

#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
//#include <boost/python/call_method.hpp>
#include <boost/python/call.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/return_opaque_pointer.hpp>
#include <boost/python/return_internal_reference.hpp>
#include <boost/python/copy_const_reference.hpp>
#include <boost/python/reference_existing_object.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/make_function.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/errors.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/operators.hpp>
#include <boost/python/iterator.hpp>
#include <boost/python/detail/api_placeholder.hpp>
#include <python/heldptr.h>
#include <python/osgmath.h>

using namespace boost::python;

namespace {
std::string RefMatrix_str(osg::RefMatrix * self)
{
    std::ostringstream ost;
    ost << "(";
    for (unsigned int i=0 ; i<4; i++) {
        ost << "  (" << (*self)(i,0) << "," << (*self)(i,1) << "," << (*self)(i,2) << "," << (*self)(i,3) << ")";
        if (i != 3) ost << ",";
    }
    ost << ")";
    return ost.str();
}

std::string RefMatrix_repr(osg::RefMatrix * self)
{
    std::ostringstream ost;
    ost << self->className() << "((";
    for (unsigned int i =0 ; i<4 ; i++) {
        ost << "(";
        for (unsigned int  j=0; j < 4 ; j++) {
            ost << (*self)(i,j);
            if (j != 3) ost << ",";
        }
        ost << ")";
        if (i != 3) ost << ",";
    }
    ost << "))";
    return ost.str();
}

std::string Matrix_str(osg::Matrix * self)
{
    std::ostringstream ost;
    ost << "(";
    for (unsigned int i=0 ; i<4; i++) {
        ost << "  (" << (*self)(i,0) << "," << (*self)(i,1) << "," << (*self)(i,2) << "," << (*self)(i,3) << ")";
        if (i != 3) ost << ",";
    }
    ost << ")";
    return ost.str();
}

void osgMatrix_setitem(osg::Matrix * self, int idx, float val)
{
    if (idx < 0 || idx >= 16) {
        PyErr_SetString(PyExc_IndexError, "range[0,15]");
        throw_error_already_set();
        return;
    }
    (*self)(idx /4, idx % 4) = val;
}

float osgMatrix_getitem(osg::Matrix * self, int idx)
{
    if (idx < 0 || idx >= 16) {
        PyErr_SetString(PyExc_IndexError, "range[0,15]");
        throw_error_already_set();
        return 0.0;
    }
    return (*self)(idx /4, idx % 4);
}


void osgMatrix_set(osg::Matrix * self, tuple matarray)
{
    int elements = len(matarray);

    if (elements != 16) {
        PyErr_SetString(PyExc_ValueError, "bad tuple length, should be 16");
        throw_error_already_set();
        return;
    }

    for (int i=0; i<elements ; i++) {
        // Changed to doubles, and where is the set method being called, was this just incomplete?
        //float val = extract<float>(matarray[i]);
        double val = extract<double>(matarray[i]);
        (*self)(i / 4, i % 4) = val;
    }
}

#if 0
// deprecated by brett

float osgMatrix_get(osg::Matrix * self, int row, int col)
{
    if (row < 0 || row >= 4 || col < 0 || col >= 4) {
        PyErr_SetString(PyExc_IndexError, "row, col should be between 0..3");
        throw_error_already_set();
        return 0;
    }
    return (*self)(row, col);
}

osg::Matrix createFromTuple(tuple matarray)
{
    osg::Matrix mat;

    osgMatrix_set(&mat, matarray);
    return mat;
}

#endif

float * asFloats(osg::Matrix * self)
{
    return (float *)self;
}

list asList(osg::Matrix * self)
{
    list l;

    for (int i = 0; i<4; i++) {
        for (int j=0 ; j<4; j++) {
            l.append((*self)(i,j));
        }
    }
    return l;
}

} // namespace

void initOSGMatrix()
{

    class_<osg::Matrix> matrix("Matrix");

	matrix
	.def(init<>())
	.def(init<const osg::Matrixd&>())
	.def(init<const osg::Matrixf&>())
	//.def(init<double, double, double, double, double, double, double, double, double, double, double, double, double, double, double>())
	
	.def("compare", &osg::Matrix::compare)
	.def("valid", &osg::Matrix::valid)
	.def("isNaN", &osg::Matrix::isNaN)
	.def("set", (void (osg::Matrix::*)(const osg::Matrix&)) &osg::Matrix::set)
	.def("set", &osgMatrix_set)

	//.def("set", (void (osg::Matrix::*)(double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double))
	//                       &osg::Matrix::set)

	.def("get", &osg::Matrix::get)
	.def("__setitem__", &osgMatrix_setitem)
	.def("__getitem__", &osgMatrix_getitem)

	.def("makeIdentity", &osg::Matrix::makeIdentity)
	.def("makeScale", (void (osg::Matrix::*)(const osg::Vec3&))&osg::Matrix::makeScale)
	.def("makeScale", (void (osg::Matrix::*)(double, double, double))&osg::Matrix::makeScale)

	.def("makeTranslate", (void (osg::Matrix::*)(const osg::Vec3&))&osg::Matrix::makeTranslate)
	.def("makeTranslate", (void (osg::Matrix::*)(double, double, double))&osg::Matrix::makeTranslate)

	.def("makeRotate",
		(void (osg::Matrix::*)(const osg::Vec3&, const osg::Vec3&))
		&osg::Matrix::makeRotate)
	.def("makeRotate",
		(void (osg::Matrix::*)(double, const osg::Vec3&))
		&osg::Matrix::makeRotate)
	.def("makeRotate",
		(void (osg::Matrix::*)(double, double, double, double))
		&osg::Matrix::makeRotate)
	.def("makeRotate",
		(void (osg::Matrix::*)(const osg::Quat&))
		&osg::Matrix::makeRotate)
	.def("makeRotate",
		(void (osg::Matrix::*)(double, const osg::Vec3&, double, const osg::Vec3&, double, const osg::Vec3&))
		&osg::Matrix::makeRotate)

	.def("makeOrtho", &osg::Matrix::makeOrtho)
	.def("getOrtho", &osg::Matrix::getOrtho)
	.def("makeOrtho2D", &osg::Matrix::makeOrtho2D)

	.def("makeFrustum", &osg::Matrix::makeFrustum)
	.def("getFrustum", &osg::Matrix::getFrustum)
	.def("makePerspective", &osg::Matrix::makePerspective)
	.def("getPerspective", &osg::Matrix::getPerspective)
	
	// hack? forcing to float Vec3 instead of Vec3d
	.def("makeLookAt", (void (osg::Matrix::*)(const osg::Vec3&, const osg::Vec3&, const osg::Vec3&))
		&osg::Matrix::makeLookAt)
	
	.def("getLookAt", (void (osg::Matrix::*) (osg::Vec3d&,osg::Vec3d&,osg::Vec3d&, double) const)
		&osg::Matrix::getLookAt)

	.def("invert", &osg::Matrix::invert)
	.def("invert_4x4_orig", &osg::Matrix::invert_4x4_orig)
	.def("invert_4x4_new", &osg::Matrix::invert_4x4_new)

	.def("identity",  &osg::Matrix::identity)

   .def("scale", (osg::Matrix (*)(const osg::Vec3&)) &osg::Matrix::scale )
	.def("scale", (osg::Matrix (*)(double, double, double)) &osg::Matrix::scale)
	.def("translate", (osg::Matrix (*)(const osg::Vec3&)) &osg::Matrix::translate)
	.def("translate",	(osg::Matrix (*)(double, double, double)) &osg::Matrix::translate)
	.def("rotate",	(osg::Matrix (*)(double, double, double, double)) &osg::Matrix::rotate)
	.def("rotate",	(osg::Matrix (*)(double, const osg::Vec3&)) &osg::Matrix::rotate)
	.def("rotate",	(osg::Matrix (*)(double, const osg::Vec3&, double, const osg::Vec3&, double, const osg::Vec3&))	&osg::Matrix::rotate)
	.def("rotate",	(osg::Matrix (*)(const osg::Quat&)) &osg::Matrix::rotate)

	.def("inverse",  &osg::Matrix::inverse)
	.def("ortho",  &osg::Matrix::ortho)
	.def("ortho2D",  &osg::Matrix::ortho2D)
	.def("frustum",  &osg::Matrix::frustum)
	.def("perspective",  &osg::Matrix::perspective)
	.def("lookAt", (osg::Matrix (*)(const osg::Vec3f&,const osg::Vec3f&,const osg::Vec3f&))	&osg::Matrix::lookAt)

	.def("preMult", (osg::Vec3 (osg::Matrix::*)(const osg::Vec3&) const) &osg::Matrix::preMult)
	.def("postMult", (osg::Vec3 (osg::Matrix::*)(const osg::Vec3&) const) &osg::Matrix::postMult)
	.def("preMult", (osg::Vec4 (osg::Matrix::*)(const osg::Vec4&) const) &osg::Matrix::preMult)
	.def("postMult", (osg::Vec4 (osg::Matrix::*)(const osg::Vec4&) const) &osg::Matrix::postMult)
	.def(self * self)
	.def(self *= self)

	.def("setTrans", (void (osg::Matrix::*)(double, double, double)) &osg::Matrix::setTrans)
	.def("setTrans", (void (osg::Matrix::*)(const osg::Vec3&)) &osg::Matrix::setTrans)
	.def("getTrans", &osg::Matrix::getTrans)
	.def("getScale", &osg::Matrix::getScale)

	//.def("transform3x3",
	//                       (static osg::Vec3 (osg::Matrix::transform3x3*)(const osg::Vec3&, const osg::Matrix&)) &osg::Matrix::transform3x3)
	//.def("transform3x3",
	//                      (osg::Vec3 (osg::Matrix::*)(const osg::Matrix&, const osg::Vec3&)) &osg::Matrix::transform3x3)

	// the 'work horse' methods
	.def("mult", &osg::Matrix::mult)
	.def("preMult", (void (osg::Matrix::*)(const osg::Matrix&)) &osg::Matrix::preMult)
	.def("postMult", (void (osg::Matrix::*)(const osg::Matrix&)) &osg::Matrix::postMult)

	.def("asFloats", &asFloats, return_value_policy<return_opaque_pointer>())
	.def("asList", &asList)

	.staticmethod("identity")
	.staticmethod("scale")
	.staticmethod("translate")
	.staticmethod("rotate")
	.staticmethod("inverse")
	.staticmethod("ortho")
	.staticmethod("ortho2D")
	.staticmethod("frustum")
	.staticmethod("perspective")
	.staticmethod("lookAt")
	.def("__str__", &Matrix_str)

         ;

   /*
    class_<osg::RefMatrix, held_ptr<osg::RefMatrix>, bases<osg::Matrix, osg::Object>, boost::noncopyable >("RefMatrix", no_init)
        .def(init<>())
        .def("__str__", &RefMatrix_str)
        .def("__repr__", &RefMatrix_repr)
        ;
   */
        }
