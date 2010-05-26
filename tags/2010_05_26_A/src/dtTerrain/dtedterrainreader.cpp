/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Matthew W. Campbell
*/
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <dtUtil/fileutils.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <dtTerrain/terraindatareader.h>
#include <dtTerrain/terraindatarenderer.h>
#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/dtedterrainreader.h>
#include <dtTerrain/imageutils.h>
#include <dtTerrain/terrain.h>

namespace dtTerrain 
{
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(DTEDTerrainReader::DTEDLevelEnum);
   const DTEDTerrainReader::DTEDLevelEnum DTEDTerrainReader::DTEDLevelEnum::ZERO("ZERO",0);
   const DTEDTerrainReader::DTEDLevelEnum DTEDTerrainReader::DTEDLevelEnum::ONE("ONE",1);
   const DTEDTerrainReader::DTEDLevelEnum DTEDTerrainReader::DTEDLevelEnum::TWO("TWO",2);
   const DTEDTerrainReader::DTEDLevelEnum DTEDTerrainReader::DTEDLevelEnum::THREE("THREE",3);
   const DTEDTerrainReader::DTEDLevelEnum DTEDTerrainReader::DTEDLevelEnum::FOUR("FOUR",4);
   const DTEDTerrainReader::DTEDLevelEnum DTEDTerrainReader::DTEDLevelEnum::FIVE("FIVE",5);
   

   //////////////////////////////////////////////////////////////////////////    
   DTEDTerrainReader::DTEDTerrainReader(const DTEDLevelEnum &level,
      const std::string &name) : TerrainDataReader(name)
   {
      SetDTEDLevel(level);
   }
    
   
   //////////////////////////////////////////////////////////////////////////    
   DTEDTerrainReader::~DTEDTerrainReader()
   {
   }
   
   //////////////////////////////////////////////////////////////////////////        
   bool DTEDTerrainReader::OnLoadTerrainTile(PagedTerrainTile &tile)
   {
      std::ostringstream latSS, lonSS;      
      std::string dtedPath;      
      
      //If the tile already has a valid heightfield then do nothing.
      if (tile.GetHeightField() != NULL)
         return true;
      
      //Convert our latitude and longitude into strings matching the directory
      //structure of DTED data.
      int lat = (int)floor(tile.GetGeoCoordinates().GetLatitude());
      int lon = (int)floor(tile.GetGeoCoordinates().GetLongitude());
      latSS << (lat < 0 ? 'S' : 'N') << std::setw(2) << osg::absolute(lat);
      lonSS << (lon < 0 ? 'W' : 'E') << std::setw(3) << std::setfill('0') << osg::absolute(lon);
      
      dtedPath = GetDTEDFilePath(latSS.str(),lonSS.str(),GetDTEDLevel().GetNumeral());
                 
      //If we didn't find any data in the location specified, throw an exception!
      if (dtedPath.empty())
      {
         //LOG_ERROR("Could not find DTED resource to load at (latitude,longitude) = (" +
         //      latSS.str() + "," + lonSS.str() + ")");
         return false;
      }
               
      //Finally, we can actually load the DTED data into our heightfield.  Once
      //the data is loaded into and OSG heightfield, we must convert that into
      //our internal heightfield representation which is more compact and more
      //suitable to write to a 16-bit heightmap image.
      dtCore::RefPtr<osgDB::ReaderWriter> gdalReader =
         osgDB::Registry::instance()->getReaderWriterForExtension("gdal");
      
      if (!gdalReader.valid())
         throw dtTerrain::TerrainReaderPluginNotFoundException(
            "The resource could not be read.  Check for available gdal osg plugin.", __FILE__, __LINE__);
            
      osgDB::ReaderWriter::ReadResult rr;
      rr = gdalReader->readHeightField(dtedPath);
      if (!rr.success())
         throw dtTerrain::TerrainCouldNotReadDataException(rr.message(), __FILE__, __LINE__);
      else
      {
         HeightField *hf = ConvertHeightField(rr.getHeightField());
         float gridSpacing = GeoCoordinates::EQUATORIAL_RADIUS *
            osg::DegreesToRadians(1.0f);
         
         hf->SetXInterval(gridSpacing/(float)(hf->GetNumColumns()-1));
         hf->SetYInterval(gridSpacing/(float)(hf->GetNumRows()-1));
         tile.SetHeightField(hf);      
      }
      
      return true;
   }
   
   ///////////////////////////////////////////////////////////////////////// 
   const std::string DTEDTerrainReader::GenerateTerrainTileCachePath(
      const PagedTerrainTile &tile)
   {
      std::ostringstream ss;
      GeoCoordinates coords = tile.GetGeoCoordinates();
      int d,m,s;
      
      //First build the latitude string.
      coords.GetLatitude(d,m,s);
      if (d < 0)
         ss << osg::absolute(d) << "-" << m << "-" << s << "_S_";
      else
         ss << osg::absolute(d) << "-" << m << "-" << s << "_N_";
      
      //Second append the longitude string.
      coords.GetLongitude(d,m,s);
      if (d < 0)
         ss << osg::absolute(d) << "-" << m << "-" << s << "_W_";
      else
         ss << osg::absolute(d) << "-" << m << "-" << s << "_E_";
         
      //Finally, append the dted level this reader is working with.
      ss << "dted" << GetDTEDLevel().GetNumeral();      
      
      return ss.str();
   }
         
   ///////////////////////////////////////////////////////////////////////// 
   std::string DTEDTerrainReader::GetDTEDFilePath(const std::string &lat, 
      const std::string &lon, int level)
   {   
      //First find the longitude data path.  This must be done in a loop
      //since depending on what DTED level we are loading, there may be
      //multiple longitude folders containing DTED data.
      std::vector<std::string> pathList;
      std::vector<std::string>::iterator lonPath;
      
      GetParentTerrain()->FindAllResources(lon,pathList);      
      for (lonPath=pathList.begin(); lonPath!=pathList.end(); ++lonPath)
      {
         //Next locate the actual DTED data file.
         std::ostringstream ss;
         ss << lat << ".dt" << level;
         std::string latPath = 
            osgDB::findFileInDirectory(ss.str(),*lonPath,osgDB::CASE_INSENSITIVE);
        
         if (!latPath.empty())
            return latPath;
      }
      
      return std::string();
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void DTEDTerrainReader::SetDTEDLevel(const DTEDLevelEnum &level)
   {
      mDTEDLevel = &level;      
   }   
  
}
