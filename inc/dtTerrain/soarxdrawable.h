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
#ifndef DELTA_SOARXDRAWABLE_H
#define DELTA_SOARXDRAWABLE_H

#include <osg/Geometry>
#include <dtCore/refptr.h>
#include "dtTerrain/pagedterraintile.h"
#include "dtTerrain/terrain_export.h"

namespace dtTerrain
{   
   class HeightField;
   
   /**
    * This class encapsulates the SoarX rendering algorithm itself.  
    * @see SoarXTerrainRenderer
    */
   class DT_TERRAIN_EXPORT SoarXDrawable : public osg::Geometry
   {
      public:
      
         /**
          * This structure represents a location on the terrain's 2D grid.
          */
         struct Index
         {
            unsigned int q;
            int x,y;
            
            ///Simply sets all values to zero.
            Index() : q(0), x(0), y(0) { }            
            
            ///Simple named constructor.
            Index(int xValue, int yValue) { x=xValue; y=yValue; q=0; }
            Index(int qValue, int xValue, int yValue)
            {
               x = xValue;
               y = yValue;
               q = qValue;
            }
            
            ///
            Index(const Index &i, const Index &j, unsigned int parity, bool direction);
            
            /**
             * Clamps the x,y components of this index between zero and the
             * value specified.
             * @param The max value to clamp this index to.
             */
            void clamp(int value) 
            { 
               x = osg::clampTo(x,0,value); 
               y = osg::clampTo(y,0,value);               
            }
            
            /**
             * Less-than comparison on this Index with another value.
             * @return True if both x,y components of this index are less than the
             *    specified value.
             */             
            bool operator<(int value) { return x < value && y < value; }
            
            /**
             * Greater-than comparison on this Index with another value.
             * @return True if both x,y components of this index are greater than the
             *    specified value.
             */
            bool operator>(int value) { return x > value && y > value; }
            
            /**
             * Right shifts both x and y components of this index by the
             * specified number of bits.
             * @param value Number of bits to shift the components by.
             */
            void operator>>=(int bits) { x >>= bits; y >>= bits; }
            
            /**
             * Left shifts both x and y components of this index by the
             * specified number of bits.
             * @param value Number of bits to shift the components by.
             */            
            void operator<<=(int bits) { x <<= bits; y <<= bits; }
            
            /**
             * Performs a bit-wise AND on the x,y components of this index.
             * @param value The value to bit-wise AND with this index.
             */
            void operator&=(int value) { x &= value; y &= value; }
            
            /**
             * In place addition of the x,y components of this index with the
             * x,y components of the other.
             * @param rhs The index to add to this one.
             */
            void operator+=(const Index &rhs) { x += rhs.x; y += rhs.y; }
            
            /**
             * In place subtraction of the x,y components of this index with
             * the x,y components of the other.
             * @param rhs The index to subtract from this one.
             */
            void operator-=(const Index &rhs) { x -= rhs.x; y -= rhs.y; }
         };
         
         /**
          * This is the structure for a single processed vertex
          * in the terrain.
          */
         struct Vertex
         {            
            Vertex() { }
            Vertex(Index i) : index(i) { }
            
            Index index;
            osg::Vec3 position;
            float radius;
            float error;
         };
         
         /** 
          * This structure is the raw data on disk.  It may be compressed
          * or encoded.  The data in this structure is converted to vertex
          * data before it is rendered.
          */
         struct RawData
         {
            float height;
            float scale;
            float error;
            float radius;
         };
      
         /**
          * Sets up some default rendering parameters used during the
          * rendering and progressive refinement.
          * @param baseBits The size of the terrain chunk. (size=(2^baseBits))
          * @param horizontalResolution The distance between each height value.
          */
         SoarXDrawable(int baseBits, float horizontalResolution);
         
         /**
          * Makes a copy of the right hand side drawable.
          * @param rhs The SoarXDrawable to copy.
          * @param copyOp Specifies whether a deep copy or a shallow copy 
          *    should occur.
          */
         SoarXDrawable(const SoarXDrawable &rhs, const osg::CopyOp &copyOp = 
            osg::CopyOp::SHALLOW_COPY);
         
         /**
          * Calls the default constructor on this class and returns a new 
          * instance.
          */
         virtual osg::Object *cloneType() const;
         
         /**
          * Makes a new instance of this class by calling its copy constructor.
          */
         virtual osg::Object *clone(const osg::CopyOp &copyop) const;   
         
         /**
          * Draws the current view of the terrain dataset.
          * @param state The current OpenGL state.
          */
         virtual void drawImplementation(osg::RenderInfo & renderInfo) const;
      
