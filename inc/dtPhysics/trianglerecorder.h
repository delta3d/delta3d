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
#include <osg/Geode>
#include <vector>

namespace dtPhysics
{

   class DT_PHYSICS_EXPORT TriangleRecorder
   {
   public:

      enum Mode
      {
         COMBINED, // Combine all geometries into one triangle data mesh
         PER_GEODE // Separate vertex data object per geode.
      };

      TriangleRecorder();
      ~TriangleRecorder();

      typedef dtUtil::Functor<dtPhysics::MaterialIndex, TYPELIST_1(const std::string&)> MaterialLookupFunc;
      typedef dtUtil::Functor<std::string, TYPELIST_1(const std::string&)> MaterialNameFilterFunc;

      /**
       * Records all the triangles in the buffers on this object for the given node.
       * @param node The node to traverse.
       * @param maxEdgeSize  The largest size of a triangle edge before the code will split the triangle in half recursively.
       *                     Large triangles can give physics engine trouble.
       * @param materialLookup A functor to use map the descriptions in the nodes to physics material id's.  It will store this
       *                       data on the triangles.
       */
      void Record(const osg::Node& node, Real maxEdgeLength = -1, MaterialLookupFunc materialLookup = MaterialLookupFunc());

      typedef std::map<osg::Vec3, int> VertexMap;

      VertexMap mVertIndexSet;

      typedef std::vector<dtCore::RefPtr<dtPhysics::VertexData> > VertexDataArray;

      VertexDataArray mData;

      const MatrixType& GetMatrix() const;
      void SetMatrix(const MatrixType& m);

      DT_DECLARE_ACCESSOR(dtPhysics::MaterialIndex, CurrentMaterial);
      DT_DECLARE_ACCESSOR(std::string, CurrentMaterialName);
      /**
       * If this is set, then only geodes that pass the pattern will work.
       * It supports * and ? for matching.
       * Empty string, the default, will accept any node.
       */
      DT_DECLARE_ACCESSOR_INLINE(std::string, PhysicsNodeNamePattern);
      DT_DECLARE_ACCESSOR(float, MaxEdgeLength);
      DT_DECLARE_ACCESSOR(Mode, Mode);
      DT_DECLARE_ACCESSOR(size_t, MaxSizePerBuffer);
      DT_DECLARE_ACCESSOR(size_t, GeodeCount);
      DT_DECLARE_ACCESSOR(bool, HideGeodes);

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

      /**
       * This is the split operator.
       * @return true if the geode should be processed.
       */
      bool operator()(osg::Geode& g);
   private:
      MatrixType mMatrix;
      int mSplitCount;
      int mReuseCount;
      bool mMatrixIsIdentity;
   };

}

#endif  // _PHYSICS_GLOBALS_H_
