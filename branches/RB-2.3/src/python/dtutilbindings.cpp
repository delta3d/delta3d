// dtutilbindings.cpp: Python bindings for dtUtil library.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

// for date/time
#include <python/dtpython.h>
#include <dtUtil/datetime.h>

void initEnumerationBindings();
void init_NoiseBindings();
void init_MatrixUtilBindings();
void init_PolarDecompBindings();
void init_LogBindings();

void init_DateTimeBindings()
{
   using namespace boost::python;

   // todo -- find a better home for this
   class_<dtUtil::DateTime>("DateTime")
      .def("GetYear",   &dtUtil::DateTime::GetYear)
      .def("SetYear",   &dtUtil::DateTime::SetYear)
      .def("GetMonth",  &dtUtil::DateTime::GetMonth)
      .def("SetMonth",  &dtUtil::DateTime::SetMonth)
      .def("GetDay",    &dtUtil::DateTime::GetDay)
      .def("SetDay",    &dtUtil::DateTime::SetDay)
      .def("GetHour",   &dtUtil::DateTime::GetHour)
      .def("SetHour",   &dtUtil::DateTime::SetHour)
      .def("GetMinute", &dtUtil::DateTime::GetMinute)
      .def("SetMinute", &dtUtil::DateTime::SetMinute)
      .def("GetSecond", &dtUtil::DateTime::GetSecond)
      .def("SetSecond", &dtUtil::DateTime::SetSecond)
      ;
}

BOOST_PYTHON_MODULE(PyDtUtil)
{
   initEnumerationBindings();
   init_NoiseBindings();
   init_MatrixUtilBindings();
   init_PolarDecompBindings();
   init_LogBindings();
   init_DateTimeBindings();
}
