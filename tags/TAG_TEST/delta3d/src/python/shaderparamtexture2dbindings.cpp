#include <python/dtpython.h>
#include <dtCore/shaderparamtexture2d.h>

using namespace boost::python;
using namespace dtCore;

void initShaderParamTexture2DBindings()
{
   class_<ShaderParamTexture2D, bases<ShaderParamTexture>, RefPtr<ShaderParamTexture2D>, boost::noncopyable>("ShaderParamTexture2D", no_init)
      .def("LoadImage", &ShaderParamTexture2D::LoadImage)
      .def("ApplyTexture2DValues", &ShaderParamTexture2D::ApplyTexture2DValues)
      ;
}

