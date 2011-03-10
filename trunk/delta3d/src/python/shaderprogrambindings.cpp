#include <python/dtpython.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shaderparameter.h>

using namespace boost::python;
using namespace dtCore;

void initShaderProgramBindings()
{

   const ShaderParameter* (ShaderProgram::*FindParameter1)(const std::string&) const = &ShaderProgram::FindParameter;
   ShaderParameter* (ShaderProgram::*FindParameter2)(const std::string&) = &ShaderProgram::FindParameter;

   void (ShaderProgram::*AddBindAttributeLocation)(const std::string&, unsigned int) = &ShaderProgram::AddBindAttributeLocation;

   class_<ShaderProgram, RefPtr<ShaderProgram>, boost::noncopyable>("ShaderProgram", no_init)
      .def("GetName", &ShaderProgram::GetName, return_internal_reference<>())
      .def("AddBindAttributeLocation", &ShaderProgram::AddBindAttributeLocation)
      .def("FindParameter", FindParameter1, return_internal_reference<>())
      .def("FindParameter", FindParameter2, return_internal_reference<>())
      .def("Update",&ShaderProgram::Update)
	   ;

}

