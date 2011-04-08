#include <dtAnim/PoseMesh.h>
#include <dtAnim/PoseMath.h>
#include <dtAnim/PoseMeshXML.h>

#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtAnim/cal3dmodelwrapper.h>

using namespace dtAnim;

/////////////////////////////////////////////////////////////////////////////////////////
PoseMesh::PoseMesh(const dtAnim::Cal3DModelWrapper* model,
                   const PoseMeshData& meshData)
  : mName(meshData.mName)
  , mBoneName(meshData.mBoneName)
{  
   std::vector<unsigned int> animids;
   GetAnimationIDsByName(const_cast<dtAnim::Cal3DModelWrapper*>(model), meshData.mAnimations, animids);
      
   mBoneID = model->GetCoreBoneID( meshData.mBoneName );  
   assert(mBoneID != -1);

   // Allocate space for osg to triangulate our verts
   std::vector<osg::Vec3> celestialPoints;

   typedef std::map<unsigned int, unsigned int> AnimVertMap;
   AnimVertMap vertMap;

   typedef std::vector<unsigned int> UIVector;
   UIVector::const_iterator endanim = animids.end();

   unsigned int vert_idx( 0 );

   for( UIVector::const_iterator anim = animids.begin(); anim != endanim; ++anim )
   {
      // If we've already handled this animation,
      // map it to the previously computed data
      AnimVertMap::iterator mapIter = vertMap.find( *anim );

      if ( mapIter != vertMap.end() )
      {
         vertMap[*anim] = mapIter->second;
         continue;
      }

      // This anim maps to this vert
      vertMap[*anim] = vert_idx;

      // frame 30 is a temp number intended to be the last or close to the last frame
      osg::Quat finalRotation = model->GetBoneAbsoluteRotationForKeyFrame( *anim, mBoneID, 30 );   

      // calculate a vector transformed by the rotation data.
      osg::Vec3 transformed = finalRotation * meshData.mForward;   

      // calculate the local azimuth and elevation for the transformed vector
      float az = 0.f;
      float el = 0.f;     

      osg::Vec3 pelvisForward(0, -1, 0);
      dtUtil::GetCelestialCoordinates( transformed, pelvisForward, az, el );

      std::ostringstream oss;
      oss << "Vert #" << vert_idx 
         << " (" << osg::RadiansToDegrees(az) << "," << osg::RadiansToDegrees(el) << ") (degs)"
         << "\t(anim=" << model->GetCoreAnimationName(*anim) << ")"
         << std::endl;

      LOG_DEBUG(oss.str());      

      // Store the vert for triangulation
      // - osg::PI_2
      osg::Vec3 newVertPoint( az, el, 0.f );

      celestialPoints.push_back( newVertPoint );

      // add a (az,el) vertex
      mVertices.push_back( new PoseMesh::Vertex( newVertPoint, *anim ) );

      ++vert_idx;
   }

   mTriangles.clear();   

   typedef std::map<PoseMesh::MeshIndexPair, std::pair<int, int> > EdgeCountMap;
   EdgeCountMap edgeCounts;

   // Populate the mesh with triangles
   for (unsigned int vertIndex = 0; vertIndex < animids.size(); vertIndex += 3)
   {
      int anim0 = animids.at(vertIndex + 0);
      int anim1 = animids.at(vertIndex + 1);
      int anim2 = animids.at(vertIndex + 2);

      int vertIndex0 = vertMap[anim0];
      int vertIndex1 = vertMap[anim1];
      int vertIndex2 = vertMap[anim2];

      PoseMesh::VertexVector::value_type vert0 = mVertices[vertIndex0];
      PoseMesh::VertexVector::value_type vert1 = mVertices[vertIndex1];
      PoseMesh::VertexVector::value_type vert2 = mVertices[vertIndex2];

      PoseMesh::Triangle newTri( vert0, vert1, vert2, vertIndex0, vertIndex1, vertIndex2 );
      mTriangles.push_back( newTri );

      // Debug
      unsigned int triIndex = vertIndex / 3;
      std::string animName0( model->GetCoreAnimationName( mVertices[vertIndex0]->mAnimID ) );
      std::string animName1( model->GetCoreAnimationName( mVertices[vertIndex1]->mAnimID ) );
      std::string animName2( model->GetCoreAnimationName( mVertices[vertIndex2]->mAnimID ) );

      // Tally the number of edges so that we can determine 
      // which ones are the silhouettes
      PoseMesh::MeshIndexPair pair0(vertIndex0, vertIndex1);
      PoseMesh::MeshIndexPair pair1(vertIndex1, vertIndex2);
      PoseMesh::MeshIndexPair pair2(vertIndex2, vertIndex0);

      ++edgeCounts[pair0].first;
      ++edgeCounts[pair1].first;
      ++edgeCounts[pair2].first;	 

      edgeCounts[pair0].second = triIndex;
      edgeCounts[pair1].second = triIndex;
      edgeCounts[pair2].second = triIndex;

      std::ostringstream oss;
      oss << "Triangle #" << triIndex << " contains (" << vertIndex0 << ", " << vertIndex1 <<
         ", " << vertIndex2 << ")" << "  (" << animName0 << ", " << animName1 << 
         ", " << animName2 << ")" << std::endl;

      LOG_DEBUG(oss.str());
   }  

   EdgeCountMap::iterator edgeIter = edgeCounts.begin(); 

   // Find all edges that belong to a single face and store them
   while (edgeIter != edgeCounts.end())
   {
      PoseMesh::MeshIndexPair edge = edgeIter->first;
      PoseMesh::MeshIndexPair swappedEdge(edge.second, edge.first);

      int total = edgeCounts[edge].first + edgeCounts[swappedEdge].first;
      assert(total != 0);

      // If we found an edge with 
      // only one face
      if (total == 1)
      {
         int triangleIndex = edgeIter->second.second;
         PoseMesh::TriangleEdge boundaryEdge(edge, triangleIndex);
         mSilhouetteEdges.push_back(boundaryEdge);

         // Mark this edge as done
         edgeIter->second.first = 69;
      }       

      edgeIter++;
   }

   ///\todo now also build the barycentric array
   unsigned int numpolygons = mTriangles.size();
   mBarySpaces.resize( numpolygons );
   for(unsigned int polygon=0; polygon<numpolygons; ++polygon)
   {
      const osg::Vec3& a = mTriangles[polygon].mVertices[0]->mData;
      const osg::Vec3& b = mTriangles[polygon].mVertices[1]->mData;
      const osg::Vec3& c = mTriangles[polygon].mVertices[2]->mData;
      mBarySpaces[polygon] = new PoseMesh::Barycentric2D(a,b,c);
   }  
}

