#include <python/dtpython.h>
#include <dtCore/shaderprogram.h>

using namespace boost::python;
using namespace dtCore;

void initShaderProgramBindings()
{
   class_<ShaderProgram, RefPtr<ShaderProgram>, boost::noncopyable>("ShaderProgram", no_init)
      .def("GetName", &ShaderProgram::GetName, return_internal_reference<>())
      ;
}