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

#include <osg/NodeVisitor>

#include <dtTerrain/soarxdrawable.h>
#include <dtTerrain/imageutils.h>
#include <dtTerrain/mathutils.h>
#include <dtTerrain/terrain.h>

#include <dtUtil/log.h>
#include <dtCore/scene.h>

#include <osg/Image>
#include <osg/io_utils>
#include <osgDB/WriteFile>
#include <osg/Texture2D>

#include <iostream>
#include <sstream>

#include <dtTerrain/terraindecorationlayer.h>
#include <dtTerrain/terraindatareader.h>
#include <dtTerrain/soarxterrainrenderer.h>
#include <dtTerrain/heightfield.h>

namespace dtTerrain
{
   ///Constants used during the refinement process.
   const bool BOTTOM = false;
   const bool SIDE = true;

   /**
    * This callback is used by the SoarX renderer to update the camera's
    * position or eyepoint which is needed by the refinement process
    * to calculate projected vertex errors.
    */
   struct SoarXCullCallback : public osg::Drawable::CullCallback
   {
      virtual bool cull(osg::NodeVisitor *visitor, osg::Drawable* drawable,
         osg::State *state) const
      {
         SoarXDrawable *soarX = dynamic_cast<SoarXDrawable *>(drawable);
         if (soarX != NULL)
         {
            soarX->SetEyePoint(visitor->getEyePoint());
         }

         return false;
      }
   };

   //////////////////////////////////////////////////////////////////////////
   SoarXDrawable::Index::Index(const Index &i, const Index &j, unsigned int parity,
      bool direction)
   {
      if (direction)
      {
         q = (i.q << 2) + (((i.q << 1) + j.q + 3) & 3) - 11;

      }
      else
      {
         q = (i.q << 2) + (((i.q << 1) + j.q + 2) & 3) - 11;
      }

      int t;
      if (!parity ^ direction)
      {
         t = j.x - i.y;
         x = y = i.x + j.y;
      }
      else
      {
         t = i.x - j.y;
         x = y = j.x + i.y;
      }

      x += t;
      y -= t;
      x >>= 1;
      y >>= 1;
   }

   //////////////////////////////////////////////////////////////////////////
   SoarXDrawable::SoarXDrawable(int baseBits, float horizontalResolution)
   {
      //Make sure we disable the use of display lists since the render data
      //is dynamic and computed on the fly.
      setSupportsDisplayList(false);
      //setUseVertexBufferObjects(true);

      // How the hell did these magik numbers appear? Note that it is now
      // overwritten in Build. -osb
      BUFFER_SIZE = 512*1024 + 1024;

      mCurrentPage = 0;
      mVAIndex = 0;
      mIAIndex = 0;
      mLeftOnly = true;
      setCullCallback(new SoarXCullCallback());

      mBaseRawData = mBaseQRawData = NULL;
      mVertexArray[0] = mVertexArray[1] = NULL;
      mIndexArray[0] = mIndexArray[1] = NULL;
      mDetailNoise = NULL;
      mSkirtHeight = 200.0f;
      mThresholdValue = 2.0f;
      mDetailMultiplier = 3.0f;

      mBaseBits = baseBits;
      mMapBits = 16;
      mEmbeddedBits = mMapBits - mBaseBits;

      mMapSize = (1 << mMapBits) + 1;
      mBaseSize = (1 << mBaseBits) + 1;
      mEmbeddedSize = 1 << mEmbeddedBits;

      mBaseVerticalResolution = 1.0f;
      mBaseHorizontalResolution = horizontalResolution;
      mBaseVerticalBias = 0.0f;
      mDetailHorizontalResolution = mBaseHorizontalResolution / (float)mEmbeddedSize;

      mMapLevels = mMapBits << 1;
      mBaseLevels = mBaseBits << 1;

      //Calculate a lookup table of bounding sphere radii for use
      //in the vertex error projection calculations.
      CalculateRadii(mDetailMultiplier);

      //Update our bounding volume...
      dirtyBound();

      //Precalculate our vertices used during the refinement process..
      int c4,c2,c1,c3;

      c4 = mMapSize - 1;
      c2 = c4 >> 1;
      c1 = c2 >> 1;
      c3 = c2 + c1;

      mBaseVertices[0].index = Index(0,0,c4);
      mBaseVertices[1].index = Index(1,c4,c4);
      mBaseVertices[2].index = Index(2,c4,0);
      mBaseVertices[3].index = Index(3,0,0);
      mBaseVertices[4].index = Index(4,c2,c2);
      mBaseVertices[5].index = Index(5,c2,0);
      mBaseVertices[6].index = Index(6,c4,c2);
      mBaseVertices[7].index = Index(7,c2,c4);
      mBaseVertices[8].index = Index(8,0,c2);
      mBaseVertices[9].index = Index(9,c3,c1);
      mBaseVertices[10].index = Index(14,c1,c1);
      mBaseVertices[11].index = Index(19,c1,c3);
      mBaseVertices[12].index = Index(24,c3,c3);
   }

