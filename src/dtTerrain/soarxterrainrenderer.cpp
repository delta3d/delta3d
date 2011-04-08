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

#include <dtCore/scene.h>

#include <osg/Group>
#include <osg/Shape>
#include <osg/Uniform>
#include <osg/Program>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osg/PolygonMode>
#include <osg/CullFace>
#include <osg/FrontFace>
#include <osg/io_utils>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>

#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>

#include <dtTerrain/terraindatareader.h>
#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/terrain.h>
#include <dtTerrain/soarxterrainrenderer.h>
#include <dtTerrain/imageutils.h>
#include <dtTerrain/soarxdrawable.h>
#include <dtTerrain/geocoordinates.h>
#include <dtTerrain/pagedterraintile.h>
#include <dtTerrain/heightfield.h>

namespace dtTerrain
{
   const float SoarXTerrainRenderer::GRADIENT_SCALE = 32.0f;   
   
   ////////////////////////////////////////////////////////////////////////// 
   IMPLEMENT_ENUM(SoarXCacheResourceName);   
   const SoarXCacheResourceName SoarXCacheResourceName::VERTEX_DATA("rawvertices.dat");
   const SoarXCacheResourceName SoarXCacheResourceName::DETAIL_VERTEX_NOISE("soarx_renderer_vertexnoise.dat");
   const SoarXCacheResourceName SoarXCacheResourceName::DETAIL_GRADIENT_TEXTURE("soarx_renderer_detailgradient.rgb");
   const SoarXCacheResourceName SoarXCacheResourceName::DETAIL_SCALE_MAP("soarx_renderer_scalemap.rgb");
   const SoarXCacheResourceName SoarXCacheResourceName::BASE_GRADIENT_TEXTURE("basegradient.rgb");

   //////////////////////////////////////////////////////////////////////////    
   SoarXTerrainRenderer::SoarXTerrainRenderer(const std::string& name) 
      : TerrainDataRenderer(name)
   {
      mFragShaderPath = "shaders/terrain.frag";
      mRootGroupNode = new osg::Group();
      mShaderProgram = NULL;
      mDetailNoise = NULL;
      mDetailGradientTexture = NULL;
      mDetailNoiseBits = 10;
      mDetailNoiseSize = 1 << mDetailNoiseBits;
      mDetailVerticalResolution = 0.0012f;
      mThreshold = 2.0f;
      mDetailMultiplier = 3.0f;
      mRenderWithFog = false;
      mUniformRenderWithFog = 0;
   }   
   
   //////////////////////////////////////////////////////////////////////////    
   SoarXTerrainRenderer::~SoarXTerrainRenderer()
   {
      delete [] mDetailNoise;
   } 
   
   //////////////////////////////////////////////////////////////////////////    
   void SoarXTerrainRenderer::OnLoadTerrainTile(PagedTerrainTile &tile)
   {
      //Before we load a tile, make sure the heightfield is valid AND
      //make sure the heightfield has valid dimensions. ( (2^n+1) x (2^n+1) )
      if (tile.GetHeightField() == NULL)
      {
         throw dtTerrain::InvalidHeightfieldDataException(
            "Cannot load terrain tile.  HeightField is NULL.", __FILE__, __LINE__);
      }
            
      //TODO CHECK DIMENSIONS!!!
      
      //If this is the first time this renderer is loading a tile, make sure we
      //have compute the data the renderer needs which is shared amoungst all the
      //terrain tiles.
      static bool firstTime = true;
      if (firstTime)
      {
         InitializeRenderer();
         firstTime = false;
      }
       
      //Each tile gets its own drawable. So we need to construct it and
      //add it to our drawable map.
      DrawableEntry newEntry;                 
      int baseSize = tile.GetHeightField()->GetNumColumns() - 1;
      
      double gridSpacing = GeoCoordinates::EQUATORIAL_RADIUS *
         osg::DegreesToRadians(1.0);
      
      double horizRes;
      int baseBits;
      
      baseBits = (int)(logf((float)baseSize) / logf(2.0f));    
      horizRes = gridSpacing / (double)(1 << baseBits);
      
      newEntry.drawable = new SoarXDrawable(baseBits,(float)horizRes);
      newEntry.drawable->SetThreshold(mThreshold);
      newEntry.drawable->SetDetailMultiplier(mDetailMultiplier);
      newEntry.drawable->SetDetailNoise(mDetailNoiseBits,
         mDetailVerticalResolution,mDetailNoise);         
      if (!newEntry.drawable->Build(tile))
         tile.SetUpdateCache(true);
      
      GeoCoordinates coords = tile.GetGeoCoordinates();
      osg::Geode *geode = new osg::Geode();
      newEntry.sceneNode = new osg::MatrixTransform();      
      
      osg::Vec3 origin = coords.GetCartesianPoint();
      newEntry.sceneNode->setMatrix(osg::Matrix::translate(origin));
           
      CheckBaseGradientCache(tile,newEntry);
      SetupRenderState(tile,newEntry,*geode->getOrCreateStateSet());
      geode->addDrawable(newEntry.drawable.get());
      newEntry.sceneNode->addChild(geode);
      mRootGroupNode->addChild(newEntry.sceneNode.get());
      
      mDrawables.insert(std::make_pair(&tile,newEntry));     
   }
   
