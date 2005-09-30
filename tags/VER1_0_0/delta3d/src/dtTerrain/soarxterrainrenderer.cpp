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

#include <dtCore/scene.h>

#include <osg/Group>
#include <osg/Shape>
#include <osg/Uniform>
#include <osg/Program>
#include <osg/StateSet>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "dtTerrain/terraindatareader.h"
#include "dtTerrain/terraindecorationlayer.h"
#include "dtTerrain/terrain.h"
#include "dtTerrain/soarxterrainrenderer.h"
#include "dtTerrain/imageutils.h"
#include "dtTerrain/soarxdrawable.h"

namespace dtTerrain
{
   const float SoarXTerrainRenderer::GRADIENT_SCALE = 128.0f;

   //////////////////////////////////////////////////////////////////////////    
   SoarXTerrainRenderer::SoarXTerrainRenderer(const std::string &name) :
      TerrainDataRenderer(name)
   {    
      mFragShaderPath = "shaders/soarxterrain.frag";
   }   
   
   //////////////////////////////////////////////////////////////////////////    
   SoarXTerrainRenderer::~SoarXTerrainRenderer()
   {
      
   }
   
   //////////////////////////////////////////////////////////////////////////    
   void SoarXTerrainRenderer::Initialize()
   {
      if (GetHeightField() == NULL)
         EXCEPT(TerrainRendererException::INVALID_HEIGHTFIELD_DATA,
            "Cannot initialize SoarX renderer.  HeightField is NULL.");
            
      //Before we resize the heightfield, convert the orginial to an image.
      mHeightMapImage = ImageUtils::MakeHeightMapImage(GetHeightField());
      SetHeightField(ResizeHeightField(GetHeightField()));
      
      int baseSize = GetHeightField()->getNumColumns()-1;
      float gridSpacing = SEMI_MAJOR_AXIS * osg::DegreesToRadians(1.0f);
      float horizRes;
      int baseBits;
      
      baseBits = (int)(logf(baseSize) / logf(2.0f));
      horizRes = gridSpacing / static_cast<float>(baseSize-1);
      GetHeightField()->setXInterval(horizRes);
      GetHeightField()->setYInterval(horizRes);
      
      mDrawable = new SoarXDrawable(baseBits,horizRes);
      mDrawable->Build(GetHeightField());        
      
      //Build the actual scene nodes used to represent the terrain.
      mRootGroupNode = new osg::Group();
      osg::Geode *geode = new osg::Geode();
      geode->addDrawable(mDrawable.get());
      
      //Build all the texture maps we need, caching them when 
      //it makes sense to do so.
      BuildTextureImages(geode);
      CalculateAutoTexCoordParams(geode);            
      
      //If we have fragment shader capable hardware, create a fragment shader
      //used to render the terrain.
      CreateFragmentShader(geode);
      
      //Finally, add the node to the group.      
      mRootGroupNode->addChild(geode);
   }
         
   //////////////////////////////////////////////////////////////////////////    
   float SoarXTerrainRenderer::GetHeight(float x, float y)
   {
      if (!mDrawable)
      {
         LOG_ERROR("Cannot retreive terrain height.  The terrain is not valid.");
         return 0.0f;
      }
      else
         return mDrawable->GetHeight(x,y);  
   }
         
