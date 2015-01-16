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
 * Teague Coonan
 */

#include <dtUtil/fileutils.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtUtil/fileutils.h>
#include <dtTerrain/terraindatareader.h>
#include <dtTerrain/terraindatarenderer.h>
#include <dtTerrain/terrain.h>
#include <dtTerrain/vegetationdecorator.h>
#include <dtTerrain/lccanalyzer.h>
#include <osg/io_utils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/ImageOptions>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/TriStripVisitor>
#include <osg/Texture2D>
#include <dtTerrain/soarxterrainrenderer.h>
#include <dtTerrain/lcctype.h>
#include <sstream>
#include <cmath>

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   VegetationDecorator::VegetationDecorator(const std::string &name) : TerrainDecorationLayer(name)
   {
      mMaxLooks=4;
      mMaxVegetationPerCell = 2000;
      mVegetationNode = new osg::Group();
   }

   //////////////////////////////////////////////////////////////////////////
   VegetationDecorator::~VegetationDecorator()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void VegetationDecorator::OnLoadTerrainTile(PagedTerrainTile &tile)
   {
      if (mLCCTypes.empty())
      {
         throw dtTerrain::VegetationInvalidLCCTypesException("No LCC types have been specified "
            "for this decorator.  Therefore, no LCC vegetation placement can occur.", __FILE__, __LINE__);
      }

      //Make sure we clear out any precomputed data that may be tile
      //specific.
      mLCCAnalyzer.Clear();
      std::vector<dtTerrain::LCCType>::iterator itor;
      for (itor=mLCCTypes.begin(); itor!=mLCCTypes.end(); ++itor)
      {
         if (!mLCCAnalyzer.ProcessLCCData(tile,*itor))
            break;
      }
      mLCCAnalyzer.Clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void VegetationDecorator::OnUnloadTerrainTile(PagedTerrainTile &tile)
   {
      VegetationMap::iterator itor = mVegetationMap.find(&tile);
      if (itor != mVegetationMap.end())
      {
         mVegetationNode->removeChild(itor->second.get());
         mVegetationMap.erase(itor);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void VegetationDecorator::OnTerrainTileResident(PagedTerrainTile &tile)
   {
      dtCore::RefPtr<osg::Group> newVegetationGroup = AddVegetation(tile);

      mVegetationNode->addChild(newVegetationGroup.get());
      mVegetationMap.insert(std::make_pair(&tile,newVegetationGroup.get()));
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Group> VegetationDecorator::AddVegetation(PagedTerrainTile &tile)
   {
      //const int MAX_VEGE_GROUPS_PER_CELL = 16384;
      std::string sceneGraphFile = tile.GetCachePath() + "/" +
         LCCAnalyzerResourceName::SCENE_GRAPH.GetName();

      //Before we calculate the vegetation scene graph for this tile,
      //check the cache to see if we already have it and if so load it.
      if (dtUtil::FileUtils::GetInstance().FileExists(sceneGraphFile))
      {
         LOG_INFO("Loading cached vegetation scene from: " + sceneGraphFile);
         osg::Group *g = dynamic_cast<osg::Group *>(osgDB::readNodeFile(sceneGraphFile));
         if (g != NULL)
            return g;
      }

      LOG_INFO("Could not find existing data in the cache.  Generating instead...");
      dtCore::RefPtr<osg::Group> rootVegetationGroup = new osg::Group();

      //float deltaZ = -.75f;
      //float randomX = 0.0f, randomY = 0.0f, randomH = 0.0f, randomScale = 0.0f;
      //int scale = 4;

      //Set up random number seed
      if (mSeed != 0)
       srand(static_cast<unsigned>(mSeed));
      else
         srand(static_cast<unsigned>(time(NULL)));

      //Calculate the origin of this cell for vegetation placement.
      GeoCoordinates coords = tile.GetGeoCoordinates();
      osg::Vec3 cellOrigin = coords.GetCartesianPoint();

      //Get slope (aspect) image.
      std::string slopePath = tile.GetCachePath() + "/" +
            LCCAnalyzerResourceName::SLOPE_IMAGE.GetName() +
            LCCAnalyzerResourceName::IMAGE_EXT.GetName();

      //Load the cached slope image.  If this could not be found, return an
      //empty group.  This could happen if the vegetation lcc data was not
      //available for the current region.
      dtCore::RefPtr<osg::Image> slopeImage = osgDB::readImageFile(slopePath);
      if (!slopeImage.valid())
      {
         LOG_WARNING("Could not find slope map image.  Cannot generate vegetation.")
         return NULL;
      }

      //The total number of vegetation objects is capped per cell.  This may be adjusted
      //if performance is low.
      //@see SetMaxVegetationPerCell()
      mCellVegetationCount = 0;
      std::vector<LCCType>::iterator itor;
      for (itor=mLCCTypes.begin(); itor!=mLCCTypes.end(); ++itor)
      {
         std::ostringstream fileNameSS;
         fileNameSS << tile.GetCachePath() << "/" <<
            LCCAnalyzerResourceName::COMPOSITE_LCC_IMAGE.GetName() << itor->GetIndex() <<
               LCCAnalyzerResourceName::IMAGE_EXT.GetName();

         dtCore::RefPtr<osg::Image> compositeImage = osgDB::readImageFile(fileNameSS.str());
         if (!compositeImage.valid())
         {
            LOG_WARNING("Could not find composite LCC image: " + fileNameSS.str());
            LOG_WARNING("Unable to generate vegetation for LCC type: " + itor->GetLCCName());
            continue;
         }
         else
         {
            LOG_INFO("Loaded composite image for LCC type: " + itor->GetLCCName());
         }

         //Preload the models used for this LCC type.
         dtCore::RefPtr<osg::Group> newVegetation = NULL;
         if (itor->GetNumberOfModels() != 0)
         {
            newVegetation = BuildVegetationForType(*compositeImage.get(),
               *slopeImage.get(),cellOrigin,*itor);

            if (newVegetation != NULL)
            {
               //std::cout << "VEGE COUNT: " << mCellVegetationCount << std::endl;
               LOG_INFO("Spatially partitioning vegetation.");
               osgUtil::Optimizer optimizer;
               optimizer.optimize(newVegetation.get(),osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS
                  | osgUtil::Optimizer::SPATIALIZE_GROUPS);
               LOG_INFO("Done partitioning.");
               rootVegetationGroup->addChild(newVegetation.get());
            }
            else
               LOG_WARNING("Could not generate vegetation for LCC type: " + itor->GetLCCName());
         }
      }

      std::ostringstream ss;
      ss << "Total vegetation placed in this tile: " << mCellVegetationCount;
      LOG_INFO(ss.str());

      return rootVegetationGroup;
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Group> VegetationDecorator::BuildVegetationForType(
      osg::Image &compositeImage, osg::Image &slopeMap, osg::Vec3 &cellOrigin, LCCType &type)
   {
      dtCore::RefPtr<osg::Group> newGroup = new osg::Group();
      unsigned int i;

      LOG_INFO("Building vegetation for LCC: " + type.GetLCCName());

      for (i=0; i<type.GetNumberOfModels(); i++)
      {
         LCCType::LCCModel *currModel = type.GetModel(i);

         LOG_INFO("Loading model file: " + currModel->name);
         currModel->sceneNode = osgDB::readNodeFile(currModel->name);
         if (!currModel->sceneNode.valid())
         {
            LOG_WARNING("Could not load model file: " + currModel->name);
         }
         else
         {
            osg::StateSet *ss = currModel->sceneNode->getOrCreateStateSet();
            ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
         }
      }

      int x,y;
      for (y=0; y<compositeImage.t(); y++)
      {
         for (x=0; x<compositeImage.s(); x++)
         {
            int numLooks = GetNumLooks(compositeImage,slopeMap,x,y,type.GetAspect(),
               mMaxLooks,type.GetMaxSlope());

            //int scaledx = int(x/scale);
            //int scaledy = int(y/scale);
            //vg = int(scaledx/8)+int(scaledy/8)*128;

            for (int currLook=0; currLook<numLooks; currLook++)
            {
               if (mCellVegetationCount > mMaxVegetationPerCell)
                  break;

               float randomX, randomY, randomScale;
               float randomHeading;

               //Deterministic version - good for debugging
               //if (GetVegetation(compositeImage,x,y,probabilitylimit))

               //Stochastic version - better realism
               int probability = (int)(255.0f*(float)rand()/RAND_MAX+1.0f);
               if (!GetVegetation(compositeImage,x,y,probability))
                  continue;

               dtCore::RefPtr<osg::PositionAttitudeTransform> xForm =
                  new osg::PositionAttitudeTransform();

               randomX = (float)rand()/(RAND_MAX+1.0f)-0.5f;     // -.5 to .5
               randomY = (float)rand()/(RAND_MAX+1.0f)-0.5f;     // -.5 to .5
               randomHeading = (float)rand()/(RAND_MAX+1.0f);          // 0 to 1.0
               randomScale = (float)rand()/(RAND_MAX+1.0f)*0.5f; // 0 to .5

               //limit orientation of urban models
               if (type.GetIndex() < 30)
                  randomHeading = floorf(randomHeading*4.0f)/4.0f;

               osg::Quat orientation;
               orientation.makeRotate(osg::PI*randomHeading, osg::Vec3(0,0,1));
               xForm->setAttitude(orientation);

               //float tempx = cellorigin[0]+(x+.5+random_x*0.95f)*108.7/scale;
               //float tempy = cellorigin[1]+(y+.5+random_y*0.95f)*108.7/scale;

               // Position of the vegetation
               osg::Vec3 vegetationPosition;
               float xPos,yPos,height;

               xPos = cellOrigin.x() + ((float)x+randomX+0.95f)*108.5f;
               yPos = cellOrigin.y() + ((float)y+randomY+0.95f)*108.5f;
               height = GetParentTerrain()->GetHeight(xPos,yPos);
               vegetationPosition.set(xPos,yPos,height);
               xForm->setPosition(vegetationPosition);

               //randomly select the models to use for this lcc type
               int whichModel = int(rand()/(RAND_MAX+1.0f)*type.GetNumberOfModels());

               if (type.GetIndex() > 30)
               {
                  float modelScale = type.GetModel(whichModel)->scale;
                  osg::Vec3 vegetationScale;

                  vegetationScale.x() = 1.5f*modelScale+randomScale;
                  vegetationScale.y() = 1.5f*modelScale+randomScale;
                  vegetationScale.z() = modelScale+randomScale;
                  xForm->setScale(vegetationScale);
               }

               //Add the Randomly selected model for this vegetation type
               xForm->addChild(type.GetModel(whichModel)->sceneNode.get());
               newGroup->addChild(xForm.get());
               mCellVegetationCount++;
            }
         }
      }

      return newGroup;
   }

   //////////////////////////////////////////////////////////////////////////
   bool VegetationDecorator::GetVegetation(osg::Image &compositeImage, int x, int y, int limit)
   {
      unsigned char *cData = (unsigned char*)compositeImage.data(x,y);
      return (cData[1] <= limit);
   }

   //////////////////////////////////////////////////////////////////////////
   int VegetationDecorator::GetVegType(const osg::Image* mCimage, int x, int y, float goodAngle)
   {
      unsigned char* c_data = NULL;
      float random_draw;
      int bin;

      if (mCimage!=NULL) c_data = (unsigned char*)mCimage->data(x,y);
      else bin = 1;

      float my_angle = (c_data[0]/360.0f)*255.0f;
      float diff = std::abs(goodAngle - my_angle);

      if ((diff <= 45) || (diff >= 315))
         bin = 2;            // biased towards older vegetation
      else if (diff >= 135)
         bin = 0;            // biased toward younger vegetation
      else
         bin = 1;            // no bias

      random_draw = (float)rand()/(RAND_MAX+1.0f);

      switch (bin)
      {
      case 0:
         if (random_draw < .50)
            return 0;
         else if (random_draw < .80)
            return 1;
         else
            return 2;
         break;

      case 1:
         if (random_draw < .33)
            return 0;
         else if (random_draw < .66)
            return 1;
         else
            return 2;
         break;

      case 2:
         if (random_draw < .50)
            return 2;
         else if (random_draw < .80)
            return 1;
         else
            return 0;
         break;
      }

      return 1;
   }

   //////////////////////////////////////////////////////////////////////////
   int VegetationDecorator::GetNumLooks(const osg::Image &compositeImage,
      const osg::Image &slopeMap, int x, int y, float goodAngle, int maxlooks, float maxslope)
   {
      unsigned char *c_data = NULL;

      c_data = (unsigned char*)compositeImage.data(x,y);
      if ((c_data[0]==255)&&(c_data[1]==255)&&(c_data[2]==255))
         return 0;

      unsigned char *s_data = NULL;
      int bin = -1;

      s_data = (unsigned char*)slopeMap.data(x,y);
      float my_angle = (s_data[2]/255.0f)*360.0f;
      float diff = std::abs(goodAngle - my_angle);
      float slope = s_data[1];
      maxslope = (maxslope/90.0f) * 255.0f;

      if (slope > maxslope/2.0f)
      {
         if ((diff <= 45) || (diff >= 315))
            bin = 2;            // biased towards older vegetation
         else if (diff >= 135)
            bin = 0;            // biased toward younger vegetation
         else
            bin = 1;            // no bias
      }
      else
      {
         bin = 1;
      }

      switch (bin)
      {
      case 0:                  // bad aspect angle
         return (int)ceil(maxlooks/2.0f);
         break;
      case 1:                  // neutral angle
         return (int)ceil(maxlooks/1.5f);
         break;
      case 2:                  // good angle
         return maxlooks;
         break;
      }
      return 1;
   }

   ////////////////////////////////////////////////////////////////////////////////
   VegetationInvalidLCCTypesException::VegetationInvalidLCCTypesException(const std::string& message, const std::string& filename, unsigned int linenum) : dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   VegetationInvalidSlopeAspectImageException::VegetationInvalidSlopeAspectImageException(const std::string& message, const std::string& filename, unsigned int linenum) : dtUtil::Exception(message, filename, linenum)
   {
   }
}