   //////////////////////////////////////////////////////////////////////////
   void SoarXTerrainRenderer::OnUnloadTerrainTile(PagedTerrainTile &tile)
   {
      DrawableMap::iterator itor = mDrawables.find(&tile);
      if (itor != mDrawables.end())
      {
         //Cache render data for this tile before removing it from the 
         //renderer.
         if (tile.IsCachingEnabled())// && tile.GetUpdateCache())
         {
            LOG_INFO("Caching tile data: " + tile.GetCachePath());
            itor->second.drawable->WriteDataToCache(tile);            
         }
         
         mRootGroupNode->removeChild(itor->second.sceneNode.get());
         itor->second.sceneNode = NULL;
         itor->second.drawable = NULL;
         itor->second.baseGradientTexture = NULL;
         mDrawables.erase(itor);
      }
   }
         
   //////////////////////////////////////////////////////////////////////////    
   float SoarXTerrainRenderer::GetHeight(float x, float y)
   {
      GeoCoordinates coords;
      int lat,lon;

      coords.SetCartesianPoint(osg::Vec3(x,y,0));

      if (coords.GetLatitude() < 0.0)
         lat = -1 * (int)ceil(osg::absolute(coords.GetLatitude()));
      else
         lat = (int) (osg::absolute(coords.GetLatitude()));

      if (coords.GetLongitude() < 0.0)
         lon = -1 * (int)ceil(osg::absolute(coords.GetLongitude()));
      else
         lon = (int) (osg::absolute(coords.GetLongitude()));

      DrawableMap::iterator itor;
      for (itor=mDrawables.begin(); itor!=mDrawables.end(); ++itor)
      {
         int drawableLat, drawableLon;
         double tile_baseLat = itor->first->GetGeoCoordinates().GetLatitude();
         double tile_baseLong = itor->first->GetGeoCoordinates().GetLongitude();
         if (tile_baseLat < 0.0)
            drawableLat = -1 * (int)ceil(osg::absolute(tile_baseLat));
         else
            drawableLat = (int) (osg::absolute(tile_baseLat));

         if (tile_baseLong < 0.0)
            drawableLon = -1 * (int)ceil(osg::absolute(tile_baseLong));
         else
            drawableLon = (int) (osg::absolute(tile_baseLong));

         if (drawableLat == lat && drawableLon == lon)
         {
            GeoCoordinates testCoords;
            testCoords.SetLatitude(lat);
            testCoords.SetLongitude(lon);

            osg::Vec3 pos = testCoords.GetCartesianPoint();        
            return itor->second.drawable->GetHeight(x-pos.x(),y-pos.y());
         }
      }

      return 0.0f;
   }
         
   //////////////////////////////////////////////////////////////////////////    
   osg::Vec3 SoarXTerrainRenderer::GetNormal(float x, float y)
   {
      float z = GetHeight(x,y);
      osg::Vec3 v1,v2,normal;
      
      v1 = osg::Vec3(0.1f,0.0f,GetHeight(x+0.1f,y) - z);
      v2 = osg::Vec3(0.0f,0.1f,GetHeight(x,y+0.1f) - z);
      
      normal = v1 ^ v2;
      normal.normalize();
      return normal;      
   }
   
