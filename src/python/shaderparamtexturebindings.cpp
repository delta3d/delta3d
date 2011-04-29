#include <python/dtpython.h>
#include <dtCore/shaderparamtexture.h>

using namespace boost::python;
using namespace dtCore;

void initShaderParamTextureBindings()
{
   class_<ShaderParamTexture, bases<ShaderParameter>, RefPtr<ShaderParamTexture>, boost::noncopyable>("ShaderParamTexture", no_init)
      .def("GetTexture", &ShaderParamTexture::GetTexture, return_internal_reference<>())
      .def("SetTextureUnit", &ShaderParamTexture::SetTextureUnit)
      ;
}

