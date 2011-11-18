#include <dtUtil/polardecomp.h>
#include <boost/python.hpp>

using namespace dtUtil;
using namespace boost::python;


void init_PolarDecompBindings()
{

   class_<PolarDecomp, PolarDecomp*, boost::noncopyable>("PolarDecomp")
      .def("Decompose", &PolarDecomp::Decompose)
      .staticmethod("Decompose");

}