   //////////////////////////////////////////////////////////////////////////    
   osg::Vec3 SoarXTerrainRenderer::GetNormal(float x, float y)
   {
      if (!mDrawable)
      {
         LOG_ERROR("Cannot retreive terrain normal.  The terrain is not valid.");
         return osg::Vec3(0,0,1);
      }
      
      float z = mDrawable->GetHeight(x,y);
      osg::Vec3 v1,v2,normal;
      
      v1 = osg::Vec3(0.1f,0.0f,mDrawable->GetHeight(x+0.1f,y) - z);
      v2 = osg::Vec3(0.0f,0.1f,mDrawable->GetHeight(x,y+0.1f) - z);
      
      normal = v1 ^ v2;
      normal.normalize();
      return normal;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void SoarXTerrainRenderer::SetDetailMultiplier(float value)
   {
      if (mDrawable.valid())
         mDrawable->SetDetailMultiplier(value);
   }

   //////////////////////////////////////////////////////////////////////////   
   float SoarXTerrainRenderer::GetDetailMultiplier() const
   {
      if (mDrawable.valid())
         return mDrawable->GetDetailMultiplier();
      else
         return 0.0f;
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   void SoarXTerrainRenderer::SetThreshold(float value)
   {
      if (mDrawable.valid())
         mDrawable->SetThreshold(value);
   }
   
   ////////////////////////////////////////////////////////////////////////// 
   float SoarXTerrainRenderer::GetThreshold() const 
   {
      if (mDrawable.valid())
         return mDrawable->GetThreshold();
      else
         return 0.0f;
   }
   
   //////////////////////////////////////////////////////////////////////////       
   osg::HeightField *SoarXTerrainRenderer::ResizeHeightField(osg::HeightField *oldHF)
   {
      unsigned int width = oldHF->getNumColumns();
      unsigned int height = oldHF->getNumRows();
      unsigned int dims;
      int correctSize;

      //We need to make sure that the dimensions of the heightmap are in fact
      //(2^n)+1 x (2^n)+1.  If they are not, we need to resize it.
      dims = osg::maximum(width,height);
      correctSize = osg::Image::computeNearestPowerOfTwo(dims) + 1;      
      if (width == (unsigned)correctSize && width == height)
         return oldHF;
      
      //Log some good information.
      std::ostringstream ss;
      ss << "Heightfield dimensions (" << width << "," << height << 
         ") are not (2^n)+1 x (2^n)+1.  Resizing...";
      LOG_WARNING(ss.str());
      
      osg::HeightField *newHF = new osg::HeightField();
      double x,xStep;
      double yStep;
      double y = 0.0;

      newHF->allocate(correctSize,correctSize);
      xStep = (width-1.0) / (correctSize-1.0);
      yStep = (height-1.0) / (correctSize-1.0);
      for (int i=0; i<correctSize; i++)
      {
         x = 0.0;
         for (int j=0; j<correctSize; j++)
         {
            float newValue = GetInterpolatedHeight(oldHF,x,y);
            newHF->setHeight(j,i,newValue);
            x += xStep;
         }
         
         y += yStep;
      }     
      
      return newHF;
   }
   
   //////////////////////////////////////////////////////////////////////////
   void SoarXTerrainRenderer::BuildTextureImages(osg::Geode *geode)
   {
      if (geode == NULL)
      {
         //todo throw exception...
         LOG_ERROR("Cannot attach textures to an invalid geode.");
         return;
      }
      
      osg::StateSet *ss = geode->getOrCreateStateSet();
      
      //Create the detail gradient image and load it into a texture map.
      dtCore::RefPtr<osg::Image> detailGradientMap =
         osgDB::readImageFile(GetParentTerrain()->GetCachePath() + "/detail_gradient.jpg");
      if (detailGradientMap == NULL)
      {
         LOG_INFO("Building detail gradient map.");
         detailGradientMap = 
            ImageUtils::CreateDetailGradientMap(mHeightMapImage->s(),mHeightMapImage->t(),4.0f);
         osgDB::writeImageFile(*detailGradientMap,GetParentTerrain()->GetCachePath() + 
            "/detail_gradient.jpg");
      }
      else
      {
         LOG_INFO("Using cached version of the detail gradient map.");
      }
      
      mDetailGradientTexture = new osg::Texture2D();
      mDetailGradientTexture->setImage(detailGradientMap.get());
      mDetailGradientTexture->setFilter(osg::Texture2D::MIN_FILTER,
         osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      mDetailGradientTexture->setFilter(osg::Texture2D::MAG_FILTER,
         osg::Texture2D::LINEAR);
      mDetailGradientTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);
      mDetailGradientTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);
      ss->setTextureAttributeAndModes(0,mDetailGradientTexture.get());
      
      //Create the detail scale image and load it into a texture map.
      LOG_INFO("Building detail scale map.");
      dtCore::RefPtr<osg::Image> detailScaleMap =
         ImageUtils::CreateDetailScaleMap(mHeightMapImage->s(),mHeightMapImage->t());
         
      mDetailScaleTexture = new osg::Texture2D();
      mDetailScaleTexture->setImage(detailScaleMap.get());
      mDetailScaleTexture->setFilter(osg::Texture2D::MIN_FILTER,
         osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      mDetailScaleTexture->setFilter(osg::Texture2D::MAG_FILTER,
         osg::Texture2D::LINEAR);
      mDetailScaleTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);
      mDetailScaleTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);     
      ss->setTextureAttributeAndModes(1,mDetailScaleTexture.get());
                 
      //Create the base gradient image and load it into a texture map.
      //Create the detail gradient image and load it into a texture map.
      LOG_INFO("Building base gradient map.");
      dtCore::RefPtr<osg::Image> baseGradientMap;
      baseGradientMap = ImageUtils::CreateBaseGradientMap(mHeightMapImage.get(),GRADIENT_SCALE * 
         mDrawable->GetBaseVerticalResolution() / mDrawable->GetBaseHorizontalResolution()); 
               
      mBaseGradientTexture = new osg::Texture2D();
      mBaseGradientTexture->setImage(baseGradientMap.get());
      mBaseGradientTexture->setFilter(osg::Texture2D::MIN_FILTER,
         osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      mBaseGradientTexture->setFilter(osg::Texture2D::MAG_FILTER,
         osg::Texture2D::LINEAR);
      mBaseGradientTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
      mBaseGradientTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
      ss->setTextureAttributeAndModes(2,mBaseGradientTexture.get());
      
      //The currentl SoarX implementation only uses the first entry in the
      //custom texture lists. (If available).  This is used as a base image map
      //for the terrain.  It could be an LCC image, a colormap, or a satellite image.
      if (!mCustomImageList.empty())
      {
         osg::Image *image = mCustomImageList[0].get();
         if (image == NULL)
            return;
         
         osg::Texture2D *baseColorTexture = new osg::Texture2D();
         baseColorTexture->setImage(image);
         baseColorTexture->setFilter(osg::Texture2D::MIN_FILTER,
            osg::Texture2D::LINEAR_MIPMAP_LINEAR);
         baseColorTexture->setFilter(osg::Texture2D::MAG_FILTER,
            osg::Texture2D::LINEAR);
         baseColorTexture->setWrap(osg::Texture::WRAP_S,osg::Texture::MIRROR);
         baseColorTexture->setWrap(osg::Texture::WRAP_T,osg::Texture::MIRROR);
         ss->setTextureAttributeAndModes(3,baseColorTexture);
      }
   }
   
   //////////////////////////////////////////////////////////////////////////
   void SoarXTerrainRenderer::CreateFragmentShader(osg::Geode *geode)
   {
      if (geode == NULL)
      {
         LOG_ERROR("Cannot create fragment shader.  A NULL geode was specified.");
         return;
      }
      
      dtCore::RefPtr<osg::Program> shaderProgram = new osg::Program();
      dtCore::RefPtr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);
      osg::StateSet *ss = geode->getOrCreateStateSet();
      
      std::string shaderPath = osgDB::findDataFile(mFragShaderPath);
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
           
      shaderProgram->addShader(fragShader.get()); 
      
      //This following code connects our textures to samplers so the 
      //fragment shader can access them.
      osg::Uniform *uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,"detailGradient");
      uniform->set(0);
      ss->addUniform(uniform);
      
      uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,"detailScale");
      uniform->set(1);
      ss->addUniform(uniform);

      uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,"baseGradient");
      uniform->set(2);
      ss->addUniform(uniform);
      
      uniform = new osg::Uniform(osg::Uniform::SAMPLER_2D,"baseColor");
      uniform->set(3);
      ss->addUniform(uniform);
      
      //Finally add the shader program to our current state set.
      ss->setAttributeAndModes(shaderProgram.get());      
   }
   
   //////////////////////////////////////////////////////////////////////////   
   void SoarXTerrainRenderer::CalculateAutoTexCoordParams(osg::Geode *geode)
   {
      if (geode == NULL)
      {
         LOG_ERROR("Cannot calculate texture coordinates.  Geode was NULL.");
         return;
      }
      
      osg::StateSet *ss = geode->getOrCreateStateSet();
      osg::TexGen *texGen = new osg::TexGen();
      float dt;
      
      //Calculate the planer mapping parameters for the detail gradient texture.
      dt = 1.0f / (mDrawable->GetDetailHorizontalResolution() * mDrawable->GetDetailSize());
      texGen->setMode(osg::TexGen::OBJECT_LINEAR);
      texGen->setPlane(osg::TexGen::S,osg::Plane(dt,0,0,0));
      texGen->setPlane(osg::TexGen::T,osg::Plane(0,dt,0,0));
      ss->setTextureAttributeAndModes(0,texGen);
      
      //Calculate the planer mapping parameters for the detail scale texture.
      texGen = new osg::TexGen();
      dt = 1.0f / (mHeightMapImage->s()*mDrawable->GetBaseHorizontalResolution());
      texGen->setMode(osg::TexGen::OBJECT_LINEAR);
      texGen->setPlane(osg::TexGen::S,osg::Plane(dt,0,0,0));
      texGen->setPlane(osg::TexGen::T,osg::Plane(0,dt,0,0));
      ss->setTextureAttributeAndModes(1,texGen);
   }

   //////////////////////////////////////////////////////////////////////////   
   float SoarXTerrainRenderer::GetInterpolatedHeight(const osg::HeightField *hf, 
      double x, double y)
   {
      int fx = (int)floor(x), fy = (int)floor(y);
      int cx = (int)ceil(x), cy = (int)ceil(y);
      
      double v1 = hf->getHeight(fx,fy);
      double v2 = hf->getHeight(cx,fy);
      double v3 = hf->getHeight(fx,cy);
      double v4 = hf->getHeight(cx,cy);
      
      double v12 = v1 + (v2-v1)*(x-fx);
      double v34 = v3 + (v4-v3)*(x-fx);
      
      return v12 + (v34-v12)*(y-fy);
   }
   
}
