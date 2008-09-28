/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
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
* John Grant & Michael Guerrero
*/

#ifndef _pd_celestial_mesh_h_
#define _pd_celestial_mesh_h_

#include <vector>
#include <osg/Vec3>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <map>
#include <string>

#include <dtUtil/export.h>
#include <dtUtil/barycentric.h>

namespace dtUtil
{
   struct DT_UTIL_EXPORT CelestialMesh
   {
      struct DT_UTIL_EXPORT Vertex
      {
         Vertex(const osg::Vec3& azel, unsigned int animID);

         osg::Vec3 mMaxAzMaxEl;
         unsigned int mAnimID;

      private:
         /// not implemented by design
         Vertex();  
      };

      struct DT_UTIL_EXPORT Triangle
      {
         Triangle(const Vertex* a, const Vertex* b, const Vertex* c,
                  unsigned short aIndex, unsigned short bIndex, unsigned short cIndex);

         // 3 pointers to the vertex points
         const Vertex* mVertices[3];
         unsigned short mIndices[3]; 

      private:
         /// not implemented by design
         Triangle();  
      };

      typedef std::pair<unsigned short, unsigned short> MeshIndexPair;

      struct DT_UTIL_EXPORT TriangleEdge
      {
         TriangleEdge(){}
         TriangleEdge(const MeshIndexPair edge, const int triangleIndex)
            : mEdge(edge), mTriangleID(triangleIndex){}

         MeshIndexPair mEdge;
         int mTriangleID;
      };

      typedef std::vector<Triangle>               TriangleVector;
      typedef std::vector<TriangleEdge>           TriangleEdgeVector;
      typedef std::vector<Vertex*>                VertexVector;
      typedef dtUtil::BarycentricSpace<osg::Vec3> Barycentric2D;
      typedef std::vector<Barycentric2D*>         Barycentric2DVector;


      CelestialMesh();
      ~CelestialMesh();

      std::string mName;
      std::string mBoneName;

      TriangleVector       mTriangles;
      TriangleEdgeVector   mSilhouetteEdges;

      VertexVector mVertices;

      /// the number of spaces is equal to the number of "celestial" Triangles.
      Barycentric2DVector mBarySpaces;  

      // Debug --------------------------------------------------
      typedef std::map<MeshIndexPair, osg::ref_ptr<osg::Geometry> > EdgeLineMap;

      EdgeLineMap mEdgeLines;
      std::vector<osg::Vec3> mAnimDirections;    
      osg::ref_ptr<osg::Geometry> mDebugPoint;

      // Used for point visual
      float mMaxDistance;
      osg::Vec2 mScreenOrigin;

      // Look up the indices for a triangle and 
      // create pairs corresponding to its edges
      void GetIndexPairsForTriangle(int triangleID,                                     
                                    MeshIndexPair &pair0,
                                    MeshIndexPair &pair1,
                                    MeshIndexPair &pair2);     
      // --------------------------------------------------------

      
   };
}

#endif // _pd_celestial_mesh_h_
