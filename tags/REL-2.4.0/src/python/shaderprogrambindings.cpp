#include <python/dtpython.h>
#include <dtCore/shaderprogram.h>

using namespace boost::python;
using namespace dtCore;

void initShaderProgramBindings()
{
   void (ShaderProgram::*AddBindAttributeLocation)(const std::string&, unsigned int) = &ShaderProgram::AddBindAttributeLocation;

   class_<ShaderProgram, RefPtr<ShaderProgram>, boost::noncopyable>("ShaderProgram", no_init)
      .def("GetName", &ShaderProgram::GetName, return_internal_reference<>())
      .def("AddBindAttributeLocation", &ShaderProgram::AddBindAttributeLocation)
      ;
}

