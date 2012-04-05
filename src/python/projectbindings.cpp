#include <python/dtpython.h>
#include <dtCore/project.h>
#include <dtCore/map.h>
#include <dtCore/scene.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(SC_overloads, SetContext, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LMIS_overloads, LoadMapIntoScene, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CM_overloads, CloseMap, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(DM_overloads, DeleteMap, 1, 2)

void initProjectBindings()
{
   Map& ( Project::*LMIS1 )( const std::string&, dtCore::Scene&, bool) = &Project::LoadMapIntoScene;
   void ( Project::*LMIS2 )( Map&, dtCore::Scene&, bool) = &Project::LoadMapIntoScene;

   void ( Project::*DM1 )( Map&, bool ) = &Project::DeleteMap;
   void ( Project::*DM2 )( const std::string&, bool ) = &Project::DeleteMap;

   void ( Project::*SM1 )( Map& ) = &Project::SaveMap;
   void ( Project::*SMA1 )( Map&, const std::string&, const std::string&, Project::ContextSlot ) = &Project::SaveMapAs;

   void ( Project::*SM2 )( const std::string& ) = &Project::SaveMap;
   void ( Project::*SMA2 )( const std::string&, const std::string&, const std::string&, Project::ContextSlot ) = &Project::SaveMapAs;

   bool ( Project::*HB1 )( Map& ) const = &Project::HasBackup;
   bool ( Project::*HB2 )( const std::string& ) const = &Project::HasBackup;

   void ( Project::*CB1 )( Map& ) = &Project::ClearBackup;
   void ( Project::*CB2 )( const std::string& ) = &Project::ClearBackup;

   class_< Project, dtCore::RefPtr<Project>, boost::noncopyable >( "Project", no_init )
      .def( "GetInstance", &Project::GetInstance, return_value_policy<reference_existing_object>() )
      .staticmethod("GetInstance")
      .def( "IsContextValid", &Project::IsContextValid )
      .def( "SetContext", &Project::SetContext, SC_overloads() )
      .def( "GetContext", &Project::GetContext, return_value_policy<copy_const_reference>() )
      .def( "Refresh", &Project::Refresh )
      .def( "GetMap", &Project::GetMap, return_internal_reference<>() )
      .def( "OpenMapBackup", &Project::OpenMapBackup, return_internal_reference<>() )
      .def( "LoadMapIntoScene", LMIS1, LMIS_overloads()[ return_internal_reference<>() ] )
      .def( "LoadMapIntoScene", LMIS2, LMIS_overloads() )
      .def( "CreateMap", &Project::CreateMap, return_internal_reference<>() )
      .def( "CloseMap", &Project::CloseMap, CM_overloads() )
      .def( "DeleteMap", DM1, DM_overloads() )
      .def( "DeleteMap", DM2, DM_overloads() )
      .def( "SaveMap", SM1 )
      .def( "SaveMapAs", SMA1 )
      .def( "SaveMap", SM2 )
      .def( "SaveMapAs", SMA2 )
      .def( "SaveMapBackup", &Project::SaveMapBackup )
      .def( "HasBackup", HB1 )
      .def( "HasBackup", HB2 )
      .def( "ClearBackup", CB1 )
      .def( "ClearBackup", CB2 )
      .def( "GetResourcePath", &Project::GetResourcePath )
      .def( "IsArchive", &Project::IsArchive )
      .def( "IsReadOnly", &Project::IsReadOnly )
      ;
}
