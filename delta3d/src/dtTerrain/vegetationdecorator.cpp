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
* @author Teague Coonan
*/
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include "dtTerrain/terraindatareader.h"
#include "dtTerrain/terraindatarenderer.h"
#include "dtTerrain/terrain.h"
#include "dtTerrain/terraintile.h"
#include "dtTerrain/vegetationdecorator.h"
#include "dtTerrain/lccanalyzer.h"
#include <osg/io_utils>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/ImageOptions>
#include <osg/PositionAttitudeTransform>
#include <osgUtil/TriStripVisitor>
#include "dtTerrain/soarxterrainrenderer.h"
#include "dtTerrain/lcctype.h"

namespace dtTerrain
{
   //////////////////////////////////////////////////////////////////////////
   VegetationDecorator::VegetationDecorator(const std::string &name) : TerrainDecorationLayer(name)
   {
      SetVegeDistance(2.0);
      mSeed = 27;
      mLoadDistance = 10000.0f;
      mDetailMultiplier=2;
      mMaxTextureSize=1024;
      mMaxLooks=1;
      mMaxObjectsPerCell=5000000;
      mSemiMajorAxis = 6378137.0;
      mImageExtension = ".jpg";
      mTotalVegeCount=0;
   }

   //////////////////////////////////////////////////////////////////////////
   VegetationDecorator::~VegetationDecorator()
   {
   }

   //////////////////////////////////////////////////////////////////////////    
   void VegetationDecorator::LoadResource(int latitude, int longitude)
   {
      // Load the vegetation and store its group node
      AddVegetation(latitude, longitude);
   }

