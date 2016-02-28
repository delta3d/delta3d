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
#include <osgDB/FileUtils>
#include <osg/MatrixTransform>

#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>

#include <dtTerrain/terrain.h>
#include <dtTerrain/terraindatareader.h>
#include <dtTerrain/terraindatarenderer.h>
#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/vegetationdecorator.h>
#include <dtTerrain/pagedterraintile.h>
#include <dtTerrain/heightfield.h>

#include <sstream>
#include <algorithm>

namespace dtTerrain 
{

   //////////////////////////////////////////////////////////////////////////
   IMPLEMENT_MANAGEMENT_LAYER(Terrain);

   //////////////////////////////////////////////////////////////////////////    
   class TerrainCullCallback : public osg::NodeCallback
   {
   public:

      TerrainCullCallback(Terrain *terrain) : mTerrain(terrain) { }         
      virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
      {
         GeoCoordinates coords;
         int i,j;

         coords.SetCartesianPoint(nv->getEyePoint());

         //Now that we have the location of the camera, figure out how many tiles to 
         //load.  The tiles to load are based on latitude and longitude for now.  A
         //cartesian based system should probably be used instead.
         double bounds = (mTerrain->GetLoadDistance() / GeoCoordinates::EQUATORIAL_RADIUS) * 
            osg::RadiansToDegrees(1.0);

         int minLat = (int)floor(coords.GetLatitude() - bounds);
         int maxLat = (int)ceil(coords.GetLatitude() + bounds);
         int minLon = (int)floor(coords.GetLongitude() - bounds);
         int maxLon = (int)ceil(coords.GetLongitude() + bounds);

         //First build a set of tiles that should be resident for this frame.
         std::set<GeoCoordinates> residentTileLocations;
         for (i=minLat; i<=maxLat; i++)
         {
            for (j=minLon; j<=maxLon; j++)
            {
               GeoCoordinates resCoords;
               resCoords.SetLatitude(i);
               resCoords.SetLongitude(j);
               resCoords.SetAltitude(0);
               residentTileLocations.insert(resCoords);   
            }   
         }

         //Inform the terrain of the tile set that should be visible for this
         //frame.
         mTerrain->EnsureTileVisibility(residentTileLocations);  
         traverse(node,nv);     
      }

   private:
      Terrain *mTerrain;
   };   

   //////////////////////////////////////////////////////////////////////////
   Terrain::Terrain(const std::string &name)
   {
      RegisterInstance(this);
      SetName(name);
      mLoadDistance = 30000.0f;      
      SetTerrainTileFactory(*(new PagedTerrainTileFactory()));
      dtCore::System::GetInstance().TickSignal.connect_slot(this, &Terrain::OnSystem);

      SetLineOfSightSpacing(25.0f); // a bit less than DTED L2
   }

   //////////////////////////////////////////////////////////////////////////
   Terrain::~Terrain()
   {
      //Be sure to clear the resident list of tiles, moving them to the
      //unload queue so they can be safely unloaded and then flush the queue.
      LOG_INFO("Cleaning up and flushing the tile unload queue.");
      UnloadAllTerrainTiles();
      PostFrame(-1.0);      
      DeregisterInstance(this);
   }    

   ////////////////////////////////////////////////////////////////////////// 
   float Terrain::GetHeight(float x, float y)
   {
      if (!mDataRenderer.valid())
         throw dtTerrain::InvalidDataRendererException(
         "Cannot retrieve the height of the terrain.", __FILE__, __LINE__);

      return mDataRenderer->GetHeight(x,y);  
   }

   ////////////////////////////////////////////////////////////////////////// 
   bool Terrain::IsClearLineOfSight( const osg::Vec3& pointOne,
                                     const osg::Vec3& pointTwo )
   {
      // Would be nice to have a functor instead of very simple built-in
      return SimpleLineOfSight(this, pointOne, pointTwo);
   }

