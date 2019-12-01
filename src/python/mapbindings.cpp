#include <python/dtpython.h>
#include <dtCore/map.h>

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(FP_overloads, FindProxies, 2, 6)

typedef std::vector< std::string > StringVector;
typedef std::map< std::string, std::string > StringStringMap;

typedef dtCore::RefPtr< ActorProxy > dtCore::BaseActorObjectPtr;
typedef std::vector< dtCore::BaseActorObjectPtr > ActorProxyVector;

void initMapBindings()
{
   class_< StringVector >( "StringVector" )
      .def(vector_indexing_suite< StringVector >())
      ;

   class_< ActorProxyVector >( "ActorProxyVector" )
      .def(vector_indexing_suite< ActorProxyVector, true >())
      ;
    
   class_< StringStringMap >( "StringStringMap" )
      .def( map_indexing_suite< StringStringMap >() )
      ;

   void (Map::*FP1) ( ActorProxyVector&,
                      const std::string&,
                      const std::string&,
                      const std::string&,
                      const std::string&,
                      Map::PlaceableFilter ) = ( void ( Map::* )( ActorProxyVector&,
                                                                  const std::string&,
                                                                  const std::string&,
                                                                  const std::string&,
                                                                  const std::string&,
                                                                  Map::PlaceableFilter ) ) &Map::FindProxies;
      
   void (Map::*GAP1) ( ActorProxyVector& ) = ( void ( Map::* )( ActorProxyVector& ) ) &Map::GetAllProxies;
   
   scope Map_scope = class_< Map, dtCore::RefPtr<Map>, boost::noncopyable >( "Map", no_init )
      .def_readonly( "MAP_FILE_EXTENSION", &Map::MAP_FILE_EXTENSION )
      .def( "GetName", &Map::GetName, return_value_policy<copy_const_reference>() )
      .def( "GetSavedName", &Map::GetSavedName, return_value_policy<copy_const_reference>() )
      .def( "SetName", &Map::SetName )
      .def( "GetFileName", &Map::GetFileName, return_value_policy<copy_const_reference>() )
      .def( "GetDescription", &Map::GetDescription, return_value_policy<copy_const_reference>() )
      .def( "SetDescription", &Map::SetDescription )
      .def( "GetAuthor", &Map::GetAuthor, return_value_policy<copy_const_reference>() )
      .def( "SetAuthor", &Map::SetAuthor )
      .def( "GetComment", &Map::GetComment, return_value_policy<copy_const_reference>() )
      .def( "SetComment", &Map::SetComment )
      .def( "GetCopyright", &Map::GetCopyright, return_value_policy<copy_const_reference>() )
      .def( "SetCopyright", &Map::SetCopyright )
      .def( "GetCreateDateTime", &Map::GetCreateDateTime, return_value_policy<copy_const_reference>() )
      .def( "SetCreateDateTime", &Map::SetCreateDateTime )
      .def( "FindProxies", FP1, FP_overloads() )
      .def( "GetAllProxies", GAP1 )
      .def( "AddProxy", &Map::AddProxy, with_custodian_and_ward<1,2>() )
      //.def( "RemoveProxy", &Map::RemoveProxy ) //doesn't recognize friend!
      .def( "ClearProxies", &Map::ClearProxies )
      .def( "RebuildProxyActorClassSet", &Map::RebuildProxyActorClassSet )
      .def( "IsModified", &Map::IsModified )
      .def( "SetModified", &Map::SetModified )
      .def( "HasLoadingErrors", &Map::HasLoadingErrors )
      .def( "GetMissingLibraries", &Map::GetMissingLibraries, return_internal_reference<>() )
      .def( "GetLibraryVersionMap", &Map::GetLibraryVersionMap, return_internal_reference<>() )
      .def( "GetAllLibraries", &Map::GetAllLibraries, return_internal_reference<>() )
      .def( "HasLibrary", &Map::HasLibrary )
      .def( "GetLibraryVersion", &Map::GetLibraryVersion )
      .def( "InsertLibrary", &Map::InsertLibrary )
      .def( "AddLibrary", &Map::AddLibrary )
      .def( "RemoveLibrary", &Map::RemoveLibrary )
      .def( "WildMatch", &Map::WildMatch )
      .staticmethod( "WildMatch" )
      ;
      
   enum_<Map::PlaceableFilter>("PlaceableFilter")
      .value("Placeable", Map::Placeable)
      .value("NotPlaceable", Map::NotPlaceable)
      .value("Either", Map::Either)
      .export_values();
}
