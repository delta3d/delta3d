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

#include <osg/Version>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
using namespace osg;
using namespace boost::python;
/// @endcond

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

std::string Matrix_str(Matrix * self)
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

void osgMatrix_setitem(Matrix * self, int idx, float val)
{
    if (idx < 0 || idx >= 16) {
        PyErr_SetString(PyExc_IndexError, "range[0,15]");
        throw_error_already_set();
        return;
    }
    (*self)(idx /4, idx % 4) = val;
}

float osgMatrix_getitem(Matrix * self, int idx)
{
    if (idx < 0 || idx >= 16) {
        PyErr_SetString(PyExc_IndexError, "range[0,15]");
        throw_error_already_set();
        return 0.0;
    }
    return (*self)(idx /4, idx % 4);
}


void osgMatrix_set(Matrix * self, tuple matarray)
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

float osgMatrix_get(Matrix * self, int row, int col)
{
    if (row < 0 || row >= 4 || col < 0 || col >= 4) {
        PyErr_SetString(PyExc_IndexError, "row, col should be between 0..3");
        throw_error_already_set();
        return 0;
    }
    return (*self)(row, col);
}

Matrix createFromTuple(tuple matarray)
{
    Matrix mat;

    osgMatrix_set(&mat, matarray);
    return mat;
}

#endif

float * asFloats(Matrix * self)
{
    return (float *)self;
}