   //////////////////////////////////////////////////////////////////////////    
   void VegetationDecorator::AddVegetation(int latitude, int longitude)
   {
      mLog = &dtUtil::Log::GetInstance();
      TerrainTile *tile = new TerrainTile;

      // This sets the current tiles cell name which is required for image editing
      char cellName[64];
      sprintf(cellName,"lat.%d_lon.%d",latitude,longitude);


      // Set the tiles latitude and longitude specified by the user. 
      tile->setLatitude(latitude);
      tile->setLongitude(longitude);

      // Instantiate the LCCAnalyzer object to process our vegetation data
      LCCAnalyzer *LCCData = new LCCAnalyzer;

      std::map<TerrainTile, LCCAnalyzer::LCCCells> segmentLCCCellMap;

      float vegedistance = mVegeDistance;
      LCCAnalyzer::LCCCells lcccell;

      // Set the cache path
      mCachePath = GetParentTerrain()->GetCachePath();

      // Grab the heightfield from the parent terrains data renderer
      osg::HeightField* hf = GetParentTerrain()->GetDataReader()->GetHeightField();
      
      // Load the LCC data from the lccanalyzer
      LCCData->SetLCCData(mLCCs);
      LCCData->LoadLCCData(hf,latitude,longitude,mMaxTextureSize, cellName);

      // This should come from the TerrainTile
      
      lcccell.mRootVegeGroup = new osg::Group();		
      lcccell.mRootVegeGroup->setName("mRootVegeGroup");
      lcccell.mRootVegeGroup->setDataVariance(osg::Object::STATIC);

      //create quadtree
      char myname[20];
      int i;
      dtCore::RefPtr<osg::Group> QuadGroup2[4];
      for (i = 0; i < 4; i++)
      {
         sprintf(myname, "QuadGroup2[%i]",i);
         QuadGroup2[i] = new osg::Group;
         QuadGroup2[i]->setName(myname);
         QuadGroup2[i]->dirtyBound();
         lcccell.mRootVegeGroup.get()->addChild(QuadGroup2[i].get());
      }
      dtCore::RefPtr<osg::Group>  QuadGroup4[16];
      for (i = 0; i < 16; i++)
      {
         sprintf(myname, "QuadGroup4[%i]",i);
         QuadGroup4[i] = new osg::Group;
         QuadGroup4[i]->setName(myname);
         QuadGroup4[i]->dirtyBound();
         QuadGroup2[i/4]->addChild(QuadGroup4[i].get());
      }
      dtCore::RefPtr<osg::Group>  QuadGroup8[64];
      for (i = 0; i < 64; i++)
      {
         sprintf(myname, "QuadGroup8[%i]",i);
         QuadGroup8[i] = new osg::Group;
         QuadGroup8[i]->setName(myname);
         QuadGroup8[i]->dirtyBound();
         QuadGroup4[i/4]->addChild(QuadGroup8[i].get());
      }
      dtCore::RefPtr<osg::Group>  QuadGroup16[256];
      for (i = 0; i < 256; i++)
      {
         sprintf(myname, "QuadGroup16[%i]",i);
         QuadGroup16[i] = new osg::Group;
         QuadGroup16[i]->setName(myname);
         QuadGroup16[i]->dirtyBound();
         QuadGroup8[i/4]->addChild(QuadGroup16[i].get());
      }
      dtCore::RefPtr<osg::Group>  QuadGroup32[1024];
      for (i = 0; i < 1024; i++)
      {
         sprintf(myname, "QuadGroup32[%i]",i);
         QuadGroup32[i] = new osg::Group;
         QuadGroup32[i]->setName(myname);
         QuadGroup32[i]->dirtyBound();
         QuadGroup16[i/4]->addChild(QuadGroup32[i].get());
      }

      dtCore::RefPtr<osg::Group>  QuadGroup64[4096];
      for (i = 0; i < 4096; i++)
      {
         sprintf(myname, "QuadGroup64[%i]",i);
         QuadGroup64[i] = new osg::Group;
         QuadGroup64[i]->setName(myname);
         QuadGroup64[i]->dirtyBound();
         QuadGroup32[i/4]->addChild(QuadGroup64[i].get());
      }

      // constants
      const int MAX_VEGE_PER_CELL = mMaxObjectsPerCell;
      const int MAX_VEGE_GROUPS_PER_CELL = 163840;
      
      // do once variables
      float deltaz = -.75f;
      float random_x = 0.0f, random_y=0.0f, random_h=0.0f, random_scale=0.0f;
      
      int scale = int(mMaxTextureSize/1024);

      //set up random number seed
      if (mSeed!= 0)
      {
         srand(static_cast<unsigned>(mSeed));
      }
      else
      {
         srand(static_cast<unsigned>(time(NULL)));
      }

      //find cell origin
      mOriginLatitude = (int)floorf(GetParentTerrain()->GetOrigin().x());
      mOriginLongitude = (int)floorf(GetParentTerrain()->GetOrigin().y());
      mOriginElevation = (int)GetParentTerrain()->GetOrigin().z();

      osg::Vec3 cellorigin(
         (longitude - mOriginLongitude)*osg::DegreesToRadians(1.0f)*mSemiMajorAxis,
         (latitude - mOriginLatitude)*osg::DegreesToRadians(1.0f)*mSemiMajorAxis,
         -mOriginElevation
         );

      //create LeafGroups (these hold a bunch of objects in the smallest atomic unit)
      dtCore::RefPtr<osg::LOD> LeafGroup[MAX_VEGE_GROUPS_PER_CELL];

      int vg;
      for (vg = 0; vg < MAX_VEGE_GROUPS_PER_CELL; vg++)
      {
         sprintf(myname, "LeafGroup[%i]",vg);
         LeafGroup[vg] = new osg::LOD();
         LeafGroup[vg]->setName(myname);
         LeafGroup[vg]->setDataVariance(osg::Object::STATIC);
      }

      // get slope (aspect) image info
      std::string SLimagePath = mCachePath + "/" + cellName + ".sl.image" + mImageExtension;

      dtCore::RefPtr<osg::Image> SLimage = osgDB::readImageFile(SLimagePath);


      for(std::vector<LCCType>::reverse_iterator l = mLCCs.rbegin();		//cycle through all the LCC types
         l != mLCCs.rend();
         l++)
      {
         // Make sure we have models to place for this vegetation type
         if ((*l).GetModelNum() > 0)
         {
            int vegecount = 0;				

            // pre load our models from all LCC types contained in mLCCs
            for(i=0; i < (*l).GetModelNum(); ++i)
            {
               mLog->LogMessage(dtUtil::Log::LOG_DEBUG,  __FUNCTION__, "Loading %s", (*l).GetModelName(i).c_str());
                              
               // Make sure we can load a model before try and add it's node
               osg::Node *vegeNode = osgDB::readNodeFile((*l).GetModelName(i));
               if (vegeNode != NULL)
               {
                  osg::StateSet *ss = vegeNode->getOrCreateStateSet();
                  ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
                  ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
                  vegeNode->setDataVariance(osg::Object::STATIC);
                  (*l).SetVegetationObject(dynamic_cast<osg::Group*>(vegeNode),i);
               }
               else
               {
                  mLog->LogMessage(dtUtil::Log::LOG_ERROR,__FUNCTION__,"Could not load the model.");
               }                  
            }

            mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Placing LCCtype %i '%s'....",(*l).GetIndex(), (*l).GetLCCName().c_str());

            // identify which probability map we'll be using
            char idxnum[3];
            sprintf(idxnum, "%i",(*l).GetIndex());

            std::string mCimagePath = mCachePath + "/" + cellName + ".c.image." + idxnum + mImageExtension;

            //set image options (to load up correct sized image)
            dtCore::RefPtr<osgDB::ImageOptions> options = new osgDB::ImageOptions;

            //options->setObjectCacheHint(osgDB::ImageOptions::CACHE_ALL);
            options->_destinationImageWindowMode = osgDB::ImageOptions::PIXEL_WINDOW;
            options->_destinationPixelWindow.set(0,0,mMaxTextureSize,mMaxTextureSize);
            osgDB::Registry::instance()->setOptions(options.get());

            //load up the probability map
            dtCore::RefPtr<osg::Image> mCimage = osgDB::readImageFile(mCimagePath);

            for (int y = 0; y < mMaxTextureSize; y++)
            {
               for (int x = 0; x < mMaxTextureSize; x++)
               {
                  int numLooks = GetNumLooks(mCimage.get(), SLimage.get(), x, y, (*l).GetAspect(), mMaxLooks, (*l).GetMaxSlope());

                  if (numLooks)
                  {
                     int scaledx = int(x/scale);
                     int scaledy = int(y/scale);
                     vg = int(scaledx/8)+int(scaledy/8)*128;

                     for (int ml = 0; ml <numLooks; ml++)
                     {
                        // deterministic version - good for debugging
                        //if (GetVegetation(mCimage.get(), x,y,probabilitylimit) && (vegecount < MAX_VEGE_PER_CELL))

                        // stochastic version - better realism
                        if (GetVegetation(mCimage.get(), x, y,(int)(255.0f*(float)rand()/(RAND_MAX+1.0f))) && (vegecount < MAX_VEGE_PER_CELL))
                        {
                           // Initialize tranform to be used for positioning the plant
                           // assign values for the transform
                           dtCore::RefPtr<osg::PositionAttitudeTransform> vegeXform = new osg::PositionAttitudeTransform();

                           random_x = (float)rand()/(RAND_MAX+1.0f)-0.5f;		// -.5 to .5
                           random_y = (float)rand()/(RAND_MAX+1.0f)-0.5f;		// -.5 to .5
                           random_h = (float)rand()/(RAND_MAX+1.0f);			   // 0 to 1.0
                           random_scale = (float)rand()/(RAND_MAX+1.0f)*0.5f; // 0 to .5
                           
                           //limit orientation of urban models
                           if ((*l).GetIndex()<30)   
                           {
                              random_h = floor(random_h*4.0f)/4.0f;  //0,.25,.50,.75 = 0, 90, 180, 270 
                              deltaz = -.1f;
                           }

                           osg::Quat attitude;
                           attitude.makeRotate(6.28f*random_h, osg::Vec3(0, 0, 1));
                           vegeXform->setAttitude(attitude);

                           float tempx = cellorigin[0]+(x+.5+random_x*0.95f)*108.7/scale; 
                           float tempy = cellorigin[1]+(y+.5+random_y*0.95f)*108.7/scale;

                           // Position of the vegetation
                           osg::Vec3 vegePosit;

                           // Grab the height of our terrain at the vegetation point in question
                           float height = this->GetParentTerrain()->GetHeight(tempx, tempy);
                           vegePosit.set(tempx,tempy,height+deltaz);
                         
                           //randomly select the models to use for this lcc type
                           int whichmodel = int(rand()/(RAND_MAX+1.0f)*(*l).GetModelNum());
                           
                           if ((*l).GetIndex()>30)     //don't scale urban models
                           {
                              // using our random model index for this type create a random scale
                              float modelscale = (*l).GetModelScale(whichmodel);
                              osg::Vec3 vegeScale(1.5*modelscale+random_scale,1.5*modelscale+random_scale,modelscale+random_scale);
                              vegeXform->setScale(vegeScale);
                           }

                           // Set the position 
                           vegeXform->setPosition(vegePosit);

                           // Add the Randomly selected model for this vegetation type
                           vegeXform->addChild((*l).GetVegetationObject(whichmodel).get());

                           //check collision detection
                           bool goodposition = true;
                           
                           // for urban objects
                           if ((*l).GetIndex()<30)		
                           {
                              osg::BoundingSphere vegeXformBS = vegeXform->getBound();
                              unsigned int nc = 0;
                              while( (goodposition) && (nc < LeafGroup[vg]->getNumChildren()) )
                              {
                                 goodposition = !(vegeXformBS.intersects(LeafGroup[vg]->getChild(nc)->getBound()));
                                 nc++;
                              }
                           }
                           else					
                              // for vegetation objects
                           {
                              osg::BoundingSphere vegeXformBS = vegeXform->getBound();
                              unsigned int nc = 0;
                              while( (goodposition) && (nc < LeafGroup[vg]->getNumChildren()) )
                              {
                                 osg::BoundingSphere childBS = LeafGroup[vg]->getChild(nc)->getBound();
                                 if ((vegeXformBS._center - childBS._center).length() < (vegeXformBS._radius/4.0f))
                                    goodposition = FALSE;
                                 nc++;
                              }
                           }

                           if (goodposition)
                           {
                              LeafGroup[vg]->addChild(vegeXform.get(),0,vegedistance);
                              LeafGroup[vg]->setRange((LeafGroup[vg]->getNumChildren())-1, 0.0f,15000.0f);
                              vegecount++;
                           }
                           else
                           {
                              vegeXform->removeChild(vegeXform->getNumChildren());
                           }
                        }//getvegetation
                     }//ml
                  }
               }//x
            }//y
            
            mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "mCimage count = %i", mCimage->referenceCount());
            
            mCimage.release();
            mCimage.release();
            mCimage.~RefPtr();
            mCimage = NULL;

            mTotalVegeCount = mTotalVegeCount + vegecount;
            mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Total count = %i", mTotalVegeCount);
         }
      }

