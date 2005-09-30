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
#ifndef DELTA_VEGETATIONDECORATOR
#define DELTA_VEGETATIONDECORATOR

#include <osg/Vec3>
#include <osg/Texture2D>
#include <dtCore/refptr.h>
#include "dtTerrain/terraindecorationlayer.h"
#include "dtTerrain/terraintile.h"
#include "dtTerrain/lccanalyzer.h"
#include "dtTerrain/lcctype.h"

//Foward declare necessary OSG classes.
namespace osg
{
   class Node;
}

namespace dtTerrain
{
   /**
   * This class is the vegetation decorator layer. It is responsible
   * for processing the probabilities of vegetation placement based on
   * LCC data recieved from the LCCAnalyzer.
   */
   class DT_TERRAIN_EXPORT VegetationDecorator : public TerrainDecorationLayer
   {
   public:
      /**
      * Constructs a new terrain decoration layer.
      */
      VegetationDecorator(const std::string &name="VegetationDecoratorLayer");
      
      /**
      * 
      */      
      void LoadResource(int latitude,int longitude);
      
      /**
      * Inherited from abstract class - not used but required
      */
      void LoadResource(const std::string &path){};
      
      /**
      * Determine the scene node of the vegetation layer
      * @return osg node
      */
      osg::Node *GetOSGNode()
      {
         return mVegetationNode.get();
      }

      /**
      * Place the vegetation into cell specified by lat-long.
      * @param latitude the latitude of the origin
      * @param longitude the longitude of the origin
      */		 
      void AddVegetation(int latitude, int longitude);

      /**
      * Determine whether vegetation exists at coord x,y
      * @param mCimage the LCC type's combined image)
      * @param x the x coordinate to check
      * @param y the y coordinate to check
      * @param limit the probability rolled
      * @return boolean on existence of vegetaton at x,y
      */
      bool GetVegetation(const osg::Image* mCimage, int x, int y, int limit);

      /**
      * Determine type/age of vegetation type (1-3; young-old).
      * @param mCimage the LCC type's combined image)
      * @param x the x coordinate to check
      * @param y the y coordinate to check
      * @param pref_angle the preferred angle of aspect in degrees for optimum growth
      * @return the age bias of the vegetation type
      */
      int GetVegType(const osg::Image* mCimage, int x, int y, float good_angle);

      /**
      * Determine whether vegetation exists at coord x,y
      * @param mCimage the LCC type's combined image)
      * @param x the x coordinate to check
      * @param y the y coordinate to check
      * @param limit the probability rolled
      * @param maximum look
      * @param maximum slope
      * @return boolean on existence of vegetaton at x,y
      */
      int GetNumLooks(const osg::Image* mCimage, const osg::Image* SLimage, int x, int y, float good_angle, int maxlooks, float maxslope);

      /**
      * Sets the vegetations load distance
      * @param load distance for the vegetation
      */
      void SetLoadDistance(float loadDistance)
      {
         mLoadDistance = loadDistance;
      }

      /**
      * Sets the distance between vegetation objects
      * @param distance between vegetation objects
      */
      void SetVegeDistance(float vegeDistance)
      {
         mVegeDistance = vegeDistance;
      }

      /**
      * Sets the maximum texture size, which is currently used for the placement of vegetation
      * @param max texture size
      */
      void SetMaxTextureSize( int maxTextureSize )
      {
         mMaxTextureSize = maxTextureSize;
      }

      /**
      * This sets the osg group node for the processed decoration layer
      * @param osg group node for the vegetation decorator
      */
      void SetVegetationNode(osg::Group *vegetationNode)
      {
         mVegetationNode = vegetationNode;
      }

      /**
      * This is a list of vegetation types created by the application 
      * to be used for processing the placement of vegetation. 
      * @param vector of LCCType objects
      */ 
      void SetLCCData(std::vector<dtTerrain::LCCType> lccTypes)
      {
         this->mLCCs = lccTypes;
      }

   protected:

      /** 
      * Destructor
      */
      virtual ~VegetationDecorator();

   private:
      std::vector<dtTerrain::LCCType> mLCCs;
      dtCore::RefPtr<osg::Group> mVegetationNode;
      float mVegeDistance;
      std::string mCachePath;
      int mMaxTextureSize;
      int mSeed;
      float mLoadDistance;
      std::string mImageExtension;
      int mOriginLongitude;
      int mOriginLatitude;
      double mSemiMajorAxis;
      int mOriginElevation;
      int mDetailMultiplier;
      int mMaxLooks;
      int mMaxObjectsPerCell;
      int mTotalVegeCount;

      /**
		* Maps loaded segments to LCCCells.
		*/
      std::map<TerrainTile, LCCAnalyzer::LCCCells> mSegmentLCCCellMap;

      dtCore::RefPtr<dtUtil::Log> mLog;
      int mLongitude;
      int mLatitude;
   };
}
#endif
