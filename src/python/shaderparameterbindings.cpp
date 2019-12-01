#include <python/dtpython.h>
#include <dtCore/shaderparameter.h>

using namespace boost::python;
using namespace dtCore;

void initShaderParameterBindings()
{
   class_<ShaderParameter, RefPtr<ShaderParameter>, boost::noncopyable>("ShaderParameter", no_init)
      .def("GetType", &ShaderParameter::GetType, return_internal_reference<>())
	   .def("SetDirty", &ShaderParameter::SetDirty)
	   ;
}

