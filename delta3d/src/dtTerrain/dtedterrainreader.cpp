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
* @author Matthew W. Campbell
*/
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <dtDAL/fileutils.h>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include "dtTerrain/terraindatareader.h"
#include "dtTerrain/terraindatarenderer.h"
#include "dtTerrain/terraindecorationlayer.h"
#include "dtTerrain/dtedterrainreader.h"
#include "dtTerrain/imageutils.h"

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
   DTEDTerrainReader::DTEDTerrainReader(const DTEDLevelEnum &maxlevel,
      const std::string &name) : TerrainDataReader(name)
   {
      SetMaxDTEDLevel(maxlevel);
   }
    
   
   //////////////////////////////////////////////////////////////////////////    
   DTEDTerrainReader::~DTEDTerrainReader()
   {
   }
    
   //////////////////////////////////////////////////////////////////////////        
   void DTEDTerrainReader::LoadElevationData(const std::string &dataPath, int lat, int lon)
   {
      std::ostringstream latSS, lonSS;      
      std::string dtedPath;      
      
      //Convert our latitude and longitude into strings matching the directory
      //structure of DTED data.
      latSS << (lat < 0 ? 'S' : 'N') << std::setw(2) << osg::absolute(lat);
      lonSS << (lon < 0 ? 'W' : 'E') << std::setw(3) << std::setfill('0') << osg::absolute(lon);
      
      int currLevel = GetMaxDTEDLevel().GetNumeral();
      while (currLevel >= 0)
      {
         dtedPath = GetDTEDFilePath(dataPath,latSS.str(),lonSS.str(),currLevel);
         if (dtedPath != "")
            break;         
            
         currLevel--;
      }
      
      //If we didn't find any data in the location specified, throw an exception!
      if (dtedPath == "")
         EXCEPT(TerrainDataReaderException::DATA_RESOURCE_NOT_FOUND,
            "Could not find DTED resource to load at (latitude,longitude) = (" +
               latSS.str() + "," + lonSS.str() + ")");
            
      //Finally, we can actually load the DTED data into our heightfield.
      dtCore::RefPtr<osgDB::ReaderWriter> gdalReader =
         osgDB::Registry::instance()->getReaderWriterForExtension("gdal");
      
      if (!gdalReader.valid())
         EXCEPT(TerrainDataReaderException::READER_PLUGIN_NOT_FOUND,
            "The resource could not be read.  Check for available gdal osg plugin.");
            
      osgDB::ReaderWriter::ReadResult rr;
      rr = gdalReader->readHeightField(dtedPath);
      if (!rr.success())
         EXCEPT(TerrainDataReaderException::COULD_NOT_READ_DATA,rr.message());
      else
      {
         mHeightField = rr.getHeightField();  
      }
      
      LOG_INFO("Loaded DTED resource: " + dtedPath);
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   std::string DTEDTerrainReader::GetDTEDFilePath(const std::string &basePath, 
      const std::string &lat, const std::string &lon, int level)
   {
      std::string lonPath,latPath; 
      std::ostringstream ss;     
      osgDB::FilePathList filePaths = osgDB::getDataFilePathList();
      osgDB::FilePathList::const_iterator pathItor;
      
      for (pathItor=filePaths.begin(); pathItor!=filePaths.end(); ++pathItor)
      {  
         //First find the longitude data path.
         lonPath = osgDB::findFileInDirectory(lon,*pathItor + "/" + basePath,
            osgDB::CASE_INSENSITIVE);
         if (lonPath == "")
            continue;
         
         //Now, see if we can locate the actual DTED file to load based on the given
         //latitude.
         ss << lat << ".dt" << level;
         latPath = osgDB::findFileInDirectory(ss.str(),lonPath,osgDB::CASE_INSENSITIVE);
         if (latPath == "")
            continue;
         else   
            return latPath;         
      }
      
      return std::string();
   }
      
   ////////////////////////////////////////////////////////////////////////// 
   void DTEDTerrainReader::SetMaxDTEDLevel(const DTEDLevelEnum &level)
   {
      mMaxDTEDLevel = &level;      
   }
    
}