   //////////////////////////////////////////////////////////////////////////
   void SoarXTerrainRenderer::SetDetailMultiplier(float value)
   {
      mDetailMultiplier = osg::clampTo(value,1.0f,20.0f);
      DrawableMap::iterator itor;
      for (itor=mDrawables.begin(); itor!=mDrawables.end(); ++itor)
         itor->second.drawable->SetDetailMultiplier(mDetailMultiplier);
   }

   //////////////////////////////////////////////////////////////////////////   
   float SoarXTerrainRenderer::GetDetailMultiplier() const
   {
      return mDetailMultiplier;
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void SoarXTerrainRenderer::SetThreshold(float value)
   {
      mThreshold = osg::clampTo(value,1.0f,20.0f);
      DrawableMap::iterator itor;
      for (itor=mDrawables.begin(); itor!=mDrawables.end(); ++itor)
         itor->second.drawable->SetThreshold(mThreshold);
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   float SoarXTerrainRenderer::GetThreshold() const 
   {
      return mThreshold;
   }
         
   //////////////////////////////////////////////////////////////////////////
   void SoarXTerrainRenderer::CreateFragmentShader()
   {      
      dtCore::RefPtr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);
      std::string shaderPath = dtUtil::FindFileInPathList(mFragShaderPath);
      if (shaderPath.empty())
      {
         LOG_ERROR("Could not load terrain fragment shader.");
         return;
      }
      else
      {
         if (!fragShader->loadShaderSourceFromFile(shaderPath))
         {
            LOG_ERROR("Error loading the fragment shader.");
            return;
         }
      }
      
      mShaderProgram = new osg::Program();
      mShaderProgram->addShader(fragShader.get());
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void SoarXTerrainRenderer::SetupRenderState(PagedTerrainTile &tile,
      DrawableEntry &entry, osg::StateSet &ss)
   {
      ss.setAttributeAndModes(mShaderProgram.get(),osg::StateAttribute::ON |
         osg::StateAttribute::OVERRIDE);
      
      //This following code connects our textures to samplers so the 
      //fragment shader can access them.      
      osg::Uniform *uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,"detailGradient");
      uniform->set(0);
      ss.addUniform(uniform);
      
      uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,"baseGradient");
      uniform->set(1);
      ss.addUniform(uniform);
      
      mUniformRenderWithFog = new osg::Uniform(osg::Uniform::BOOL, "renderWithFog");
      mUniformRenderWithFog->set(mRenderWithFog);
      ss.addUniform(mUniformRenderWithFog.get());

      if (tile.GetBaseTextureImage() != NULL)
      {
         osg::Texture2D *baseColorTexture = new osg::Texture2D();
         baseColorTexture->setImage(tile.GetBaseTextureImage());
         baseColorTexture->setFilter(osg::Texture2D::MIN_FILTER,
            osg::Texture2D::LINEAR_MIPMAP_LINEAR);
         baseColorTexture->setFilter(osg::Texture2D::MAG_FILTER,
            osg::Texture2D::LINEAR);
         baseColorTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
         baseColorTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
      
         uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,"baseColor");
         uniform->set(2);
         ss.addUniform(uniform);

         ss.setTextureAttributeAndModes(2,baseColorTexture,osg::StateAttribute::ON);
      }
      
      //Setup our basic render state used for all tiles.
      osg::CullFace *cf = new osg::CullFace();
      osg::PolygonMode *pm = new osg::PolygonMode();
      osg::FrontFace *ff = new osg::FrontFace();
      
