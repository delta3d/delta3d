/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2008 MOVES Institute 
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
* Michael Guerrero
*/

#ifndef _POSE_MESH_H_
#define _POSE_MESH_H_

#include "export.h"

#include <vector>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <map>
#include <string>

#include <dtUtil/export.h>
#include <dtUtil/barycentric.h>

#define TRIANGLE_NOT_FOUND -1

namespace dtAnim 
{ 
   class Cal3DModelWrapper; 
   struct PoseMeshData;

   class DT_ANIM_EXPORT PoseMesh
   {
   public:

      struct DT_ANIM_EXPORT TargetTriangle
      {
         bool  mIsInside;
         int   mTriangleID;
         float mAzimuth;
         float mElevation;
      };   

      struct DT_ANIM_EXPORT Vertex
      {
         Vertex(const osg::Vec3& data, unsigned int animID);

         osg::Vec3 mData;
         unsigned int mAnimID; 

         float     mDebugPrecision;
         osg::Vec3 mDebugData;
         osg::Quat mDebugRotation;
         
      };

      struct DT_ANIM_EXPORT Triangle
      {
         Triangle(const Vertex* a, const Vertex* b, const Vertex* c,
            unsigned short aIndex, unsigned short bIndex, unsigned short cIndex);

         // 3 pointers to the vertex points
         const Vertex* mVertices[3];
         unsigned short mIndices[3];
      };

      typedef std::pair<unsigned short, unsigned short> MeshIndexPair;

      struct DT_ANIM_EXPORT TriangleEdge
      {
         TriangleEdge(){}
         TriangleEdge(const MeshIndexPair edge, const int triangleIndex)
            : mEdge(edge), mTriangleID(triangleIndex){}

         MeshIndexPair mEdge;
         int mTriangleID;
      };

      typedef std::vector<Vertex*>                VertexVector;
      typedef dtUtil::BarycentricSpace<osg::Vec3> Barycentric2D;
      typedef std::vector<Barycentric2D*>         Barycentric2DVector;
      typedef std::vector<Triangle>               TriangleVector;
      typedef std::vector<TriangleEdge>           TriangleEdgeVector;
      typedef std::vector<std::string>            StringVector;

      typedef std::map<MeshIndexPair, osg::ref_ptr<osg::Geometry> > EdgeLineMap;

      PoseMesh(dtAnim::Cal3DModelWrapper* model,
               const PoseMeshData& meshData);

      ~PoseMesh();

      const std::string& GetName() const                 { return mName;            }
      const std::string& GetEffectorName() const         { return mBoneName;        }
      int GetEffectorID() const                          { return mEffectorID;      }
      const VertexVector& GetVertices() const            { return mVertices;        }
      const Barycentric2DVector& GetBarySpaces() const   { return mBarySpaces;      }
      const TriangleVector& GetTriangles() const         { return mTriangles;       }
      const TriangleEdgeVector GetSilhouette() const     { return mSilhouetteEdges; }
      const osg::Vec3& GetRootForwardAxis() const        { return mRootForward;     }
      const osg::Vec3& GetEffectorForwardAxis() const    { return mEffectorForward; }

      /**  
      *  GetTargetTriangleData Finds the triangle in the mesh for the given azimuth elevation 
      *                        if it exists, otherwise it returns the closest triangle and its coordinates
      *  @param azimuth the horizontal angle between our forward and our target
      *  @param elevation the vertical angle between our forward and our target  
      *  @return outTriangle struct containing the nearest triangle and it's location
      */
      void GetTargetTriangleData(const float azimuth,
                                 const float elevation,
                                 TargetTriangle& outTriangle) const;

      /**
      *  FindCelestialTriangleID Looks up a celestial triangle from a mesh using azimuth and elevation
      *  @param azimuth the horizontal angle of interest
      *  @param elevation the vertical angle of interest
      */
      int FindPoseTriangleID(float azimuth, float elevation) const;

      /**
      * GetIndexPairsForTriangle Look up the indices for a triangle and 
      *                          create pairs corresponding to its edges
      * @param triangleID the triangle to extract pairs from
      * @param pair0 the first pair(edge)
      * @param pair1 the second pair(edge)
      * @param pair2 the thris pair(edge)
      */
      void GetIndexPairsForTriangle(int triangleID, 
                                    MeshIndexPair& pair0,
                                    MeshIndexPair& pair1,
                                    MeshIndexPair& pair2) const;

   private:

      std::string mName;
      std::string mBoneName;

      int mRootID;
      int mEffectorID;

      osg::Vec3 mRootForward;
      osg::Vec3 mEffectorForward;

      TriangleVector       mTriangles;
      TriangleEdgeVector   mSilhouetteEdges;

      VertexVector mVertices;

      /// the number of spaces is equal to the number of Triangles.
      Barycentric2DVector mBarySpaces;  

      // the model should be made const later
      void GetAnimationIDsByName(const dtAnim::Cal3DModelWrapper* model,
                                 const std::vector<std::string>& animNames,
                                 std::vector<unsigned int>& animIDs) const;
   };
}

#endif // _POSE_MESH_H_
