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
#include <dtCore/scene.h>
#include <dtDAL/fileutils.h>
#include <dtUtil/exception.h>

#include "dtTerrain/terrain.h"
#include "dtTerrain/terraindatareader.h"
#include "dtTerrain/terraindatarenderer.h"
#include "dtTerrain/terraindecorationlayer.h"
#include "dtTerrain/vegetationdecorator.h"

#include <sstream>

namespace dtTerrain 
{
    
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_MANAGEMENT_LAYER(Terrain);
   
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(TerrainException);
   
   TerrainException TerrainException::INVALID_RESOURCE_PATH("INVALID_RESOURCE_PATH");
   TerrainException TerrainException::UNSUPPORTED_DATA_FORMAT("UNSUPPORTED_DATA_FORMAT");
   TerrainException TerrainException::INVALID_DATA_READER("INVALID_DATA_READER");
   TerrainException TerrainException::INVALID_DATA_RENDERER("INVALID_DATA_RENDERER");
   TerrainException TerrainException::INVALID_DECORATION_LAYER("INVALID_DECORATION_LAYER");
   
   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(TerrainCoordinateSystem);
   const TerrainCoordinateSystem TerrainCoordinateSystem::CARTESIAN("CARTESIAN");
   const TerrainCoordinateSystem TerrainCoordinateSystem::GEOGRAPHIC("GEOGRAPHIC");
   
   
   //////////////////////////////////////////////////////////////////////////
   Terrain::Terrain(const std::string &name)
   {
      RegisterInstance(this);
      SetName(name);
      mCoordinateSystem = &TerrainCoordinateSystem::GEOGRAPHIC;
   }
   
   //////////////////////////////////////////////////////////////////////////
   Terrain::~Terrain()
   {
      DeregisterInstance(this);
   }    

   //////////////////////////////////////////////////////////////////////////   
   void Terrain::LoadResource(const std::string &path)
   {
      if (!mDataReader.valid())
         EXCEPT(TerrainException::INVALID_DATA_READER,"Cannot load resource. "
            "Terrain does not have a data reader assigned to it.");
      
      if (!mDataRenderer.valid())
         EXCEPT(TerrainException::INVALID_DATA_RENDERER,"Cannot load resource. "
            "Terrain does not have a data renderer assigned to it.");
      
      //Make sure to remove any trailing slashes in the dataPath that was given.
      std::string dataPath = path;
      if (dataPath[dataPath.length()-1] == '/' || dataPath[dataPath.length()-1] == '\\')
         dataPath = dataPath.substr(0,path.length()-2);
           
      try 
      {
         //Depending on what coordinate system we are currently using,
         //ask the reader to load the terrain data.  This probably should be
         //thought out better in the future.  This method seems a little rigid
         //to me.
         if (GetCoordinateSystem() == TerrainCoordinateSystem::CARTESIAN)
            mDataReader->LoadElevationData(dataPath,mOrigin.x(),mOrigin.y());
         else if (GetCoordinateSystem() == TerrainCoordinateSystem::GEOGRAPHIC)
            mDataReader->LoadElevationData(dataPath,(int)floor(mOrigin.x()),(int)floor(mOrigin.y()));
            
         //Now that we have loaded the height data, hand it over to the renderer.
         mDataRenderer->SetHeightField(mDataReader->GetHeightField());
         mDataRenderer->Initialize();
         GetMatrixNode()->addChild(mDataRenderer->GetRootDrawable());
         
         for (TerrainLayer::iterator itor=mDecorationLayers.begin(); itor!=mDecorationLayers.end(); ++itor)
         {
            itor->second->LoadResource((int)floor(mOrigin.x()),(int)floor(mOrigin.y()));
            if (itor->second->IsVisible())
               GetMatrixNode()->addChild(itor->second->GetOSGNode());   
         }
      }
      catch (dtUtil::Exception &ex)
      {
         EXCEPT(TerrainException::UNSUPPORTED_DATA_FORMAT,ex.What());
      }
      
      mDataPath = path;
   }

   //////////////////////////////////////////////////////////////////////////   
   void Terrain::SetOrigin(const osg::Vec3d &origin)
   {
      mOrigin = origin;
   }   