      //stats counters
      int groupcount = 0;
      int maxchildren = 0;
      int minchildren = 999;
      int vegechild = 0;			
      int totalcount = 0;

      //only add in groups that are non-empty
      for (int vg = 0; vg < MAX_VEGE_GROUPS_PER_CELL; ++vg)
      {
         vegechild = LeafGroup[vg]->getNumChildren();

         if (vegechild != 0)
         {
            if (vegechild > maxchildren)	maxchildren = vegechild;
            if (vegechild < minchildren)	minchildren = vegechild;

            int whichgroup = 0;
            int row = int(vg/256);
            int toprow = (vg-int(vg/128)*128);

            if (int(vg/2)*2 == int(vg/4)*4)
               whichgroup = int(toprow/2)*2 + int(row/2)*128 + 2*(row - int(row/2)*2);
            else
               whichgroup = int((toprow-2)/2)*2 + int(row/2)*128 + 2*(row - int(row/2)*2) + 1;

            groupcount++;
            totalcount = totalcount + vegechild;
            QuadGroup64[whichgroup]->addChild(LeafGroup[vg].get());
         }
         else
         {
            LeafGroup[vg].release();
            LeafGroup[vg].~RefPtr();
            LeafGroup[vg] = NULL;
         }
      }
      if( groupcount != 0 )
         mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "gc = %i, max = %i, min = %i, ave = %i", groupcount, maxchildren, minchildren, totalcount/groupcount);
      else
         mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "gc = %i, max = %i, min = %i", groupcount, maxchildren, minchildren);

      //delete empty nodes
      for (int i = 0; i < 4096; i++)
      {
         if(QuadGroup64[i]->getNumChildren() == 0)
         {
            QuadGroup32[i/4]->removeChild(QuadGroup64[i].get());
            QuadGroup64[i].release();
            QuadGroup64[i].~RefPtr();
            QuadGroup64[i]=NULL;
         }
      }

      for (int i = 0; i < 1024; i++)
      {
         if(QuadGroup32[i]->getNumChildren() == 0)
         {
            QuadGroup16[i/4]->removeChild(QuadGroup32[i].get());
            QuadGroup32[i].release();
            QuadGroup32[i].~RefPtr();
            QuadGroup32[i]=NULL;
         }
      }

      for (int i = 0; i < 256; i++)
      {
         if(QuadGroup16[i]->getNumChildren() == 0)
         {
            QuadGroup8[i/4]->removeChild(QuadGroup16[i].get());
            QuadGroup16[i].release();
            QuadGroup16[i].~RefPtr();
            QuadGroup16[i]=NULL;
         }
      }
      for (int i = 0; i < 64; i++)
      {
         if(QuadGroup8[i]->getNumChildren() == 0)
         {
            QuadGroup4[i/4]->removeChild(QuadGroup8[i].get());
            QuadGroup8[i].release();
            QuadGroup8[i].~RefPtr();
            QuadGroup8[i]=NULL;
         }
      }
      for (int i = 0; i < 16; i++)
      {
         if(QuadGroup4[i]->getNumChildren() == 0)
         {
            QuadGroup2[i/4]->removeChild(QuadGroup4[i].get());
            QuadGroup4[i].release();
            QuadGroup4[i].~RefPtr();
            QuadGroup4[i]=NULL;
         }
      }
      for (int i = 0; i < 4; i++)
      {
         if(QuadGroup2[i]->getNumChildren() == 0)
         {
            lcccell.mRootVegeGroup.get()->removeChild(QuadGroup2[i].get());
            QuadGroup2[i].release();
            QuadGroup2[i].~RefPtr();
            QuadGroup2[i]=NULL;
         }
      }

      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Placement Done!");

      osgUtil::Optimizer optimizer;

      optimizer.optimize(lcccell.mRootVegeGroup.get(),
         osgUtil::Optimizer::SHARE_DUPLICATE_STATE |
         osgUtil::Optimizer::MERGE_GEOMETRY |
         osgUtil::Optimizer::REMOVE_REDUNDANT_NODES |
         osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS |
         osgUtil::Optimizer::CHECK_GEOMETRY);
      mLog->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,   "Optimization Done!");

      SetVegetationNode(lcccell.mRootVegeGroup.get());
   }

   //////////////////////////////////////////////////////////////////////////
   bool VegetationDecorator::GetVegetation(const osg::Image* mCimage, int x, int y, int limit)
   {
      unsigned char* c_data = NULL;

      if (mCimage!=NULL) c_data = (unsigned char*)mCimage->data(x,y);
      else return false;

      if (abs(c_data[1] - c_data[2]) < 15)   //fudge factor to account for JPEG compression artifacts
         return (c_data[1] <= limit);
      else
         return false;
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
      float diff = fabs(goodAngle - my_angle);

      if ((diff <= 45) || (diff >= 315))
         bin = 2;				// biased towards older vegetation
      else if (diff >= 135)
         bin = 0;				// biased toward younger vegetation
      else
         bin = 1;				// no bias

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
   int VegetationDecorator::GetNumLooks(const osg::Image* mCimage, const osg::Image* SLimage, int x, int y, float goodAngle, int maxlooks, float maxslope)
   {
      unsigned char *c_data = NULL;

      if(mCimage!=NULL)
      {
         c_data = (unsigned char*)mCimage->data(x,y);
         if ((c_data[0]==255)&&(c_data[1]==255)&&(c_data[2]==255))
            return 0;
      }

      unsigned char *s_data = NULL;
      int bin = -1;

      if (SLimage!=NULL)
      {
      
         s_data = (unsigned char*)SLimage->data(x,y);
      
      
         float my_angle = (s_data[2]/255.0f)*360.0f;
         float diff = fabs(goodAngle - my_angle);
         float slope = s_data[1];
         maxslope = (maxslope/90.0f) * 255.0f;	

         if (slope > maxslope/2.0f)
         {
            if ((diff <= 45) || (diff >= 315))
               bin = 2;				// biased towards older vegetation
            else if (diff >= 135)
               bin = 0;				// biased toward younger vegetation
            else
               bin = 1;				// no bias
         }
         else
         {
            bin = 1;
         }
      }

      switch (bin)
      {
      case 0:						// bad aspect angle
         return (int)ceil(maxlooks/2.0f);
         break;
      case 1:						// neutral angle
         return (int)ceil(maxlooks/1.5f);
         break;
      case 2:						// good angle
         return maxlooks;		
         break;
      }
      return 1;
   }
}