      ff->setMode(osg::FrontFace::CLOCKWISE);
      pm->setMode(osg::PolygonMode::FRONT,osg::PolygonMode::FILL);
      ss.setAttributeAndModes(cf,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
      ss.setAttributeAndModes(pm,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
      ss.setAttributeAndModes(ff,osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
      ss.setMode(GL_LIGHTING,osg::StateAttribute::OFF | 
         osg::StateAttribute::OVERRIDE);
         
      //Attach our shared textures...
      ss.setTextureAttributeAndModes(0,mDetailGradientTexture.get(),
         osg::StateAttribute::ON);
      ss.setTextureAttributeAndModes(1,entry.baseGradientTexture.get(),osg::StateAttribute::ON);
         
      //Setup automatic texture coordinate generation..
      osg::TexGen *texGen = new osg::TexGen();
      float dt;
      
      //Calculate the planer mapping parameters for the detail gradient texture.
      dt = 1.0f / (entry.drawable->GetDetailHorizontalResolution() * 
         (float)entry.drawable->GetDetailSize());
      texGen->setMode(osg::TexGen::OBJECT_LINEAR);
      texGen->setPlane(osg::TexGen::S,osg::Plane(dt,0,0,0));
      texGen->setPlane(osg::TexGen::T,osg::Plane(0,-dt,0,1));
      ss.setTextureAttributeAndModes(0,texGen);
      
      texGen = new osg::TexGen();
      dt = 1.0f / ((float)tile.GetHeightField()->GetNumColumns() * 
            entry.drawable->GetBaseHorizontalResolution());
      texGen->setMode(osg::TexGen::OBJECT_LINEAR);
      texGen->setPlane(osg::TexGen::S,osg::Plane(dt,0,0,0));
      texGen->setPlane(osg::TexGen::T,osg::Plane(0,-dt,0,1));
      ss.setTextureAttributeAndModes(1,texGen);
   }
     
   //////////////////////////////////////////////////////////////////////////
   void SoarXTerrainRenderer::InitializeRenderer()
   {
      if (mDetailNoise == NULL)
         CheckDetailNoiseCache();
      
      if (!mShaderProgram.valid())
         CreateFragmentShader();
         
      if (!mDetailGradientTexture.valid())
         CheckDetailGradientCache();
         
      //if (!mDetailScaleTexture.valid())
      //   CheckDetailScaleCache();
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void SoarXTerrainRenderer::CheckDetailNoiseCache()
   {
      if (GetParentTerrain()->GetCachePath().empty())
      {
         //Since we are not using a cache, just calculate the noise.
         CalculateDetailNoise();
         return;
      }
      
      std::string cachePath = GetParentTerrain()->GetCachePath() + "/" +
         SoarXCacheResourceName::DETAIL_VERTEX_NOISE.GetName();
             
      //See if the file already exists, if not, generate the data and save
      //it to the cache.
      std::ifstream inFile;
      inFile.open(cachePath.c_str(),std::ios::in | std::ios::binary);
      if (!inFile.is_open())
      {
         //We need to generate the data.
         CalculateDetailNoise();
               
         //Once we have calculated the noise, cache it immediatly.
         std::ofstream outFile;
         outFile.open(cachePath.c_str(),std::ios::out | std::ios::trunc | std::ios::binary);
         if (!outFile.is_open())
         {
            LOG_INFO("Could not open detail vertex noise cache file: " + cachePath);
         }
         else
         {
            LOG_INFO("Caching detail vertex noise: " + cachePath);
            outFile.write((char *)&mDetailNoiseBits,sizeof(int));
            outFile.write((char *)&mDetailNoise[0],
               sizeof(float)*mDetailNoiseSize*mDetailNoiseSize);
            outFile.close();
         }
      }
      else
      {
         LOG_INFO("Reading detail vertex noise from cache: " + cachePath);
               
         int detailBits;
         inFile.read((char *)&detailBits,sizeof(int));
         if (detailBits != mDetailNoiseBits)
         {
            LOG_INFO("Cache data mismatch in detail vertex noise.  Detail noise sizes "
               "are not equal.");
            mDetailNoise = NULL;
         }
         else
         {
            mDetailNoiseBits = detailBits;
            mDetailNoiseSize = 1 << mDetailNoiseBits;
            mDetailNoise = new float[mDetailNoiseSize*mDetailNoiseSize];
            inFile.read((char *)&mDetailNoise[0],sizeof(float) *
               mDetailNoiseSize*mDetailNoiseSize);   
         }
                     
         inFile.close();
      }                     
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void SoarXTerrainRenderer::CheckDetailScaleCache()
   {
      dtCore::RefPtr<osg::Image> scaleMapImage;
      
      if (GetParentTerrain()->GetCachePath().empty())
      {
         //Since we are not using a cache, just calculate the scale map.
         scaleMapImage = ImageUtils::CreateDetailScaleMap(mDetailNoiseSize,mDetailNoiseSize);
      }
      else
      {
         //See if the file exists in the cache, if not, create the scale map and cache it.
         std::string cachePath = GetParentTerrain()->GetCachePath() + "/" +
            SoarXCacheResourceName::DETAIL_SCALE_MAP.GetName();
         
         if (dtUtil::FileUtils::GetInstance().FileExists(cachePath))
         {
            LOG_INFO("Reading detail scale map from cache: " + cachePath);
            scaleMapImage = osgDB::readImageFile(cachePath);
            if (scaleMapImage == NULL)
            {
               LOG_INFO("Unable to load detail gradient from the cache.  Generating instead.");
               scaleMapImage = ImageUtils::CreateDetailScaleMap(mDetailNoiseSize,mDetailNoiseSize);
                  
               //Cache the new gradient texture..
               osgDB::writeImageFile(*scaleMapImage.get(),cachePath);
            }
            else
            {
               if (scaleMapImage->s() != mDetailNoiseSize || scaleMapImage->t() != mDetailNoiseSize)
               {
                  LOG_INFO("Cache data mismatch in detail scale map.  Detail noise sizes "
                     "are not equal.  Generating instead.");
                  scaleMapImage = ImageUtils::CreateDetailScaleMap(mDetailNoiseSize,mDetailNoiseSize);
                  
                  //Cache the new gradient texture..
                  osgDB::writeImageFile(*scaleMapImage.get(),cachePath);
               }
            }
         }
         else
         {
            LOG_INFO("Generating detail gradient texture.");
            scaleMapImage = ImageUtils::CreateDetailScaleMap(mDetailNoiseSize,mDetailNoiseSize);
                  
            //Cache the new gradient texture..
            osgDB::writeImageFile(*scaleMapImage.get(),cachePath);
         }
      }
      
       //Now that we have the image, create a texture map from it.
      mDetailScaleTexture = new osg::Texture2D();
      mDetailScaleTexture->setImage(scaleMapImage.get());
      mDetailScaleTexture->setFilter(osg::Texture2D::MIN_FILTER,
         osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      mDetailScaleTexture->setFilter(osg::Texture2D::MAG_FILTER,
         osg::Texture2D::LINEAR);
      mDetailScaleTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);
      mDetailScaleTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void SoarXTerrainRenderer::CheckDetailGradientCache()
   {
      dtCore::RefPtr<osg::Image> gradientImage = NULL;
      
      if (GetParentTerrain()->GetCachePath().empty())
      {
         //Since we are not using a cache, just calculate the gradient.
         gradientImage = ImageUtils::CreateDetailGradientMap(mDetailNoiseSize,mDetailNoiseSize,
            4.0f);         
      }
      else
      {
         //See if the file exists in the cache, if not, create the gradient image and cache it.
         std::string cachePath = GetParentTerrain()->GetCachePath() + "/" +
            SoarXCacheResourceName::DETAIL_GRADIENT_TEXTURE.GetName();
            
         if (dtUtil::FileUtils::GetInstance().FileExists(cachePath))
         {
            LOG_INFO("Reading detail gradient texture from cache: " + cachePath);
            gradientImage = osgDB::readImageFile(cachePath);
            if (gradientImage == NULL)
            {
               LOG_INFO("Unable to load detail gradient from the cache.  Generating instead.");
               gradientImage = ImageUtils::CreateDetailGradientMap(mDetailNoiseSize,mDetailNoiseSize,
                  4.0f);
                  
               //Cache the new gradient texture..
               osgDB::writeImageFile(*gradientImage.get(),cachePath);
            }
            else
            {
               if (gradientImage->s() != mDetailNoiseSize || gradientImage->t() != mDetailNoiseSize)
               {
                  LOG_INFO("Cache data mismatch in detail gradient texture.  Detail noise sizes "
                     "are not equal.  Generating instead.");
                  gradientImage = ImageUtils::CreateDetailGradientMap(mDetailNoiseSize,mDetailNoiseSize,
                     4.0f);
                  
                  //Cache the new gradient texture..
                  osgDB::writeImageFile(*gradientImage.get(),cachePath);
               }
            }
         }
         else
         {
            LOG_INFO("Generating detail gradient texture.");
            gradientImage = ImageUtils::CreateDetailGradientMap(mDetailNoiseSize,mDetailNoiseSize,
               4.0f);
                  
            //Cache the new gradient texture..
            osgDB::writeImageFile(*gradientImage.get(),cachePath);
         }
      }
     
      //Now that we have the image, create a texture map from it.
      mDetailGradientTexture = new osg::Texture2D();
      mDetailGradientTexture->setImage(gradientImage.get());
      mDetailGradientTexture->setFilter(osg::Texture2D::MIN_FILTER,
         osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      mDetailGradientTexture->setFilter(osg::Texture2D::MAG_FILTER,
         osg::Texture2D::LINEAR);
      mDetailGradientTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::REPEAT);
      mDetailGradientTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::REPEAT);
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void SoarXTerrainRenderer::CheckBaseGradientCache(const PagedTerrainTile &tile, 
      DrawableEntry &entry)
   {
      dtCore::RefPtr<osg::Image> baseGradientImage = NULL;
      float scale = 0.85f;
      
      if (tile.GetCachePath().empty())
      {
         //Not using the cache so just generate the data.
         baseGradientImage = ImageUtils::CreateBaseGradientMap(*tile.GetHeightField(),scale);  
      }
      else
      {
         //See if the file exists in the cache, if not, create the scale map and cache it.
         std::string cachePath = tile.GetCachePath() + "/" +
            SoarXCacheResourceName::BASE_GRADIENT_TEXTURE.GetName();
         
         if (dtUtil::FileUtils::GetInstance().FileExists(cachePath))
         {
            LOG_INFO("Reading base gradient from cache: " + cachePath);
            baseGradientImage = osgDB::readImageFile(cachePath);
            if (baseGradientImage == NULL)
            {
               LOG_INFO("Unable to load detail gradient from the cache.  Generating instead.");
               baseGradientImage = ImageUtils::CreateBaseGradientMap(*tile.GetHeightField(),
                  scale);  
                  
               //Cache the new gradient texture..
               osgDB::writeImageFile(*baseGradientImage.get(),cachePath);
            }
            else
            {
               int numCols = tile.GetHeightField()->GetNumColumns();
               int numRows = tile.GetHeightField()->GetNumRows();
               
               if (baseGradientImage->s() != (numCols-1) || baseGradientImage->t() != (numRows-1))
               {
                  LOG_INFO("Cache data mismatch in base gradiant map.  Dimensions are "
                     "invalid.  Generating instead.");
                  baseGradientImage = ImageUtils::CreateBaseGradientMap(*tile.GetHeightField(),
                     scale); 
                     
                  //Cache the new gradient texture..
                  osgDB::writeImageFile(*baseGradientImage.get(),cachePath);
               }
            }
         }
         else
         {
            LOG_INFO("Generating base gradient texture: " + cachePath);
            baseGradientImage = ImageUtils::CreateBaseGradientMap(*tile.GetHeightField(),
               scale);
                  
            //Cache the new gradient texture..
            osgDB::writeImageFile(*baseGradientImage.get(),cachePath);
         }
      }     
      
      entry.baseGradientTexture = new osg::Texture2D();
      entry.baseGradientTexture->setImage(baseGradientImage.get());
      entry.baseGradientTexture->setFilter(osg::Texture2D::MIN_FILTER,
         osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      entry.baseGradientTexture->setFilter(osg::Texture2D::MAG_FILTER,
         osg::Texture2D::LINEAR);
      entry.baseGradientTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
      entry.baseGradientTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void SoarXTerrainRenderer::CalculateDetailNoise()
   {
      //Make sure we allocate our detail noise buffer.
      mDetailNoise = new float[mDetailNoiseSize*mDetailNoiseSize];
      
      //Calculate for each "pixel" in our detail buffer, the noise value at
      //that location.
      for (int i=0; i<mDetailNoiseSize; i++)
      {
         for (int j=0; j<mDetailNoiseSize; j++)
         {
            SoarXDrawable::Index index(j,i);
            unsigned short value = ImageUtils::CalculateDetailNoise(j,i);
            index &= (mDetailNoiseSize-1);      
            mDetailNoise[index.y*mDetailNoiseSize+index.x] = 
               (value - 32768.0f) * mDetailVerticalResolution;
         }
      }     
   }


   void SoarXTerrainRenderer::SetEnableFog(bool pEnableFog)
   {  
      mRenderWithFog = pEnableFog;

      if(mUniformRenderWithFog.valid())
      {
         mUniformRenderWithFog->set(pEnableFog);
      }

   }
   
}