         /**
          * 
          */
         virtual bool supports(osg::PrimitiveFunctor&) const { return true; }
         
         /**
          * Computes the dimensions of the currently loaded terrain.
          */
         virtual osg::BoundingBox computeBoundingBox() const;
         
         /**
          * Builds the internal data structures for use in rendering and managing
          * terrain data for a tile.
          * @param The source terrain tile.
          * @return True if the data was paged from the tile's cache, false otherwise.
          * @note This method will load pre-computed data from the tile's cache
          *    if present.  If the cache is not present, the data will be
          *    precomputed and then cached if the tile has its caching enabled.
          */
         bool Build(const PagedTerrainTile &tile);
         
         /**
          * Clears any memory used by the internal rendering structures.
          */
         void Clear();
         
         /**
          * Gets the elevation at the current (x,y) coordinates.
          */
         float GetHeight(float x, float y);
         
         /**
          * Sets the eye point of the camera.  This is used during the refinement
          * process to calculate vertex projection errors.
          * @param pos The camera position.
          */
         void SetEyePoint(const osg::Vec3 &pos) { mEyePoint = pos; }
         
         /**
          * Calculates and caches values used to determine error approximation 
          * in the rendering/refinement process.
          * @param value 
          * @return The clamped multiplier value.
          * @note The value given is clamped within the range 1-20.
          */
         float SetDetailMultiplier(float value)
         {
            mDetailMultiplier = osg::clampTo(value,1.0f,20.0f);
            CalculateRadii(mDetailMultiplier);
            return mDetailMultiplier;
         }
         
         /**
          * Gets the current detail multiplier.
          * @return The detail multiplier.
          */
         float GetDetailMultiplier() const
         {
            return mDetailMultiplier;
         }
         
         /**
          * Sets the threshold value used during error projection and refinement
          * in the rendering process.
          * @param value The new threshold value.
          * @return The clamped value.
          * @note The value given is clamped within the range 1-10
          */
         float SetThreshold(float value)
         {
            mThresholdValue = osg::clampTo(value,1.0f,10.0f);
            return mThresholdValue;
         }
         
         /**
          * Gets the current error threshold.
          * @return The current value.
          */
         float GetThreshold() const
         {
            return mThresholdValue;
         }
         
         /**
          * Gets the base vertical resolution.  This number determines
          * the overall scaling for height values in the terrain.
          * @return The current vertical resolution.
          */
         float GetBaseVerticalResolution() const
         {
            return mBaseVerticalResolution;
         }
         
         /**
          * Gets the base horizontal resolution.  This number determines
          * the overall scaling for the terrain's X and Y dimensions.
          * @return The current horizonal resolution.
          */
         float GetBaseHorizontalResolution() const
         {
            return mBaseHorizontalResolution;
         }
         
         /**
          * Gets the detail map's horizontal resolution.
          */
         float GetDetailHorizontalResolution() const { return mDetailHorizontalResolution; }
         
         /**
          * Sets the noise data used to procedurally generate additional detail 
          * vertices when rendering the terrain.
          * @param detailBits The size of the detail data.  The amount of data
          *    should be (2^detailBits * 2^detailBits).
          * @param detailVerticalResolution A scale value used when calculating the
          *    final detail vertex position.
          * @param detailData The detail noise values.
          */
         void SetDetailNoise(int detailBits, float detailVerticalResolution,
            float *detailData);
         
         /**
          * Gets the detail map's vertical resolution.
          */
         float GetDetailVerticalResolution() const { return mDetailVerticalResolution; }
         
         /**
          * Gets the size of the detail map.
          */
         int GetDetailSize() const { return mDetailSize; }
         
         bool RestoreDataFromCache(const PagedTerrainTile &tile);
         
         void WriteDataToCache(const PagedTerrainTile &tile);

      protected:
      
         ///Simply destroys the terrain drawable.
         virtual ~SoarXDrawable();
         
         ///Gets the raw data at the given index.
         RawData *GetRawData(Index index)
         {
            index.clamp(mBaseSize-1);
            return &mBaseRawData[index.y*mBaseSize + index.x];
         }
         
         /**
          * Precalculates vertex bounding spheres based on an object space
          * error metric.
          */
         void CalculateVertexErrors();
         
         ///Recursive method used by the above method to build the error
         ///values.
         void CalculateVertexErrorsHelper(Index i, Index j);
         
         /**
          * Calculates the object space error value of a vertex 
          * using the two indices provided.
          */
         float CalculateError(Index i, Index j);
         
