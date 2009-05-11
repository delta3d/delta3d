#include <python/dtpython.h>
#include <dtCore/shadermanager.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LSD_overloads, LoadShaderDefinitions, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(FSP1_overloads, FindShaderPrototype1, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(FSP2_overloads, FindShaderPrototype2, 1, 2)

class ShaderManagerWrap
{
   static ShaderManagerWrap* mInstance;
public:
   static ShaderManagerWrap& GetInstance()
   {
      if (mInstance == NULL)
      {
         mInstance = new ShaderManagerWrap();
      }
      return *mInstance;
   }
   void LoadShaderDefinitions(const std::string& fileName, bool merge = true)
   {  
      ShaderManager::GetInstance().LoadShaderDefinitions(fileName, merge);
   }
   const ShaderProgram* FindShaderPrototype(const std::string& name, const std::string& groupName="") const
   {
      return ShaderManager::GetInstance().FindShaderPrototype(name, groupName);
   }
   ShaderProgram* FindShaderPrototype(const std::string& name, const std::string& groupName="")
   {
      return ShaderManager::GetInstance().FindShaderPrototype(name, groupName);
   }
   ShaderProgram* AssignShaderFromPrototype(dtCore::ShaderProgram& shader, osg::Node& node)
   {
      return ShaderManager::GetInstance().AssignShaderFromPrototype(shader, node);
   }
   void UnassignShaderFromNode(osg::Node& node)
   {
      ShaderManager::GetInstance().UnassignShaderFromNode(node);
   }
};

ShaderManagerWrap* ShaderManagerWrap::mInstance(NULL);

void initShaderManagerBindings()
{
   const ShaderProgram* (ShaderManagerWrap::*FindShaderPrototype1)(const std::string&,const std::string&) const = &ShaderManagerWrap::FindShaderPrototype;
   ShaderProgram* (ShaderManagerWrap::*FindShaderPrototype2)(const std::string&,const std::string&) = &ShaderManagerWrap::FindShaderPrototype;


   class_<ShaderException, bases<dtUtil::Enumeration>, boost::noncopyable>("ShaderException", no_init)
      ;
   class_<ShaderManagerWrap, boost::noncopyable>("ShaderManager", no_init)
      .def("GetInstance", &ShaderManagerWrap::GetInstance, return_value_policy<reference_existing_object>())
      .staticmethod("GetInstance")
      .def("LoadShaderDefinitions", &ShaderManagerWrap::LoadShaderDefinitions, LSD_overloads())
      .def("FindShaderPrototype", FindShaderPrototype1, return_internal_reference<>(), FSP1_overloads())
      .def("FindShaderPrototype", FindShaderPrototype2, return_internal_reference<>(), FSP2_overloads())
      .def("AssignShaderFromPrototype", &ShaderManagerWrap::AssignShaderFromPrototype, return_internal_reference<>())
      .def("UnassignShaderFromNode", &ShaderManagerWrap::UnassignShaderFromNode)
      ;
}
