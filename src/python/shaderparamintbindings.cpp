#include <python/dtpython.h>
#include <dtCore/shaderparamint.h>

using namespace boost::python;
using namespace dtCore;

void initShaderParamIntBindings()
{
   class_<ShaderParamInt, bases<ShaderParameter>, RefPtr<ShaderParamInt>, boost::noncopyable>("ShaderParamInt", no_init)
      .def("GetValue", &ShaderParamInt::GetValue)
      .def("SetValue", &ShaderParamInt::SetValue)
      ;
}