list asList(Matrix * self)
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

    class_<Matrix> matrix("Matrix");

	matrix
	.def(init<>())
	.def(init<const Matrixd&>())
	.def(init<const Matrixf&>())
	//.def(init<double, double, double, double, double, double, double, double, double, double, double, double, double, double, double>())

	.def("compare", &Matrix::compare)
	.def("valid", &Matrix::valid)
	.def("isNaN", &Matrix::isNaN)
	.def("set", (void (Matrix::*)(const Matrix&)) &Matrix::set)
	.def("set", &osgMatrix_set)

	//.def("set", (void (Matrix::*)(double, double, double, double, double, double, double, double, double, double, double, double, double, double, double, double))
	//                       &Matrix::set)

	.def("get", &Matrix::get)
	.def("__setitem__", &osgMatrix_setitem)
	.def("__getitem__", &osgMatrix_getitem)

	.def("makeIdentity", &Matrix::makeIdentity)
	.def("makeScale", (void (Matrix::*)(const osg::Vec3&))&Matrix::makeScale)
	.def("makeScale", (void (Matrix::*)(double, double, double))&Matrix::makeScale)

	.def("makeTranslate", (void (Matrix::*)(const osg::Vec3&))&Matrix::makeTranslate)
	.def("makeTranslate", (void (Matrix::*)(double, double, double))&Matrix::makeTranslate)

	.def("makeRotate",
		(void (Matrix::*)(const osg::Vec3&, const osg::Vec3&))
		&Matrix::makeRotate)
	.def("makeRotate",
		(void (Matrix::*)(double, const osg::Vec3&))
		&Matrix::makeRotate)
	.def("makeRotate",
		(void (Matrix::*)(double, double, double, double))
		&Matrix::makeRotate)
	.def("makeRotate",
		(void (Matrix::*)(const osg::Quat&))
		&Matrix::makeRotate)
	.def("makeRotate",
		(void (Matrix::*)(double, const osg::Vec3&, double, const osg::Vec3&, double, const osg::Vec3&))
		&Matrix::makeRotate)

	.def("makeOrtho", &Matrix::makeOrtho)
	.def("getOrtho",
      (bool (Matrix::*)(double&, double&, double&, double&, double&, double&) const)
      &Matrix::getOrtho)
   .def("getOrtho",
      (bool (Matrix::*)(float&, float&, float&, float&, float&, float&) const)
      &Matrix::getOrtho)
	.def("makeOrtho2D", &Matrix::makeOrtho2D)

	.def("makeFrustum", &Matrix::makeFrustum)
   .def("getFrustum",
      (bool (Matrix::*)(double&, double&, double&, double&, double&, double&) const)
      &Matrix::getFrustum)
   .def("getFrustum",
      (bool (Matrix::*)(float&, float&, float&, float&, float&, float&) const)
      &Matrix::getFrustum)
	.def("makePerspective", &Matrix::makePerspective)
   .def("getPerspective",
      (bool (Matrix::*)(double&, double&, double&, double&) const)
      &Matrix::getPerspective)
   .def("getPerspective",
      (bool (Matrix::*)(float&, float&, float&, float&) const)
      &Matrix::getPerspective)

	// hack? forcing to float Vec3 instead of Vec3d
	.def("makeLookAt", (void (Matrix::*)(const osg::Vec3&, const osg::Vec3&, const osg::Vec3&))
		&Matrix::makeLookAt)

	.def("getLookAt", (void (Matrix::*) (osg::Vec3d&,osg::Vec3d&,osg::Vec3d&, double) const)
		&Matrix::getLookAt)

	.def("invert", &Matrix::invert)


	.def("invert_4x3", &Matrix::invert_4x3)
	.def("invert_4x4", &Matrix::invert_4x4)

	.def("identity",  &Matrix::identity)

	.def("scale", (Matrix (*)(const osg::Vec3&)) &Matrix::scale )
	.def("scale", (Matrix (*)(double, double, double)) &Matrix::scale)
	.def("translate", (Matrix (*)(const osg::Vec3&)) &Matrix::translate)
	.def("translate",	(Matrix (*)(double, double, double)) &Matrix::translate)
	.def("rotate",	(Matrix (*)(double, double, double, double)) &Matrix::rotate)
	.def("rotate",	(Matrix (*)(double, const osg::Vec3&)) &Matrix::rotate)
	.def("rotate",	(Matrix (*)(double, const osg::Vec3&, double, const osg::Vec3&, double, const osg::Vec3&))	&Matrix::rotate)
	.def("rotate",	(Matrix (*)(const osg::Quat&)) &Matrix::rotate)

	.def("inverse",  &Matrix::inverse)
	.def("ortho",  &Matrix::ortho)
	.def("ortho2D",  &Matrix::ortho2D)
	.def("frustum",  &Matrix::frustum)
	.def("perspective",  &Matrix::perspective)
	.def("lookAt", (Matrix (*)(const osg::Vec3f&,const osg::Vec3f&,const osg::Vec3f&))	&Matrix::lookAt)

	.def("preMult", (osg::Vec3 (Matrix::*)(const osg::Vec3&) const) &Matrix::preMult)
	.def("postMult", (osg::Vec3 (Matrix::*)(const osg::Vec3&) const) &Matrix::postMult)
	.def("preMult", (osg::Vec4 (Matrix::*)(const osg::Vec4&) const) &Matrix::preMult)
	.def("postMult", (osg::Vec4 (Matrix::*)(const osg::Vec4&) const) &Matrix::postMult)
	.def(self * self)
	.def(self *= self)

	.def("setTrans", (void (Matrix::*)(double, double, double)) &Matrix::setTrans)
	.def("setTrans", (void (Matrix::*)(const osg::Vec3&)) &Matrix::setTrans)
	.def("getTrans", &Matrix::getTrans)
	.def("getScale", &Matrix::getScale)

	//.def("transform3x3",
	//                       (static osg::Vec3 (Matrix::transform3x3*)(const osg::Vec3&, const Matrix&)) &Matrix::transform3x3)
	//.def("transform3x3",
	//                      (osg::Vec3 (Matrix::*)(const Matrix&, const osg::Vec3&)) &Matrix::transform3x3)

	// the 'work horse' methods
	.def("mult", &Matrix::mult)
	.def("preMult", (void (Matrix::*)(const Matrix&)) &Matrix::preMult)
	.def("postMult", (void (Matrix::*)(const Matrix&)) &Matrix::postMult)

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
