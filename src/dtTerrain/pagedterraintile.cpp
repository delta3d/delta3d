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
#include <dtTerrain/pagedterraintile.h>
#include <dtTerrain/heightfield.h>
#include <dtTerrain/terrain.h>
#include <dtTerrain/terraindatareader.h>
#include <dtTerrain/terraindatarenderer.h>
#include <dtTerrain/terraindecorationlayer.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>

#include <dtCore/scene.h>

#include <iostream>
#include <fstream>

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(PagedTerrainTileResourceName);
   const PagedTerrainTileResourceName 
      PagedTerrainTileResourceName::HEIGHTFIELD_FILENAME("heightmap.hfb");
      
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(PagedTerrainTileLoadState);
   const PagedTerrainTileLoadState PagedTerrainTileLoadState::LOADED("LOADED");
   const PagedTerrainTileLoadState PagedTerrainTileLoadState::PAGING("PAGING");
   const PagedTerrainTileLoadState PagedTerrainTileLoadState::NOT_LOADED("NOT_LOADED");
         
      
   //////////////////////////////////////////////////////////////////////////
   PagedTerrainTile::PagedTerrainTile(Terrain *parent) : mParentTerrain(parent)
   {
      mEnableCaching = false;
      mUpdateCache = false;
      mCachePath = "";
   }
   
   //////////////////////////////////////////////////////////////////////////
   PagedTerrainTile::~PagedTerrainTile()
   {
   
   }
   
   //////////////////////////////////////////////////////////////////////////
   void PagedTerrainTile::SetCachePath(const std::string &path)
   {
      if (!path.empty())
      {
         mCachePath = path;
         mEnableCaching = true;         
      }
      else
      {
         mCachePath = "";
         mEnableCaching = false;
         mUpdateCache = false;
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void PagedTerrainTile::WriteToCache()
   {
      if (!IsCachingEnabled() || !GetUpdateCache())
         return;
      
      //Cache the heightfield.
      if (mHeightField.valid() && mHeightField->GetNumColumns() != 0 &&
         mHeightField->GetNumRows() != 0)
      {
         //The heightfield data is cached in its raw form to prevent loss of 
         //data due to conversions and such.
         std::ofstream outFile;
         std::string path = mCachePath + "/" + 
            PagedTerrainTileResourceName::HEIGHTFIELD_FILENAME.GetName();         
        
         outFile.open(path.c_str(),std::ios::out | std::ios::trunc | std::ios::binary);
         if (!outFile.is_open())
         {
            LOG_ERROR("Unable to cache height field data.  Could not open the cache file "
               "for writing.");
         }
         else         
         {
            unsigned int numCols = mHeightField->GetNumColumns();
            unsigned int numRows = mHeightField->GetNumRows();
            
            outFile.write((char *)&numCols,sizeof(unsigned int));
            outFile.write((char *)&numRows,sizeof(unsigned int));
            outFile.write((char *)mHeightField->GetHeightFieldData(),
               numCols*numRows*sizeof(short));
         }
         
         outFile.close();        
      }
      
      //Cache the base texture image if present for this tile.
      
   }
   
   //////////////////////////////////////////////////////////////////////////
   void PagedTerrainTile::ReadFromCache()
   {
      if (!IsCachingEnabled())
         return;
      
      //Attempt to load the heightfield if it is present.
      std::ifstream inFile;
      std::string path = mCachePath + "/" + 
         PagedTerrainTileResourceName::HEIGHTFIELD_FILENAME.GetName();
      inFile.open(path.c_str(),std::ios::in | std::ios::binary);
      if (inFile.is_open())
      {
         unsigned int numRows,numCols;
         
         inFile.read((char *)&numCols,sizeof(unsigned int));
         inFile.read((char *)&numRows,sizeof(unsigned int));
         
         short *data = new short[numCols*numRows];
         inFile.read((char *)&data[0],numCols*numRows*sizeof(short));
         mHeightField = new HeightField();
         mHeightField->ConvertFromRaw(numCols,numRows,data);
         
         inFile.close();
      }
   }
   
}
