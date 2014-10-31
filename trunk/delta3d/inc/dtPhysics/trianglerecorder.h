/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology, BMH Operation
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
 * David Guthrie
 * Allen Danklefsen
 */

#ifndef DELTA_TRIANGLE_RECORDER_H
#define DELTA_TRIANGLE_RECORDER_H

#include <dtPhysics/physicsexport.h>
#include <dtPhysics/physicstypes.h>
#include <dtPhysics/physicsreaderwriter.h>
#include <dtPhysics/physicsmaterials.h>
#include <dtPhysics/geometry.h>
#include <dtUtil/functor.h>
#include <dtUtil/getsetmacros.h>
#include <vector>

namespace dtPhysics
{

   class DT_PHYSICS_EXPORT TriangleRecorder
   {
      public:

         TriangleRecorder();
         ~TriangleRecorder();

         typedef dtUtil::Functor<dtPhysics::MaterialIndex, TYPELIST_1(const std::string&)> MaterialLookupFunc;

         /**
          * Records all the triangles in the buffors on this object for the given node.
          * @param node The node to traverse.
          * @param maxEdgeSize  The largest size of a triangle edge before the code will split the triangle in half recursively.
          *                     Large triangles can give physics engine trouble.
          * @param materialLookup A functor to use map the descriptions in the nodes to physics material id's.  It will store this
          *                       data on the triangles.
          */
         void Record(const osg::Node& node, Real maxEdgeLength = -1, MaterialLookupFunc materialLookup = MaterialLookupFunc());

         typedef std::map<osg::Vec3, int> VertexMap;

         VertexMap mVertIndexSet;
         dtCore::RefPtr<VertexData> mData;

         const MatrixType& GetMatrix() const;
         void SetMatrix(const MatrixType& m);

         DT_DECLARE_ACCESSOR(dtPhysics::MaterialIndex, CurrentMaterial);
         DT_DECLARE_ACCESSOR(float, MaxEdgeLength);

         /**
          * Called once for each visited triangle.
          *
          * @param v1 the triangle's first vertex
          * @param v2 the triangle's second vertex
          * @param v3 the triangle's third vertex
          * @param treatVertexDataAsTemporary whether or not to treat the vertex data
          * as temporary
          */
         void operator()(const VectorType& v1,
                  const VectorType& v2,
                  const VectorType& v3,
                  bool treatVertexDataAsTemporary);
      private:
         MatrixType mMatrix;
         int mSplitCount;
         int mReuseCount;
         bool mMatrixIsIdentity;
   };

}

#endif  // _PHYSICS_GLOBALS_H_
