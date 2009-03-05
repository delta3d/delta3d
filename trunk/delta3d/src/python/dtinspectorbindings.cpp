// dtinspectorbindings.cpp: Python bindings for dtInspector library.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>

#include <dtInspectorQt/inspectorqt.h>

using namespace boost::python;
using namespace dtInspectorQt;


BOOST_PYTHON_MODULE(PyDtInspectorQt)
{
   class_<InspectorQt, bases<dtCore::Base>, dtCore::RefPtr<InspectorQt>, boost::noncopyable>("InspectorQt",init<int&,char**>())
      .def(init<int&, char**>());
}