/////////////////////////////////////////////////////////////////////////////////////////
PoseMesh::~PoseMesh()
{
   VertexVector::iterator endvert = mVertices.end();
   for(VertexVector::iterator vert=mVertices.begin(); vert!=endvert; ++vert)
   {
      delete *vert;
   }

   // the head data
   unsigned int numspaces;
   numspaces = mBarySpaces.size();
   for(unsigned int space=0; space<numspaces; ++space)
   {
      delete mBarySpaces[space];
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
PoseMesh::Vertex::Vertex(const osg::Vec3& data, unsigned int animID)
: mData(data)
, mAnimID(animID)
{
}

PoseMesh::Triangle::Triangle(const PoseMesh::Vertex* a,
                             const PoseMesh::Vertex* b,
                             const PoseMesh::Vertex* c,
                             unsigned short aIndex,
                             unsigned short bIndex,
                             unsigned short cIndex)
{
   mVertices[0] = a;
   mVertices[1] = b;
   mVertices[2] = c;

   mIndices[0] = aIndex;
   mIndices[1] = bIndex;
   mIndices[2] = cIndex;
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMesh::GetIndexPairsForTriangle(int triangleID,                                              
                                        MeshIndexPair &pair0,
                                        MeshIndexPair &pair1,
                                        MeshIndexPair &pair2) const
{
   assert(triangleID < (int)mTriangles.size());

   // Get the pointer to the verts that make up this tri
   unsigned short index0 = mTriangles[triangleID].mIndices[0];
   unsigned short index1 = mTriangles[triangleID].mIndices[1];
   unsigned short index2 = mTriangles[triangleID].mIndices[2];

   // Make 3 pairs of points to represent the 3 edges of the tri
   pair0 = MeshIndexPair(index0, index1);
   pair1 = MeshIndexPair(index1, index2);
   pair2 = MeshIndexPair(index2, index0);
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMesh::GetAnimationIDsByName(const dtAnim::Cal3DModelWrapper *model,
                                     const std::vector<std::string> &animNames,
                                     std::vector<unsigned int> &animIDs) const
{
   // Convert the animation string into a list of ID's           
   for (unsigned int animIndex = 0; animIndex < animNames.size(); ++animIndex)
   {
      int id = model->GetCoreAnimationIDByName( animNames[animIndex] );

      // The mesh is invalid if we cannot find it's animations
      if (id == -1)
      {
         std::string errorMessage("Unable to find animation id by name: ");
         errorMessage.append(animNames[animIndex]);

         throw dtUtil::Exception(errorMessage, __FILE__, __LINE__);
      }

      animIDs.push_back( id );
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMesh::GetTargetTriangleData(const float azimuth,
                                     const float elevation,                                                          
                                     TargetTriangle &outTriangle) const
{
   int triangleID = FindCelestialTriangleID( azimuth, elevation );

   // At this point, we know if we're in or out
   outTriangle.mIsInside = (triangleID != -1);

   // Find the closest triangle point
   if (triangleID == -1)
   {         
      osg::Vec3 closestPoint;
      int closestTriangleID;

      osg::Vec3 refPoint(azimuth, elevation, 0);
      float minDistance = FLT_MAX;        

      const PoseMesh::TriangleEdgeVector &silhouetteList = GetSilhouette();
      for (unsigned int edgeIndex = 0; edgeIndex < silhouetteList.size(); ++edgeIndex)
      {
         PoseMesh::MeshIndexPair edge = silhouetteList[edgeIndex].mEdge;
         const PoseMesh::VertexVector &vertices = GetVertices();

         osg::Vec3 startPoint = vertices[edge.first]->mData;
         osg::Vec3 endPoint   = vertices[edge.second]->mData;

         osg::Vec3 closestPointToCurrentEdge;

         dtUtil::GetClosestPointOnSegment(startPoint, endPoint, refPoint, closestPointToCurrentEdge);

         // We don't need exact distance, just a way too compare (this is faster)
         float distance = (refPoint - closestPointToCurrentEdge).length2();

         if (distance < minDistance)
         {
            minDistance       = distance;
            closestPoint      = closestPointToCurrentEdge;
            closestTriangleID = silhouetteList[edgeIndex].mTriangleID;
         }
      }

      outTriangle.mTriangleID = closestTriangleID;
      outTriangle.mAzimuth    = closestPoint.x();
      outTriangle.mElevation  = closestPoint.y();
      return;
   }      

   outTriangle.mTriangleID = triangleID;
   outTriangle.mAzimuth    = azimuth;
   outTriangle.mElevation  = elevation;		
}

/////////////////////////////////////////////////////////////////////////////////////////
/// Algorithm in detail at http://www.blackpawn.com/texts/pointinpoly/default.html
int PoseMesh::FindCelestialTriangleID(float azimuth, float elevation) const
{
   const PoseMesh::TriangleVector &triangles = GetTriangles();
   unsigned int numTris = triangles.size();

   osg::Vec3f point(azimuth, elevation, 0.0f);

   int animationIndex = TRIANGLE_NOT_FOUND;

   for (unsigned int triIndex = 0; triIndex < numTris; ++triIndex)
   {
      const osg::Vec3 &A = triangles[triIndex].mVertices[0]->mData;
      const osg::Vec3 &B = triangles[triIndex].mVertices[1]->mData;
      const osg::Vec3 &C = triangles[triIndex].mVertices[2]->mData;

      if (!dtUtil::IsPointBetweenVectors(point, A, B, C)) { continue; }
      if (!dtUtil::IsPointBetweenVectors(point, B, A, C)) { continue; }      

      animationIndex = triIndex;
      break;
   }

   return animationIndex;
}