   //////////////////////////////////////////////////////////////////////////   
   const osg::Vec3d &Terrain::GetOrigin() const
   {
      return mOrigin;
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   float Terrain::GetHeight(float x, float y)
   {
      if (!mDataRenderer.valid())
      {
         LOG_ERROR("Cannot retrieve the height of the terrain.  The current "
            " renderer is not valid.");
         return 0.0f;
      }
      
      return mDataRenderer->GetHeight(x,y);
     
      /*int latitude = (int)floorf((y/SEMI_MAJOR_AXIS) * 
         osg::RadiansToDegrees(1.0f) + (int)floorf(mOrigin.y()));
      int longitude = (int)floorf((x/SEMI_MAJOR_AXIS) *
         osg::RadiansToDegrees(1.0f) + (int)floorf(mOrigin.x()));
         
      return mDataRenderer->GetHeight(
         x - (longitude - (int)floorf(mOrigin.x())) *
            osg::DegreesToRadians(1.0f) * SEMI_MAJOR_AXIS,
         y - (1 + latitude - (int)floorf(mOrigin.y())) * 
            osg::DegreesToRadians(1.0f) * SEMI_MAJOR_AXIS);*/
   }
   
   //////////////////////////////////////////////////////////////////////////
   bool Terrain::SetCachePath(const std::string &path)
   {
      if (!dtDAL::FileUtils::GetInstance().DirExists(path))
      {
         try 
         {
            dtDAL::FileUtils::GetInstance().MakeDirectory(path);
         }
         catch (dtUtil::Exception &ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR);
            return false;
         }         
      }
      
      mCachePath = path;
      return true;      
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::SetDataReader(TerrainDataReader *reader)
   {
      if (mDataReader != NULL)
         mDataReader->mParentTerrain = NULL;
         
      mDataReader = reader;
      mDataReader->mParentTerrain = this;
   }

   //////////////////////////////////////////////////////////////////////////   
   void Terrain::SetDataRenderer(TerrainDataRenderer *renderer)
   {
      if (mDataRenderer != NULL)
         mDataRenderer->mParentTerrain = NULL;
         
      mDataRenderer = renderer;
      mDataRenderer->mParentTerrain = this;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void Terrain::AddDecorationLayer(TerrainDecorationLayer *newLayer)
   {
      if (newLayer == NULL)
         EXCEPT(TerrainException::INVALID_DECORATION_LAYER,
            "Cannot add NULL decoration layer.");
      
      //First make sure we have a unique name for the new layer.      
      TerrainLayer::iterator itor = mDecorationLayers.find(newLayer->GetName());
      
      if (itor != mDecorationLayers.end())
      {
         static int layerCount = 0;
         std::string oldName = newLayer->GetName();
         std::ostringstream newName;
         newName << oldName << layerCount++;
         newLayer->SetName(newName.str());
         LOG_WARNING("New decoration layer name was not unique. OldName: " +
            oldName + " NewName: " + newLayer->GetName());
      }
      
      //Finally add the new layer to our list and to the terrain itself.
      newLayer->mParentTerrain = this;
      mDecorationLayers.insert(std::make_pair(newLayer->GetName(),newLayer));      
      //if (newLayer->IsVisible())
      //   GetMatrixNode()->addChild(newLayer->GetOSGNode());           
   }         
     
   //////////////////////////////////////////////////////////////////////////
   void Terrain::RemoveDecorationLayer(TerrainDecorationLayer *toRemove)
   {
      TerrainLayer::iterator itor = mDecorationLayers.begin();
      
      while (itor != mDecorationLayers.end())
      {
         if (itor->second.get() == toRemove)
            break;
         ++itor;
      }
      
      toRemove->mParentTerrain = NULL;
      GetMatrixNode()->removeChild(toRemove->GetOSGNode());
      mDecorationLayers.erase(itor);
   }
         
   //////////////////////////////////////////////////////////////////////////
   void Terrain::RemoveDecorationLayer(const std::string &name)
   {
      TerrainLayer::iterator itor = mDecorationLayers.find(name);
      
      if (itor != mDecorationLayers.end())
      {
         itor->second->mParentTerrain = NULL;
         GetMatrixNode()->removeChild(itor->second->GetOSGNode());
         mDecorationLayers.erase(itor);
      }
   }
         
   //////////////////////////////////////////////////////////////////////////
   TerrainDecorationLayer *Terrain::GetDecorationLayer(const std::string &name)
   {
      TerrainLayer::iterator itor = mDecorationLayers.find(name);
      
      if (itor != mDecorationLayers.end())
         return itor->second.get();
      else 
         return NULL;
   }
         
   //////////////////////////////////////////////////////////////////////////
   const TerrainDecorationLayer *Terrain::GetDecorationLayer(const std::string &name) const
   {
      TerrainLayer::const_iterator itor = mDecorationLayers.find(name);
      
      if (itor != mDecorationLayers.end())
         return itor->second.get();
      else
         return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::ClearDecorationLayers()
   {
      TerrainLayer::iterator itor;
      
      for (itor=mDecorationLayers.begin(); itor!=mDecorationLayers.end(); ++itor)
      {
         itor->second->mParentTerrain = NULL;
         GetMatrixNode()->removeChild(itor->second->GetOSGNode());
      }
      
      mDecorationLayers.clear();         
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void Terrain::HideDecorationLayer(const std::string &name)
   {
      TerrainDecorationLayer *layer = GetDecorationLayer(name);
      if (layer != NULL)
      {
         layer->mIsVisible = false;
         GetMatrixNode()->removeChild(layer->GetOSGNode());
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void Terrain::HideDecorationLayer(TerrainDecorationLayer *toHide)
   {
      if (toHide == NULL)
         return;
      
      toHide->mIsVisible = false;
      GetMatrixNode()->removeChild(toHide->GetOSGNode());
   }
   
   //////////////////////////////////////////////////////////////////////////    
   void Terrain::ShowDecorationLayer(const std::string &name)
   {
      TerrainDecorationLayer *layer = GetDecorationLayer(name);
      if (layer != NULL)
      {
         layer->mIsVisible = true;
         if (!GetMatrixNode()->containsNode(layer->GetOSGNode()))
            GetMatrixNode()->addChild(layer->GetOSGNode());
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void Terrain::ShowDecorationLayer(TerrainDecorationLayer *toShow)
   {
      if (toShow == NULL)
         return;
         
      toShow->mIsVisible = true;
      if (!GetMatrixNode()->containsNode(toShow->GetOSGNode()))
         GetMatrixNode()->addChild(toShow->GetOSGNode());
   }
      
   //////////////////////////////////////////////////////////////////////////   
   void Terrain::GetDecorationLayers(std::vector<dtCore::RefPtr<TerrainDecorationLayer> > &layers)
   {
      TerrainLayer::iterator itor = mDecorationLayers.begin();
      
      //Make sure we clear out any data that was passed to this method.
      if (!layers.empty())
         layers.clear();
         
      layers.reserve(mDecorationLayers.size());
      while (itor != mDecorationLayers.end())
      {
         layers.push_back(itor->second);
         ++itor;
      }
   }
}
