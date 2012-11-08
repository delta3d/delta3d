#include <python/dtpython.h>
#include <dtCore/shaderparamfloat.h>

using namespace boost::python;
using namespace dtCore;

void initShaderParamFloatBindings()
{
   class_<ShaderParamFloat, bases<ShaderParameter>, RefPtr<ShaderParamFloat>, boost::noncopyable>("ShaderParamFloat", no_init)
      .def("GetValue", &ShaderParamFloat::GetValue)
	   .def("SetValue", &ShaderParamFloat::SetValue)
      ;
}

