// terrainBindings.cpp
//    python bindings for dtTerrain::Terrain 
//
// May 16, 2006, Jerry Isdale
// minimal bindings provided, use CreateTerrain factory method
// since we only have dted and soarx
// provide access to soarxdrawable::setBufferSize
//
// June 9, 2006, Chris Osborn
// Added more bindings
// Remove reference to SoarXDrawable's functions
///////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <dtTerrain/terrain.h>

using namespace boost::python;
using namespace dtTerrain;

typedef std::vector< std::string > StringVector;

class TerrainWrap : public Terrain, public wrapper< Terrain >
{
   public:

      TerrainWrap( const std::string& name = "Terrain" ) :
         Terrain( name )
      {
      }

	   void UnloadAllTerrainTiles()
	   {
		   if( override UnloadAllTerrainTiles = this->get_override( "UnloadAllTerrainTiles" ) )
		   {
 			   #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            call<void>( this->get_override( "UnloadAllTerrainTiles" ).ptr() );
            #else
            UnloadAllTerrainTiles();
            #endif
		   }
         else
         {
            Terrain::UnloadAllTerrainTiles();
         }
	   }

	   void DefaultUnloadAllTerrainTiles()
	   {
		   this->Terrain::UnloadAllTerrainTiles();
	   }

      bool IsTerrainTileResident( const GeoCoordinates& coords )
	   {
		   if( override IsTerrainTileResident = this->get_override( "IsTerrainTileResident" ) )
		   {
			   #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
            return call<bool>( this->get_override( "IsTerrainTileResident" ).ptr(), coords );
            #else
            return IsTerrainTileResident( coords );
            #endif
		   }            
		   return Terrain::IsTerrainTileResident( coords );
	   }

	   bool DefaultIsTerrainTileResident( const GeoCoordinates& coords )
	   {
		   return this->Terrain::IsTerrainTileResident( coords );
	   }
};

void initTerrainBindings()
{
   class_< StringVector >( "StringVector" )
      .def( vector_indexing_suite< StringVector, true >() )
      ;

   // I am only bindings the string versions for now since TerrainDecorateLayer has not
   // yet been bound. -osb
   void ( Terrain::*RemoveDecorationLayerByString )( const std::string& ) = &Terrain::RemoveDecorationLayer;
   void ( Terrain::*HideDecorationLayerByString )( const std::string& ) = &Terrain::HideDecorationLayer;
   void ( Terrain::*ShowDecorationLayerByString )( const std::string& ) = &Terrain::ShowDecorationLayer;

   class_< TerrainWrap, bases<dtCore::Physical>, dtCore::RefPtr<TerrainWrap>, boost::noncopyable >("Terrain", init< optional< const std::string& > >() ) 
		.def( "AddResourcePath", &Terrain::AddResourcePath )
      .def( "RemoveResourcePath", &Terrain::RemoveResourcePath )
      .def( "FindResource", &Terrain::FindResource )
      .def( "FindAllResources", &Terrain::FindAllResources )
      .def( "GetHeight", &Terrain::GetHeight )
      .def( "IsClearLineOfSight", &Terrain::IsClearLineOfSight )
      .def( "SetLineOfSightSpacing", &Terrain::SetLineOfSightSpacing )
      .def( "GetLineOfSightSpacing", &Terrain::GetLineOfSightSpacing )
      .def( "UnloadAllTerrainTiles", &Terrain::UnloadAllTerrainTiles, &TerrainWrap::DefaultUnloadAllTerrainTiles )
      .def( "IsTerrainTileResident", &Terrain::IsTerrainTileResident, &TerrainWrap::DefaultIsTerrainTileResident )
      .def( "SetCachePath", &Terrain::SetCachePath )
      .def( "GetCachePath", &Terrain::GetCachePath, return_value_policy<copy_const_reference>() )
      .def( "SetLoadDistance", &Terrain::SetLoadDistance )
      .def( "GetLoadDistance", &Terrain::GetLoadDistance )
      .def( "RemoveDecorationLayer", RemoveDecorationLayerByString )
      .def( "GetNumDecorationLayers", &Terrain::GetNumDecorationLayers )
      .def( "ClearDecorationLayers", &Terrain::ClearDecorationLayers )
      .def( "HideDecorationLayer", HideDecorationLayerByString )
      .def( "ShowDecorationLayer", ShowDecorationLayerByString )
      ;
}
