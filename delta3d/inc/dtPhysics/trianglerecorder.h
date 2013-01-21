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
#include <vector>

namespace dtPhysics
{

   class DT_PHYSICS_EXPORT TriangleRecorder
   {
      public:

         TriangleRecorder();
         ~TriangleRecorder();

         void Record(const osg::Node& node);

         std::vector<VectorType> mVertices;
         std::vector<unsigned> mIndices;

         const MatrixType& GetMatrix() const;
         void SetMatrix(const MatrixType& m);

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
         bool mMatrixIsIdentity;
   };

}

#endif  // _PHYSICS_GLOBALS_H_
