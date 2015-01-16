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

#ifndef DELTA_VEGETATIONDECORATOR
#define DELTA_VEGETATIONDECORATOR

#include <osg/Vec3>
#include <osg/Group>
#include <osg/Image>
#include <dtCore/refptr.h>
#include <dtUtil/enumeration.h>
#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/lccanalyzer.h>
#include <dtTerrain/lcctype.h>

namespace dtTerrain
{
   class VegetationInvalidLCCTypesException : public dtUtil::Exception
   {
   public:
   	VegetationInvalidLCCTypesException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~VegetationInvalidLCCTypesException() {};
   };
   
   class VegetationInvalidSlopeAspectImageException : public dtUtil::Exception
   {
   public:
   	VegetationInvalidSlopeAspectImageException(const std::string& message, const std::string& filename, unsigned int linenum);
   	virtual ~VegetationInvalidSlopeAspectImageException() {};
   };
   

   /**
    * This class is the vegetation decorator layer. It is responsible
    * for processing the probabilities of vegetation placement based on
    * LCC data recieved from the LCCAnalyzer.
    */
   class DT_TERRAIN_EXPORT VegetationDecorator : public TerrainDecorationLayer
   {
      public:

         ///Help minimize some typing...
         typedef std::map<dtCore::RefPtr<PagedTerrainTile>,dtCore::RefPtr<osg::Group> >
            VegetationMap;

         /**
          * Constructs a new terrain decoration layer.
          */
         VegetationDecorator(const std::string &name="VegetationDecoratorLayer");

         /**
          * Calculates various LCC images and procedurally builds a list
          * of trees and other vegetation models which are then placed
          * about the specified terrain tile.
          */
         virtual void OnLoadTerrainTile(PagedTerrainTile &tile);

         /**
          * Removes the vegetation models from the map of currently
          * visible tiles/vegetation.
          */
         virtual void OnUnloadTerrainTile(PagedTerrainTile &tile);

         /**
          * Places the actual vegetation geometry on the terrain.  Note,
          * the vegetation placement occurs here since it is dependent
          * on querying for the height of the terrain in order to place
          * the vegetation.  The terrain height can only be queried once
          * it has been loaded by the renderer.  Performing this here
          * ensures that the renderer has loaded this tile's data.
          */
         virtual void OnTerrainTileResident(PagedTerrainTile &tile);

         /**
          * Gets the root scenegraph node for the vegetation.
          * @return A group node holding the vegetation hierarchy for the
          *    current visible set of tiles.
          */
         virtual osg::Node *GetOSGNode() { return mVegetationNode.get(); }

         /**
          * Place the vegetation into cell specified by lat-long.
          * @param The terrain tile with which to place vegetation.
          * @return A group node containing the vegetation scene for the
          *    specified tile.
          */
         dtCore::RefPtr<osg::Group> AddVegetation(PagedTerrainTile &tile);

         /**
          * Determine whether vegetation exists at coord x,y
          * @param mCimage the LCC type's combined image)
          * @param x the x coordinate to check
          * @param y the y coordinate to check
          * @param limit the probability rolled
          * @return boolean on existence of vegetaton at x,y
          */
         bool GetVegetation(osg::Image &mCimage, int x, int y, int limit);

         /**
          * Determine type/age of vegetation type (1-3; young-old).
          * @param mCimage the LCC type's combined image)
          * @param x the x coordinate to check
          * @param y the y coordinate to check
          * @param pref_angle the preferred angle of aspect in degrees for optimum growth
          * @return the age bias of the vegetation type
          */
         int GetVegType(const osg::Image* mCimage, int x, int y, float good_angle);

         int GetNumLooks(const osg::Image &compositeImage,
                         const osg::Image &slopeMap,
                         int x,
                         int y,
                         float good_angle,
                         int maxlooks,
                         float maxslope);

         /**
          * Sets the distance between vegetation objects
          * @param distance between vegetation objects
          */
         void SetVegetationDistance(const float distance)
         {
            mVegetationDistance = distance;
         }

         void SetLCCTypes(std::vector<dtTerrain::LCCType> &lccTypes)
         {
            mLCCTypes = lccTypes;
         }

         void SetGeospecificImage(const std::string& geoImageFilename)
         {
            mGeoImageFilename = geoImageFilename;
            mLCCAnalyzer.AddGeospecificImage(mGeoImageFilename);
         }

         void SetRandomSeed(const int &seed) { mSeed = seed; }

         /**
          * Sets the limit on the number of vegetation models placed per terrain
          * tile.
          * @param newMax The new limit.
          */
         void SetMaxVegetationPerCell(int newMax) { mMaxVegetationPerCell = newMax; }

         /**
          * Gets a reference to the LCCAnlyzer.
          * @return The LCCAnalyzer for this vegetation decorator.
          */
         LCCAnalyzer &GetLCCAnalyzer() { return mLCCAnalyzer; }

      protected:

         dtCore::RefPtr<osg::Group> BuildVegetationForType(osg::Image &compositeImage, osg::Image &slopeMap,
            osg::Vec3 &cellOrigin, LCCType &type);

         /**
          * Destructor
          */
         virtual ~VegetationDecorator();

      private:
         std::vector<dtTerrain::LCCType> mLCCTypes;
         LCCAnalyzer mLCCAnalyzer;
         std::string mGeoImageFilename;

         dtCore::RefPtr<osg::Group> mVegetationNode;
         VegetationMap mVegetationMap;

         float mVegetationDistance;
         int mSeed;
         int mMaxLooks;
         int mMaxVegetationPerCell;
         int mCellVegetationCount;
   };
}

#endif