   //////////////////////////////////////////////////////////////////////////
   bool Terrain::SetCachePath(const std::string &path)
   {
      //Make sure we remove any trailing slashes from the cache path.
      std::string newPath(path);
      if (newPath[newPath.length()-1] == '/' || newPath[newPath.length()-1] == '\\')
         newPath = newPath.substr(0,newPath.length()-1);

      if (!dtUtil::FileUtils::GetInstance().DirExists(newPath))
      {
         try 
         {
            dtUtil::FileUtils::GetInstance().MakeDirectory(newPath);
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
   void Terrain::AddResourcePath(const std::string &path)
   {
      //Before we add the path, make sure we remove any trailing slashes...
      if (path[path.length()-1] == '/' || path[path.length()-1] == '\\')
         mResourcePathList.push_back(path.substr(0,path.length()-1));
      else
         mResourcePathList.push_back(path);
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::RemoveResourcePath(const std::string &path)
   {
      std::list<std::string>::iterator itor;
      for (itor=mResourcePathList.begin(); itor!=mResourcePathList.end(); ++itor)
      {
         if (*itor == path)
         {
            mResourcePathList.erase(itor);
            break;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////   
   const std::string Terrain::FindResource(const std::string &path)
   {
      osgDB::FilePathList filePaths = osgDB::getDataFilePathList();
      osgDB::FilePathList::const_iterator pathItor;
      std::list<std::string>::iterator resItor;     
      std::string fullPath;

      //In order to find a resource, we must try all combinations of 
      //the terrain's resource path list with that of the Delta3D
      //path lists.
      for (pathItor=filePaths.begin(); pathItor!=filePaths.end(); ++pathItor)
      {
         if (mResourcePathList.empty())
         {
            fullPath = osgDB::findFileInDirectory(path,*pathItor);
            if (!fullPath.empty())
               return fullPath;  
         }
         else
         {
            for (resItor=mResourcePathList.begin(); 
               resItor!=mResourcePathList.end(); ++resItor)
            {
               fullPath = osgDB::findFileInDirectory(path,*pathItor + "/" + *resItor,
                  osgDB::CASE_INSENSITIVE);
               if (!fullPath.empty())
                  return fullPath;
            }
         }
      }

      return fullPath;
   }

   //////////////////////////////////////////////////////////////////////////   
   void Terrain::FindAllResources(const std::string &path, 
      std::vector<std::string> &resourcePaths)
   {
      osgDB::FilePathList filePaths = osgDB::getDataFilePathList();
      osgDB::FilePathList::const_iterator pathItor;
      std::list<std::string>::iterator resItor;     
      std::string fullPath;

      //Just to make sure, clear the result vector.
      resourcePaths.clear();

      //In order to find a resource, we must try all combinations of 
      //the terrain's resource path list with that of the Delta3D
      //path lists.
      for (pathItor=filePaths.begin(); pathItor!=filePaths.end(); ++pathItor)
      {
         if (mResourcePathList.empty())
         {
            fullPath = osgDB::findFileInDirectory(path,*pathItor);
            if (!fullPath.empty())
               resourcePaths.push_back(fullPath);
         }
         else
         {
            for (resItor=mResourcePathList.begin(); 
               resItor!=mResourcePathList.end(); ++resItor)
            {
               fullPath = osgDB::findFileInDirectory(path,*pathItor + "/" + *resItor,
                  osgDB::CASE_INSENSITIVE);
               if (!fullPath.empty())
                  resourcePaths.push_back(fullPath);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////   
   PagedTerrainTile *Terrain::CreateTerrainTile(const GeoCoordinates &coords)
   {
      if (!mDataReader.valid())
      {
         LOG_ERROR("Cannot create new tile.  The terrain data reader is NULL.");
         return NULL;
      }

      if (!mTileFactory.valid())
      {
         LOG_ERROR("Cannot create new paged terrain tile.  The tile factory is NULL.");
         return NULL;
      }

      PagedTerrainTile *newTile = mTileFactory->CreateNewTile(coords,*this);
      if (!newTile)
         return NULL;     

      return newTile;
   }

   //////////////////////////////////////////////////////////////////////////   
   void Terrain::LoadTerrainTile(PagedTerrainTile &newTile)
   {
      mTilesToLoadQ.push(&newTile);
      mResidentTiles.insert(std::make_pair(newTile.GetGeoCoordinates(),&newTile));
   }

   //////////////////////////////////////////////////////////////////////////   
   void Terrain::UnloadTerrainTile(PagedTerrainTile &toRemove)
   {
      //Remove it from the resident list of tiles and queue it up in the list
      //of tiles to unload.   
      mTilesToUnloadQ.push(&toRemove);
      mResidentTiles.erase(toRemove.GetGeoCoordinates());     
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::UnloadAllTerrainTiles()
   {
      TerrainTileMap::iterator itor;
      for (itor=mResidentTiles.begin(); itor!=mResidentTiles.end(); ++itor)
         mTilesToUnloadQ.push(itor->second.get());
      mResidentTiles.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   bool Terrain::IsTerrainTileResident(const GeoCoordinates &coords)
   {
      TerrainTileMap::iterator itor = mResidentTiles.find(coords);
      if (itor == mResidentTiles.end())
         return false;
      else
         return true;
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::EnsureTileVisibility(const std::set<GeoCoordinates> &coordList)
   {
      //This is a two pass operation.  First we need to unload the tiles that
      //are visible but shouldn't be.  Second, we need to load the tiles that
      //are visible but not currently in the resident tiles list.
      std::set<GeoCoordinates>::const_iterator visItor;
      TerrainTileMap::iterator resItor;
      std::vector<dtCore::RefPtr<PagedTerrainTile> > result;
      std::vector<dtCore::RefPtr<PagedTerrainTile> >::iterator resultItor;

      //Loop through the currently visible set of tiles, if there is a tile not 
      //in the newly specified list, unload it.      
      resItor = mResidentTiles.begin();
      while (resItor != mResidentTiles.end())
      {
         if (!std::binary_search(coordList.begin(),coordList.end(),
            resItor->second->GetGeoCoordinates()))
         {
            result.push_back(resItor->second);
         }       

         ++resItor;
      }

      for (resultItor=result.begin(); resultItor!=result.end(); ++resultItor)
         UnloadTerrainTile(*resultItor->get());

      //Now we need to make sure all tiles from the requested visible set
      //that are not currently loaded are put in the load queue.
      for (visItor=coordList.begin(); visItor!=coordList.end(); ++visItor)
      {
         resItor = mResidentTiles.find(*visItor);
         if (resItor == mResidentTiles.end())
         {
            PagedTerrainTile *newTile = CreateTerrainTile(*visItor);
            LoadTerrainTile(*newTile);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

   {

      if (str == dtCore::System::MESSAGE_PRE_FRAME)
         PreFrame(deltaSim);
      else if (str == dtCore::System::MESSAGE_POST_FRAME)
         PostFrame(deltaSim);     
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::PreFrame(double frameTime)
   {      
      //To flush the load queue, we pass the terrain tile through four
      //stages.  Exception handling is done on a per stage basis.  Therefore,
      //failure on one stage does not mean the tile will not load.  The only 
      //exception to this rule occurs when the reader fails to load.  In this 
      //case the tile is removed from the queue and safely igored.  For example,
      //if the application specific cached data cannot load, the other parts of the
      //tile (heightfield, decorators, etc.) may still load assuming they are not
      //dependent on the failed stages.
      if (!mDataReader.valid())
         throw dtTerrain::InvalidDataReaderException(
         "Cannot flush the terrain tile load queue.  The terrain reader is not valid.", __FILE__, __LINE__);

      if (!mDataRenderer.valid())
         throw dtTerrain::InvalidDataRendererException(
         "Cannot flush the terrain tile load queue.  The terrain renderer is not valid.", __FILE__, __LINE__);

      while (!mTilesToLoadQ.empty())      
      {
         PagedTerrainTile *currTile = mTilesToLoadQ.front().get();

         //Create a cache path for the tile being loaded if it does not already
         //exist.
         if (!mCachePath.empty())
         {
            std::string tilePath = mCachePath + "/" + "tile_" + 
               mDataReader->GenerateTerrainTileCachePath(*currTile);

            //Now that we generated a tile's cache path, make sure it exists.  If it does
            //not go ahead and create it.
            if (!dtUtil::FileUtils::GetInstance().DirExists(tilePath))
            {
               try 
               {
                  dtUtil::FileUtils::GetInstance().MakeDirectory(tilePath);
                  currTile->SetCachePath(tilePath);                  
               }
               catch (dtUtil::Exception &ex)
               {
                  ex.LogException(dtUtil::Log::LOG_ERROR);
               }
            }
            else
            {
               currTile->SetCachePath(tilePath);
            }
         }
         else
         {
            currTile->SetCachePath("");
         }

         //First, we tell the tile to load any tile specific data from its cache.
         //This is to allow subclassed terrain tiles to cache and restore application
         //specific data.  Note, the base paged tile implementation of this method
         //will load any basic data from its cache if present.
         try
         {
            currTile->ReadFromCache();

            //When the tile is first loaded its contents are in sync with its cache.
            //This should be set to "true" by either an external class if any tile
            //related data needs to be updated in the cache.
            currTile->SetUpdateCache(false);
         }
         catch (dtUtil::Exception &ex)
         {
            LOG_ERROR("Error loading terrain tile. (RestoreFromCache): " + ex.What());
         }

         //Second, tell the terrain reader we need to load the tile.
         try
         {
            if (!mDataReader->OnLoadTerrainTile(*currTile))
            {
               mTilesToLoadQ.pop();
               continue;
            }
         }
         catch (dtUtil::Exception &ex)
         {
            ex.What();
            //The responsibility of error reporting is left up to the terrain 
            //reader in this case as to avoid too many redundant error messages.
            mTilesToLoadQ.pop();
            continue;          
         }       

         //Third, we pass the terrain tile to each of the decorator
         //layers so they may load or create data relating to the tile.
         TerrainLayerMap::iterator layerItor;
         for (layerItor=mDecorationLayers.begin(); layerItor!=mDecorationLayers.end(); 
            ++layerItor)
         {
            try
            {
               layerItor->second->OnLoadTerrainTile(*currTile);   
            }
            catch (dtUtil::Exception &ex)
            {
               LOG_ERROR("Error loading tile in decoration layer. (" + layerItor->first
                  + "):  " + ex.What());
            }  
         }  

         //Finally, we tell the terrain renderer to load the tile.  This gives the
         //renderer a chance to generate, preprocess, or do any data loading
         //it needs for an individual tile.
         try
         {
            mDataRenderer->OnLoadTerrainTile(*currTile);
         }
         catch (dtUtil::Exception &ex)
         {
            LOG_ERROR("Error loading terrain tile. (TerrainRenderer): " + ex.What());
         }         

         //Need to make one final pass over all the decorators in case they need to 
         //perform any post tile loading operations.
         for (layerItor=mDecorationLayers.begin(); layerItor!=mDecorationLayers.end(); 
            ++layerItor)
         {
            try
            {
               /*GeoCoordinates c =*/ currTile->GetGeoCoordinates();
               layerItor->second->OnTerrainTileResident(*currTile);   
            }
            catch (dtUtil::Exception &ex)
            {
               LOG_ERROR("Error processing tile in decoration layer. (" + layerItor->first
                  + "):  " + ex.What());
            }  
         }  

         //Now once we have finished loading a tile, remove it from the load queue,
         //and add it to the lisbt of loaded and resident tiles.
         mTilesToLoadQ.pop();
      }     
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::PostFrame(double frameTime)
   {
      //In the same way we passed tiles through different stages to load them,
      //we conversly pass them through the same stages to unload them.  This 
      //allows each stage to possibly cache data before unloading and perform
      //any necessary clean up operations that should occur.      
      if (!mDataReader.valid())
         throw dtTerrain::InvalidDataReaderException(
         "Cannot flush the terrain tile load queue.  The terrain reader is not valid.", __FILE__, __LINE__);

      if (!mDataRenderer.valid())
         throw dtTerrain::InvalidDataRendererException(
         "Cannot flush the terrain tile load queue.  The terrain renderer is not valid.", __FILE__, __LINE__);

      while (!mTilesToUnloadQ.empty())
      {
         LOG_INFO("UnLoading new terrain tile.");
         PagedTerrainTile *currTile = mTilesToUnloadQ.front().get();

         //First, we tell the tile to unload any tile specific data to its cache.
         //This is to allow subclassed terrain tiles to save and restore application
         //specific data.  By default, heightfield data and base image data are cached.
         try
         {
            if (currTile->GetUpdateCache())
               currTile->WriteToCache();
         }
         catch (dtUtil::Exception &ex)
         {
            LOG_ERROR("Error unloading terrain tile. (SaveToCache): " + ex.What());
         }

         //Second, inform the terrain reader that a tile is being unloaded.
         try
         {
            mDataReader->OnUnloadTerrainTile(*currTile);
         }
         catch (dtUtil::Exception &ex)
         {
            LOG_ERROR("Error unloading terrain tile. (TerrainReader): " + ex.What());
         }         

         //Third, we pass the terrain tile to each of the decorator
         //layers so they may cache any data relating to the tile.
         TerrainLayerMap::iterator layerItor;
         for (layerItor=mDecorationLayers.begin(); layerItor!=mDecorationLayers.end(); 
            ++layerItor)
         {
            try
            {
               layerItor->second->OnUnloadTerrainTile(*currTile);
            }
            catch (dtUtil::Exception &ex)
            {
               LOG_ERROR("Error unloading tile in decoration layer. (" + layerItor->first
                  + "):  " + ex.What());
            }  
         }       

         //Finally, we tell the terrain renderer to unload the tile.  This gives the
         //renderer a chance to save off any data it does not want to pregenerate
         //every time a tile is loaded.
         try
         {
            mDataRenderer->OnUnloadTerrainTile(*currTile);
         }
         catch (dtUtil::Exception &ex)
         {
            LOG_ERROR("Error unloading terrain tile. (TerrainRenderer): " + ex.What());
         }

         //Finally, we're done.
         mTilesToUnloadQ.pop();         
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::SetDataReader(TerrainDataReader *reader)
   {
      if (mDataReader != NULL)
         mDataReader->mParentTerrain = NULL;

      mDataReader = reader;
      if (mDataReader != NULL)
         mDataReader->mParentTerrain = this;
   }

   //////////////////////////////////////////////////////////////////////////   
   void Terrain::SetDataRenderer(TerrainDataRenderer *renderer)
   {
      if (mDataRenderer != NULL)
         mDataRenderer->mParentTerrain = NULL;

      mDataRenderer = renderer;

      //Now we need to initialize the renderer.  Once it has been initialized
      //we can add its scene graph node.
      if (mDataRenderer != NULL)
      {
         mDataRenderer->mParentTerrain = this;
         GetMatrixNode()->addChild(mDataRenderer->GetRootDrawable());

         //If we have a valid renderer that means we should enable the
         //cull callback which queues the visible tiles to be rendered.
         GetMatrixNode()->setCullCallback(new TerrainCullCallback(this));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::AddDecorationLayer(TerrainDecorationLayer *newLayer)
   {
      if (newLayer == NULL)
         throw dtTerrain::InvalidDecorationLayerException(
         "Cannot add NULL decoration layer.", __FILE__, __LINE__);

      //First make sure we have a unique name for the new layer.      
      TerrainLayerMap::iterator itor = mDecorationLayers.find(newLayer->GetName());

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
      if (newLayer->IsVisible())
         GetMatrixNode()->addChild(newLayer->GetOSGNode());           
   }         

   //////////////////////////////////////////////////////////////////////////
   void Terrain::RemoveDecorationLayer(TerrainDecorationLayer *toRemove)
   {
      TerrainLayerMap::iterator itor = mDecorationLayers.begin();

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
      TerrainLayerMap::iterator itor = mDecorationLayers.find(name);

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
      TerrainLayerMap::iterator itor = mDecorationLayers.find(name);

      if (itor != mDecorationLayers.end())
         return itor->second.get();
      else 
         return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   const TerrainDecorationLayer *Terrain::GetDecorationLayer(const std::string &name) const
   {
      TerrainLayerMap::const_iterator itor = mDecorationLayers.find(name);

      if (itor != mDecorationLayers.end())
         return itor->second.get();
      else
         return NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   void Terrain::ClearDecorationLayers()
   {
      TerrainLayerMap::iterator itor;

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
      TerrainLayerMap::iterator itor = mDecorationLayers.begin();

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

   bool SimpleLineOfSight( dtTerrain::Terrain* terrain,
                           const osg::Vec3& pointOne,
                           const osg::Vec3& pointTwo )
   {   
      osg::Vec3 ray = pointTwo - pointOne;
      double length( ray.length() );
      // If closer than post spacing, then clear LOS
      if( length < terrain->GetLineOfSightSpacing() )
      {
         return true;
      }

      float stepsize( terrain->GetLineOfSightSpacing() / length );
      double s( 0.0 );

      while( s < 1.0 )
      {
         osg::Vec3 testPt = pointOne + ray*s;
         double h( terrain->GetHeight( testPt.x(), testPt.y() ) );

         // Segment blocked by terrain
         if( h >= testPt.z() )
         {
            return false;
         }
         s += stepsize;
      }

      // Walked full ray, so clear LOS
      return true;
   }


   ////////////////////////////////////////////////////////////////////////////////
   NullPointerException::NullPointerException(const std::string& message, const std::string& filename, unsigned int linenum)
      : dtUtil::Exception(message, filename, linenum)
   {
   }
    ////////////////////////////////////////////////////////////////////////////////
   InvalidDataRendererException::InvalidDataRendererException(const std::string& message, const std::string& filename, unsigned int linenum) 
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidDataReaderException::InvalidDataReaderException(const std::string& message, const std::string& filename, unsigned int linenum) 
      : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   InvalidDecorationLayerException::InvalidDecorationLayerException(const std::string& message, const std::string& filename, unsigned int linenum) 
      : dtUtil::Exception(message, filename, linenum)
   {
   }
}
