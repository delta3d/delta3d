#include <prefix/dtutilprefix-src.h>
#include <dtUtil/celestialmesh.h>

using namespace dtUtil;

CelestialMesh::Vertex::Vertex(const osg::Vec3& azel, unsigned int animID)
   : mMaxAzMaxEl(azel)
   , mAnimID(animID)
{
}

CelestialMesh::Triangle::Triangle(const CelestialMesh::Vertex* a,
                                  const CelestialMesh::Vertex* b,
                                  const CelestialMesh::Vertex* c,
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

void CelestialMesh::GetIndexPairsForTriangle(int triangleID,                                              
                                             MeshIndexPair &pair0,
                                             MeshIndexPair &pair1,
                                             MeshIndexPair &pair2)
{
   // Get the pointer to the verts that make up this tri
   unsigned short index0 = mTriangles[triangleID].mIndices[0];
   unsigned short index1 = mTriangles[triangleID].mIndices[1];
   unsigned short index2 = mTriangles[triangleID].mIndices[2];

   // Make 3 pairs of points to represent the 3 edges of the tri
   pair0 = MeshIndexPair(index0, index1);
   pair1 = MeshIndexPair(index1, index2);
   pair2 = MeshIndexPair(index2, index0);
}

CelestialMesh::CelestialMesh()
{
}

CelestialMesh::~CelestialMesh()
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