   //////////////////////////////////////////////////////////////////////////
   SoarXDrawable::SoarXDrawable(const SoarXDrawable &rhs, const osg::CopyOp &copyOp) :
      osg::Geometry(rhs,copyOp)
   {
      //Needs to be implemented!!!
   }

   //////////////////////////////////////////////////////////////////////////
   SoarXDrawable::~SoarXDrawable()
   {
      Clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::Clear()
   {
      if (mBaseRawData != NULL)
         delete [] mBaseRawData;

      if (mBaseQRawData != NULL)
         delete [] mBaseQRawData;

      if (mVertexArray[0] != NULL)
         delete [] mVertexArray[0];
      if (mVertexArray[1] != NULL)
         delete [] mVertexArray[1];
      if (mIndexArray[0] != NULL)
         delete [] mIndexArray[0];
      if (mIndexArray[1] != NULL)
         delete [] mIndexArray[1];

      mBaseRawData = mBaseQRawData = NULL;
      mVertexArray[0] = mVertexArray[1] = NULL;
      mIndexArray[0] = mIndexArray[1] = NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Object* SoarXDrawable::cloneType() const
   {
      return new SoarXDrawable(mBaseBits,mBaseHorizontalResolution);
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Object* SoarXDrawable::clone(const osg::CopyOp& copyOp) const
   {
      return new SoarXDrawable(*this,copyOp);
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::drawImplementation(osg::RenderInfo & renderInfo) const
   {
      //This is a terrible hack to get around the fact that we need to
      //modify the drawable data before actually drawing it even though
      //this method should not be modifying data, only displaying it.

      (const_cast<SoarXDrawable *>(this))->Render(*renderInfo.getState());

      //Use the base class's drawing code to actually push the vertex and index
      //buffers to the graphics card.
      osg::Geometry::drawImplementation(renderInfo);
   }

   //////////////////////////////////////////////////////////////////////////
   osg::BoundingBox SoarXDrawable::computeBoundingBox() const
   {
      //float horizSize = mBaseHorizontalResolution * mBaseSize;
      //osg::BoundingBox bb;

      //bb = osg::BoundingBox(-1e10,1e10,-1e10,1e10,-1e10,1e10);
      //setBound(bb);
      return osg::BoundingBox();
   }

   //////////////////////////////////////////////////////////////////////////
   float SoarXDrawable::GetHeight(float x, float y)
   {
      x /= mDetailHorizontalResolution;
      y /= mDetailHorizontalResolution;

      float xq = floorf(x);
      float yq = floorf(y);
      x -= xq;
      y -= yq;

      Index i0((unsigned int)xq, (unsigned int)yq);
      Index i1((unsigned int)xq+1, (unsigned int)yq);
      Index i2((unsigned int)xq, (unsigned int)yq+1);
      Index i3((unsigned int)xq+1, (unsigned int)yq+1);

      i0.clamp(mMapSize);
      i1.clamp(mMapSize);
      i2.clamp(mMapSize);
      i3.clamp(mMapSize);

      Vertex v0(i0);
      Vertex v1(i1);
      Vertex v2(i2);
      Vertex v3(i3);

      GetVertex(v0);
      GetVertex(v1);
      GetVertex(v2);
      GetVertex(v3);

      return MathUtils::Lerp(y,
         MathUtils::Lerp(x,v0.position.z(),v1.position.z()),
         MathUtils::Lerp(x,v2.position.z(),v3.position.z()));
   }

   //////////////////////////////////////////////////////////////////////////
   bool SoarXDrawable::Build(const PagedTerrainTile &tile)
   {
      unsigned int i,j;

      if (mDetailNoise == NULL)
      {
         throw dtTerrain::NullPointerException("MUST specify detail noise data before "
            "building rendering data structures.  Cannot build tile.", __FILE__, __LINE__);
      }

      Clear();

      //Precalculate our base indices..
      int cx = mBaseSize - 1;
      int ch = (mBaseSize - 1) >> 1;

      mBaseIndices[0] = Index(ch,ch);
      mBaseIndices[1] = Index(ch,0);
      mBaseIndices[2] = Index(cx,0);
      mBaseIndices[3] = Index(cx,ch);
      mBaseIndices[4] = Index(cx,cx);
      mBaseIndices[5] = Index(ch,cx);
      mBaseIndices[6] = Index(0,cx);
      mBaseIndices[7] = Index(0,ch);
      mBaseIndices[8] = Index(0,0);

      //First, check the cache to see if this data has already been calculated.
      //If RestoreDataFromCache fails, the code then checks to see what data
      //was not available and generates it.
      bool usedCache = true;
      if (!RestoreDataFromCache(tile))
      {
         if (mBaseRawData == NULL)
         {
            LOG_INFO("Generating vertex error values and sphere tree hierarchy.");

            const HeightField *hf = tile.GetHeightField();
            mBaseRawData = new RawData[mBaseSize*(mBaseSize+1)];
            for (i=0; i<hf->GetNumRows(); i++)
            {
               for (j=0; j<hf->GetNumColumns(); j++)
               {
                  RawData *data = GetRawData(Index(j,i));
                  data->height = hf->GetInterpolatedHeight((float)j * (float)hf->GetNumColumns()/(float)mBaseSize,(float)(hf->GetNumRows() - 1) -
                                                           (float)i * (float)hf->GetNumRows()/(float)mBaseSize) *
                                                            mBaseVerticalResolution + mBaseVerticalBias;
                  data->error = 0.0f;
                  data->radius = 0.0f;
                  data->scale = 1.0f;
               }
            }

            LOG_INFO("Precalculating vertex error values.");
            CalculateVertexErrors();

            LOG_INFO("Ensuring proper sphere tree for terrain vertices.");
            RepairBoundingSphereHierarchy();
         }

         usedCache = false;
      }

      BUFFER_SIZE = tile.GetHeightField()->GetNumRows() *
                    unsigned( double( tile.GetHeightField()->GetNumColumns() ) * 1.1 );

      // isdale: too big a buffer size will run us out of memory
      // should be a way to catch this, pre-test for it.
      mVertexArray[0] = new osg::Vec3[BUFFER_SIZE];
      mVertexArray[1] = new osg::Vec3[BUFFER_SIZE];
      mIndexArray[0] = new unsigned int[BUFFER_SIZE];
      mIndexArray[1] = new unsigned int[BUFFER_SIZE];

      for (i=0; i<13; i++)
         GetVertex(mBaseVertices[i]);

      return usedCache;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::GetVertex(Vertex &v)
   {
      const float INV_EMBEDDED_SIZE = 1.0f / (float)mEmbeddedSize;

      Index baseIndex = v.index;
      Index detailIndex = v.index;
      Index interpolateIndex = v.index;

      baseIndex >>= mMapBits-mBaseBits;
      detailIndex &= mDetailSize-1;
      interpolateIndex &= mEmbeddedSize-1;

      int iBase = (baseIndex.y << mBaseBits) + baseIndex.y + baseIndex.x;
      int iDetail = (detailIndex.y << mDetailBits) + detailIndex.x;

      RawData v0 = mBaseRawData[iBase];
      float d0 = mDetailNoise[iDetail];

      v.position.x() = mDetailHorizontalResolution * v.index.x;
      v.position.y() = mDetailHorizontalResolution * v.index.y;

      if (interpolateIndex.x != 0 || interpolateIndex.y != 0)
      {
         RawData v1 = mBaseRawData[iBase+1];
         RawData v2 = mBaseRawData[iBase+mBaseSize];
         RawData v3 = mBaseRawData[iBase+mBaseSize+1];

         float x = interpolateIndex.x * INV_EMBEDDED_SIZE;
         float y = interpolateIndex.y * INV_EMBEDDED_SIZE;

         v.position.z() = MathUtils::Lerp(y,
                             MathUtils::Lerp(x,v0.height,v1.height),
                             MathUtils::Lerp(x,v2.height,v3.height)) +
                          MathUtils::Lerp(y,
                             MathUtils::Lerp(x,v0.scale,v1.scale),
                             MathUtils::Lerp(x,v2.scale,v3.scale)) * d0;
      }
      else
      {
         v.position.z() = v0.height + v0.scale*d0;
         v.radius = v0.radius;
         v.error = v0.error;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::CalculateRadii(float f)
   {
      static const float T = 1.0f / sqrtf(2.0f);
      int i;

      mRadii[0] = 1.0f / (1.0f-T) * mDetailHorizontalResolution*mMapSize*0.5f*f;
      for (i=1; i<64; i++)
         mRadii[i] = mRadii[i-1] * T;
      for (i=0; i<64; i++)
         mRadii[i] = mRadii[i] * mRadii[i];
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::SetDetailNoise(int detailBits, float detailVerticalResolution,
      float *detailData)
   {
      mDetailNoise = detailData;
      mDetailBits = detailBits;
      mDetailSize = 1 << mDetailBits;
      mDetailLevels = mDetailBits << 1;
      mDetailVerticalResolution = detailVerticalResolution;
      mDetailVerticalBias = -32768.0f * mDetailVerticalResolution;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::CalculateVertexErrors()
   {
      Index c = mBaseIndices[0];

      mLevel = 0;
      for (unsigned int i=1; i<9; i++)
      {
         Index j = mBaseIndices[i++];
         CalculateVertexErrorsHelper(c,j);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::CalculateVertexErrorsHelper(Index i, Index j)
   {
      mLevel++;

      float errorValue = CalculateError(i,j);
      GetRawData(j)->error = errorValue;

      if (mLevel < mBaseLevels-1)
      {
         CalculateVertexErrorsHelper(j,Index(i,j,0,false));
         CalculateVertexErrorsHelper(j,Index(i,j,0,true));
      }

      mLevel--;
   }

   //////////////////////////////////////////////////////////////////////////
   float SoarXDrawable::CalculateError(Index i, Index j)
   {
      Index l(j.x+i.y-j.y, j.y+j.x-i.x);
      Index r(j.x+j.y-i.y, j.y+i.x-j.x);

      osg::Vec3 jp(GetVertex(j).position);
      osg::Vec3 lp(GetVertex(l).position);
      osg::Vec3 rp(GetVertex(r).position);

      osg::Vec3 lerp;
      lerp = lp + rp;
      lerp *= 0.5f;

      return (jp-lerp).length();
   }

   //////////////////////////////////////////////////////////////////////////
   SoarXDrawable::Vertex SoarXDrawable::GetVertex(Index index)
   {
      Vertex v;

      index.clamp(mBaseSize - 1);
      RawData *d = GetRawData(index);
      v.position = osg::Vec3(mBaseHorizontalResolution * index.x,
         mBaseHorizontalResolution * index.y,d->height);
      v.error = d->error;
      v.radius = d->radius;

      return v;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::RepairBoundingSphereHierarchy()
    {
      unsigned int counterBits;
      unsigned int x,y;

      for (counterBits=mBaseBits; counterBits>0; counterBits--)
      {
         unsigned int shift = mBaseBits-counterBits;
         unsigned int counter = 1 << (counterBits-1);

         if (shift > 0) //Only from second level.
         {
            for (y=0; y<=counter; y++)
            {
               for (x=0; x<=counter; x++)
               {
                  CheckChildren1(Index(((x<<1) + 1), (y<<1)), shift);
                  CheckChildren1(Index((x<<1), ((y<<1) + 1)), shift);
               }
            }
         }

         for (y=0; y<counter; y++)
         {
            for (x=0; x<counter; x++)
            {
               CheckChildren2(Index(((x<<1) + 1), ((y<<1) + 1)), shift);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::CheckChildren1(Index index, unsigned int shift)
   {
      //Index t(index);
      unsigned int w = 1 << (shift-1);

      index <<= shift;
      Repair(index,Index(index.x+w,index.y+w));

      if (index.x > 0 && index.y > 0)
         Repair(index, Index(index.x - w, index.y - w));

      if (index.x > 0)
         Repair(index, Index(index.x-w, index.y+w));

      if (index.y > 0)
         Repair(index, Index(index.x+w, index.y-w));
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::CheckChildren2(Index index, unsigned int shift)
   {
      Index t(index);

      index <<= shift;
      Repair(index,Index((t.x-1) << shift, t.y << shift));
      Repair(index,Index(t.x << shift, (t.y-1) << shift));
      Repair(index,Index((t.x+1) << shift, t.y << shift));
      Repair(index,Index(t.x << shift, (t.y+1) << shift));
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::Repair(Index i, Index c)
   {
      Vertex vi = GetVertex(i);
      Vertex vc = GetVertex(c);
      RawData* di = GetRawData(i);
      float d;

      d = (vi.position - vc.position).length() + vc.radius;
      if (d > di->radius)
         di->radius = d;

      if (vc.error > di->error)
         di->error = vc.error;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::Render(osg::State &state)
   {
      ///(111111) bitmask instructing the start of the refinement process to
      ///check all 6 planes.
      const unsigned int PLANEMASK = 63;

      mActiveVertexArray = mVertexArray[mCurrentPage];//.get();
      mActiveIndexArray = mIndexArray[mCurrentPage];//.get();
      mFrustumPlanes = state.getViewFrustum().getPlaneList();

      //Calculate the number used to build projection errors
      //using the current vertical field of view and the height of
      //the viewport.
      int windowSizeY = state.getCurrentViewport()->height();
      osg::Matrix mat = state.getProjectionMatrix();
      double fovY,aspect,nearZ,farZ;

      mat.getPerspective(fovY,aspect,nearZ,farZ);
      mMagic = (float)windowSizeY / ((float)osg::DegreesToRadians(fovY) * mThresholdValue);

      //Make sure our primitive sets and vertex arrays are assigned properly.
      if (getNumPrimitiveSets() != 0)
         removePrimitiveSet(0);

      mVAIndex = 0;
      mIAIndex = 0;
      mSkirtIndices.clear();
      mSkirtVertices.clear();

      mFirst = true;
      mLevel = 1;
      Append(mBaseVertices[3]);
      Append(mBaseVertices[3]);

      mLeftOnly = false;
      Refine(mBaseVertices[8],mBaseVertices[10],BOTTOM,SIDE,PLANEMASK);
      Append(mBaseVertices[4]);
      mLeftOnly = false;
      Refine(mBaseVertices[8],mBaseVertices[11],SIDE,BOTTOM,PLANEMASK);
      Append(mBaseVertices[0]);

      mLeftOnly = false;
      Refine(mBaseVertices[7],mBaseVertices[11],BOTTOM,SIDE,PLANEMASK);
      Append(mBaseVertices[4]);
      mLeftOnly = false;
      Refine(mBaseVertices[7],mBaseVertices[12],SIDE,BOTTOM,PLANEMASK);
      Append(mBaseVertices[1]);

      mLeftOnly = false;
      Refine(mBaseVertices[6],mBaseVertices[12],BOTTOM,SIDE,PLANEMASK);
      Append(mBaseVertices[4]);
      mLeftOnly = false;
      Refine(mBaseVertices[6],mBaseVertices[9],SIDE,BOTTOM,PLANEMASK);
      Append(mBaseVertices[2]);

      mLeftOnly = false;
      Refine(mBaseVertices[5],mBaseVertices[9],BOTTOM,SIDE,PLANEMASK);
      Append(mBaseVertices[4]);
      mLeftOnly = false;
      Refine(mBaseVertices[5],mBaseVertices[10],SIDE,BOTTOM,PLANEMASK);
      Append(mBaseVertices[3]);

      for (unsigned int i=0; i<mSkirtVertices.size(); i++)
      {
         mActiveIndexArray[mIAIndex++] = mSkirtIndices[i];
         mActiveIndexArray[mIAIndex++] = mVAIndex;
         mActiveVertexArray[mVAIndex++] = mSkirtVertices[i];
      }

      mCurrentPage = 1-mCurrentPage;
      //if (mActiveIndexArray->getNumIndices() != mIAIndex)
     // {
       //  mActiveIndexArray->setNumIndices(mIAIndex);
        // mActiveIndexArray->dirty();
      //}

      addPrimitiveSet(new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP,
         mIAIndex,(GLuint*)&mActiveIndexArray[0]));
      setVertexArray(new osg::Vec3Array(mVAIndex,&mActiveVertexArray[0]));
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::Append(Vertex &v)
   {
      if (v.index.x == 0 || v.index.y == 0 ||
          v.index.x == mMapSize-1 || v.index.y == mMapSize-1)
      {
         //Edge vertex so add it to the skirt.
         mSkirtIndices.push_back(mVAIndex);
         mSkirtVertices.push_back(v.position);
         mSkirtVertices.back().z() -= mSkirtHeight;
      }

      mActiveVertexArray[mVAIndex] = v.position;
      mActiveIndexArray[mIAIndex] = mVAIndex;
      mVAIndex++;
      mIAIndex++;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::Refine(Vertex &v1, Vertex &v2, bool in, bool out,
      unsigned int planes)
   {
      mLevel++;
      Vertex leftVertex(Index(v1.index,v2.index,(mLevel & 1), false));
      Vertex rightVertex(Index(v1.index,v2.index,(mLevel & 1), true));

      unsigned int leftPlanes = planes;
      unsigned int rightPlanes = planes;
      bool isLeft,isRight;

      isLeft = Active(leftVertex,leftPlanes);
      isRight = Active(rightVertex,rightPlanes);

      if (isLeft)
      {
         if (isRight) //Left-right
         {
            Refine(v2,leftVertex,!in,SIDE,leftPlanes);
            Append(v1);
            if (mLeftOnly)
            {
               Append(v2);
               RightRefine(v2,rightVertex,!out,rightPlanes);
            }
            else
            {
               Refine(v2,rightVertex,SIDE,!out,rightPlanes);
            }

            mLeftOnly = false;
         }
         else //Left only
         {
            mLeftOnly = true;
            LeftRefine(v2,leftVertex,!in,leftPlanes);
            Append(v1);
            if (out == BOTTOM)
               Append(v2);
         }
      }
      else
      {
         if (mFirst)
         {
            (in == SIDE) ? Append(v1) : Append(v2);
            mFirst = false;
         }
         else
         {
            TurnCorner();
         }

         if (isRight)
         {
            if (in == BOTTOM)
               Append(v1);

            Append(v2);
            RightRefine(v2,rightVertex,!out,rightPlanes);
         }
         else //None
         {
            if (in == SIDE)
            {
               Append(v2);
               if (out == SIDE)
                  Append(v1);
            }
            else
            {
               Append(v1);
            }
         }
      }

      --mLevel;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::RightRefine(Vertex v1, Vertex &v2, bool out,
      unsigned int planes)
   {
      unsigned int levelSave = mLevel;
      Vertex right;
      bool isRight;

      do
      {
         ++mLevel;
         Append(v2);
         right.index = Index(v1.index,v2.index,(mLevel & 1),true);
         isRight = Active(right,planes);
         v1 = v2;
         v2 = right;
         out = !out;
      } while (isRight);

      if (out == BOTTOM)
         TurnCorner();
      mLevel = levelSave;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::LeftRefine(Vertex &v1, Vertex &v2, bool in,
      unsigned int planes)
   {
      ++mLevel;

      Vertex left(Index(v1.index,v2.index,(mLevel & 1), false));
      if (Active(left,planes))
      {
         LeftRefine(v2,left,!in,planes);
      }
      else
      {
         if (mFirst)
         {
            (in == SIDE) ? Append(v1) : Append(v2);
            mFirst = false;
         }
         else
         {
            TurnCorner();
         }

         if (in == SIDE)
            Append(v2);
      }

      Append(v1);
      --mLevel;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::TurnCorner()
   {
       if (mActiveIndexArray != NULL)
       {
          mActiveIndexArray[mIAIndex] = mActiveIndexArray[mIAIndex-2];
          mIAIndex++;
       }
   }

   //////////////////////////////////////////////////////////////////////////
   bool SoarXDrawable::Active(Vertex &v, unsigned int &planes)
   {
      static unsigned int clipPlanes[] = { 1,2,4,8,16,32 };
      const int RADIUS_BOOST = 2000; //Ad hoc constant for lazy frustum culling.
      const float INV_EMBEDDED_SIZE = 1.0f / (float)mEmbeddedSize;
      int i;
      float dist,proj;

      //Reject vertices after max level has been reached.
      if ((mLevel+1) > mMapLevels)
         return false;

      //Calculate real 2D position of the vertex.
      v.position.x() = mDetailHorizontalResolution * v.index.x;
      v.position.y() = mDetailHorizontalResolution * v.index.y;

      //Vertices below a certain threshold are not clipped.
      bool baseVertex = (mLevel + 1) < mBaseLevels;
      if (baseVertex)
      {
         Index baseIndex = v.index;
         baseIndex >>= mMapBits - mBaseBits;

         int iBase;
         RawData v0;

         iBase = (baseIndex.y << mBaseBits) + baseIndex.y + baseIndex.x;
         v0 = mBaseRawData[iBase];

         v.position.z() = v0.height;
         v.radius = v0.radius;
         v.error = v0.error;

         //Avoid culling if bounding sphere is already outside of the frustum.
         if (planes != 0)
         {
            float radius = v.radius + RADIUS_BOOST;

            //Check against each frustum plane that has not already been
            //disreguarded.
            for (i=0; i<6; i++)
            {
               if (planes & clipPlanes[i])
               {
                  float distance = mFrustumPlanes[i].distance(v.position);
                  if (distance < -radius)
                     return false;
                  else if (distance > radius)
                     planes ^= clipPlanes[i];
               }
            }
         }

         //Now check the projection detail-error for both real and detail vertices.
         osg::Vec3 delta = mEyePoint - v.position;
         dist = (delta.x() * delta.x()) + (delta.y() * delta.y());

         proj = mRadii[mLevel+1];
         if (proj > dist)
            return true;

         //If its not accepted then check the real projected error.
         dist += (delta.z() * delta.z());
         proj = (mMagic*v.error) + v.radius;
         proj = proj*proj;

         return proj > dist;
      }
      else
      {
         //The vertex is detail geometry.
         float dx,dy;

         dx = mEyePoint.x() - v.position.x();
         dy = mEyePoint.y() - v.position.y();

         dist = (dx*dx) + (dy*dy);
         proj = mRadii[mLevel+1];
         if (proj > dist)
         {
            Index baseIndex = v.index;
            Index detailIndex = v.index;
            Index interpolateIndex = v.index;

            baseIndex >>= mMapBits - mBaseBits;
            detailIndex &= mDetailSize-1;

            int iBase = (baseIndex.y << mBaseBits) + baseIndex.y + baseIndex.x;
            int iDetail = (detailIndex.y << mDetailBits) + detailIndex.x;
            interpolateIndex &= mEmbeddedSize - 1;

            dx = interpolateIndex.x * INV_EMBEDDED_SIZE;
            dy = interpolateIndex.y * INV_EMBEDDED_SIZE;

            RawData v0 = mBaseRawData[iBase];
            RawData v1 = mBaseRawData[iBase+1];
            RawData v2 = mBaseRawData[iBase+mBaseSize];
            RawData v3 = mBaseRawData[iBase+mBaseSize+1];
            float d0 = mDetailNoise[iDetail];

            v.position.z() = MathUtils::Lerp(dy,
                                 MathUtils::Lerp(dx,v0.height,v1.height),
                                 MathUtils::Lerp(dx,v2.height,v3.height)) +
                             MathUtils::Lerp(dy,
                                 MathUtils::Lerp(dx,v0.scale,v1.scale),
                                 MathUtils::Lerp(dx,v2.scale,v3.scale)) * d0;

            return true;
         }
         else
         {
            return false;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool SoarXDrawable::RestoreDataFromCache(const PagedTerrainTile &tile)
   {
      bool readResult = true;
      std::ifstream inFile;
      std::string cachePath = tile.GetCachePath() + "/" +
         SoarXCacheResourceName::VERTEX_DATA.GetName();

      //Read the cached precomputed vertex data and precomputed noise data.
      //If one of them is missing, it will be calculated when this method returns.
      inFile.open(cachePath.c_str(),std::ios::in | std::ios::binary);
      if (!inFile.is_open())
      {
         LOG_INFO("Vertex data not found in tile cache: " + cachePath);
         readResult = false;
      }
      else
      {
         int baseSize;
         inFile.read((char *)&baseSize,sizeof(int));
         if (baseSize != mBaseSize)
         {
            LOG_INFO("Cache data mismatch in vertex data. Base terrain sizes are not "
               "equal.");
            readResult = false;
         }
         else
         {
            mBaseRawData = new RawData[mBaseSize*(mBaseSize+1)];
            inFile.read((char *)&mBaseRawData[0],sizeof(RawData)*mBaseSize*(mBaseSize+1));
         }

         inFile.close();
      }

      return readResult;
   }

   //////////////////////////////////////////////////////////////////////////
   void SoarXDrawable::WriteDataToCache(const PagedTerrainTile &tile)
   {
      std::ofstream outFile;
      std::string cachePath = tile.GetCachePath() + "/" +
         SoarXCacheResourceName::VERTEX_DATA.GetName();

      //First write out the precomputed vertex data.
      outFile.open(cachePath.c_str(),std::ios::out | std::ios::trunc | std::ios::binary);
      if (!outFile.is_open())
      {
         LOG_ERROR("Unable to cache drawable vertex data to: " + cachePath);
         return;
      }

      outFile.write((char *)&mBaseSize,sizeof(int));
      outFile.write((char *)&mBaseRawData[0],sizeof(RawData)*mBaseSize*(mBaseSize+1));
      outFile.close();
   }

}
