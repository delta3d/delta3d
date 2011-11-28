#include <osg/Vec4>

#include <python/dtpython.h>
#include <dtCore/shaderparamvec4.h>

using namespace boost::python;
using namespace dtCore;

void initShaderParamVec4Bindings()
{
   const osg::Vec4& (ShaderParamVec4::*GetValue)() const = &ShaderParamVec4::GetValue;

   class_<ShaderParamVec4, bases<ShaderParameter>, RefPtr<ShaderParamVec4>, boost::noncopyable>("ShaderParamVec4", no_init)
      .def("GetValue", GetValue, return_internal_reference<>())
      .def("SetValue", &ShaderParamVec4::SetValue)
      ;
}