         ///Gets a vertex based on the provided index.
         Vertex GetVertex(Index index);
         
         ///
         void CalculateRadii(float f);         
         
         /**
          * Ensures that the bounding spheres for each vertex satisfy the 
          * requirement that a vertex's bounding sphere contains all of its
          * descendant vertices' bounding spheres.  This ensures a proper
          * sphere tree for all the vertices.
          */
         void RepairBoundingSphereHierarchy();
         
         ///
         void CheckChildren1(Index index, unsigned int shift);
         
         ///
         void CheckChildren2(Index index, unsigned int shift);
         
         ///
         void Repair(Index i, Index c);
         
         /**
          * Refines and renders the terrain.
          */
         void Render(osg::State &state);
         
         /**
          * Adds a vertex and associated index to the vertex and index array.
          */
         void Append(Vertex &v);
         
         /**
          * Starts the procedural view-dependent refinement process.  The SoarX
          * refinement proceess is based on the longest edge bisection of
          * isosceles right triangles.  It subdivides each triangle about its
          * hypotenuse, creating two smaller triangles.
          */
         void Refine(Vertex &v1, Vertex &v2, bool in, bool out,
            unsigned int planes);
         
         /**
          * Recursivly refines the right of the two child triangles of the 
          * root refinement procedure.
          */
         void RightRefine(Vertex v1, Vertex &v2, bool out, unsigned int planes);
         
         /**
          * Recursivly refines the left of the two child triangles of the 
          * root refinement procedure.
          */   
         void LeftRefine(Vertex &v1, Vertex &v2, bool in, unsigned int planes);
            
         /**
          * Inserts a degenerate triangle in the terrain vertex stream in order
          * to generate a maximum length triangle strip during the refinement 
          * process.
          */
         void TurnCorner();
            
         /**
          * Decides is a vertex to be in the resulting rendered terrain mesh.
          * This is performed by calculating its position and its 2D/3D projected
          * error.  Hierarchial view-frustum culling is also performed here.
          * @param v The vertex to check.
          * @param planes A bitmask designating which planes to cull to.
          * @param level Current level of recursion.
          */
         bool Active(Vertex &v, unsigned int &planes);   
         
         /**
          * Retrieves a vertex from the procedural terrain mesh.
          */
         void GetVertex(Vertex &v);

      private:         
         
         float mThresholdValue;
         float mDetailMultiplier;
         float mMagic;
         
         Vertex mBaseVertices[13];
         Index mBaseIndices[9];
         float mRadii[64];
         RawData *mBaseRawData, *mBaseQRawData;
         float *mDetailNoise;
         
         //Used during the refinement process to determine how to tesselate the 
         //geometry.
         bool mLeftOnly, mFirst;
         unsigned int mLevel;
         
         ///The primary vertex and index array used when building the renderable terrain data.
         ///Note, there are two because the rendering and filling are "page-flipped"
         ///so one array can be filled while the graphics card is pulling from
         ///the other.
         //dtCore::RefPtr<osg::Vec3Array> mVertexArray[2];
         //dtCore::RefPtr<SoarXDrawElementsUInt> mIndexArray[2];
         osg::Vec3 *mVertexArray[2];
         unsigned int *mIndexArray[2];
         osg::Vec3 *mActiveVertexArray;
         unsigned int *mActiveIndexArray;
         unsigned char mCurrentPage;
         unsigned int mVAIndex,mIAIndex;
         
         //Used in frustum culling the terrain vertices.
         osg::Polytope::PlaneList mFrustumPlanes;
         
         ///The current position of the camera.
         osg::Vec3 mEyePoint;
         
         ///This vertex and index buffer is constructed during the refinement process to 
         ///compensate for cracks in the borders of terrain tiles.
         std::vector<unsigned int> mSkirtIndices;
         std::vector<osg::Vec3> mSkirtVertices;
         float mSkirtHeight;
      
         int mMapBits;
         int mBaseBits;
         int mDetailBits;
         int mEmbeddedBits;
         
         int mMapSize;
         int mBaseSize;
         int mDetailSize;
         int mEmbeddedSize;
         
         float mBaseHorizontalResolution;
         float mBaseVerticalResolution;
         float mBaseVerticalBias;
         
         float mDetailHorizontalResolution;
         float mDetailVerticalResolution;
         float mDetailVerticalBias;
         
         unsigned int mMapLevels;
         unsigned int mDetailLevels;
         unsigned int mBaseLevels;           
         
         friend struct SoarXCullCallback;

         ///Number of vertices and indices in the render buffers.
         unsigned int BUFFER_SIZE;
   };   
   
}

#endif
