#include <dtUtil/matrixutil.h>
#include <boost/python.hpp>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>

using namespace dtUtil;
using namespace boost::python;


void init_MatrixUtilBindings()
{

   void (*Print1)(const osg::Vec3&) = &MatrixUtil::Print;
   void (*Print2)(const osg::Vec4&) = &MatrixUtil::Print;
   void (*Print3)(const osg::Matrix&) = &MatrixUtil::Print;

   void (*SetRow1)(osg::Matrix&, const osg::Vec3&, int) = &MatrixUtil::SetRow;
   void (*SetRow2)(osg::Matrix&, const osg::Vec4&, int) = &MatrixUtil::SetRow;

   void (*TransformVec31)(osg::Vec3&, const osg::Matrix&) = &MatrixUtil::TransformVec3;
   void (*TransformVec32)(osg::Vec3&, const osg::Vec3&, const osg::Matrix&) = &MatrixUtil::TransformVec3;



   class_<MatrixUtil, MatrixUtil*, boost::noncopyable>("MatrixUtil")
      .def("Print", Print1)
      .def("Print", Print2)
      .def("Print", Print3)
      .staticmethod("Print")
      .def("ClampUnity", &MatrixUtil::ClampUnity)
      .staticmethod("ClampUnity")
      .def("Transpose", &MatrixUtil::Transpose)
      .staticmethod("Transpose")
      .def("GetRow3", &MatrixUtil::GetRow3)
      .staticmethod("GetRow3")
      .def("GetRow4", &MatrixUtil::GetRow4)
      .staticmethod("GetRow4")
      .def("SetRow", SetRow1)
      .def("SetRow", SetRow2)
      .staticmethod("SetRow")
      .def("HprToMatrix", &MatrixUtil::HprToMatrix)
      .staticmethod("HprToMatrix")
      .def("MatrixToHpr", &MatrixUtil::MatrixToHpr)
      .staticmethod("MatrixToHpr")
      .def("PositionAndHprToMatrix", &MatrixUtil::PositionAndHprToMatrix)
      .staticmethod("PositionAndHprToMatrix")
      .def("TransformVec3", TransformVec31)
      .def("TransformVec3", TransformVec32)
      .staticmethod("TransformVec3");


}


