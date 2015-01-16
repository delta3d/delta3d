/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include <dtPhysics/convexhull.h>

#include <pal_i/hull.h>

#include <map>

namespace dtPhysics
{

   ////////////////////////////////////////////////////////////
   ConvexHull::ConvexHull(VertexData& meshData, unsigned margin)
   : mNewVertexData(new VertexData)
   {

      HullDesc desc;
      desc.SetHullFlag(QF_TRIANGLES);
      desc.mVcount       = meshData.mVertices.size();
      desc.mVertices     = new double[desc.mVcount*3];

      for (unsigned i = 0; i < desc.mVcount; ++i)
      {
         unsigned hullIdx = i*3;
         desc.mVertices[hullIdx] = meshData.mVertices[i][0];
         desc.mVertices[hullIdx+1] = meshData.mVertices[i][1];
         desc.mVertices[hullIdx+2] = meshData.mVertices[i][2];
      }

      desc.mVertexStride = sizeof(double)*3;
      desc.mMaxVertices = margin;

      HullResult dresult;
      HullLibrary hl;
      //HullError ret =
      hl.CreateConvexHull(desc,dresult);

      mNewVertexData->mIndices.resize(dresult.mNumIndices);
      mNewVertexData->mVertices.resize(dresult.mNumOutputVertices);

      for (unsigned i = 0; i < dresult.mNumIndices; ++i)
      {
         mNewVertexData->mIndices[i] = dresult.mIndices[i];
      }

      for (unsigned i = 0; i < dresult.mNumOutputVertices; ++i)
      {
         unsigned resultIdx = i*3;
         mNewVertexData->mVertices[i] = VectorType(dresult.mOutputVertices[resultIdx], dresult.mOutputVertices[resultIdx+1], dresult.mOutputVertices[resultIdx+2]);
      }

      hl.ReleaseResult(dresult);
   }

   ////////////////////////////////////////////////////////////
   ConvexHull::~ConvexHull()
   {
   }

   ////////////////////////////////////////////////////////////
   void ConvexHull::GetVertexData(VertexData& v)
   {
      v.mIndices.swap(mNewVertexData->mIndices);
      v.mVertices.swap(mNewVertexData->mVertices);
      v.mMaterialFlags.swap(mNewVertexData->mMaterialFlags);
      v.SwapMaterialTable(*mNewVertexData);
   }

   ////////////////////////////////////////////////////////////
   ConvexHull::ConvexHull(const ConvexHull&)
   {}

   ////////////////////////////////////////////////////////////
   ConvexHull& ConvexHull::operator=(const ConvexHull&)
   { return *this; }


}
