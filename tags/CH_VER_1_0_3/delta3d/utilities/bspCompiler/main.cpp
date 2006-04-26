#include <algorithm>
#include <iostream>
#include <list>

#include <osg/BoundingSphere>
#include <osg/Drawable>
#include <osg/FrontFace>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/NodeVisitor>
#include <osg/Plane>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Transform>
#include <osg/TriangleFunctor>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgUtil/Optimizer>
#include <osgUtil/TriStripVisitor>

#include <dtBSP/bspcullcallback.h>
#include "splitgeode.h"

using namespace dtBSP;
using namespace osg;
using namespace osgDB;
using namespace osgUtil;
using namespace std;

/**
 * A triangle obtained from the scene graph.
 */
struct Triangle
{
   /**
    * The vertices of the triangle.
    */
   Vec3 mVertices[3];
   
   /**
    * The Geode from which the triangle was obtained.
    */
   osg::ref_ptr<Geode> mGeode;
};


/**
 * Records visited triangles into an array.
 */
class TriangleRecorder
{
   public:

      /**
       * The triangles that can become part of the BSP tree.
       */
      vector<Triangle> mTriangles;
      
      /**
       * The triangles that cannot become part of the BSP tree.
       */
      vector<Triangle> mDetailTriangles;
      
      /**
       * True if the active drawable is detail geometry.
       */
      bool mDetail;
      
      /**
       * The active geode.
       */
      osg::ref_ptr<Geode> mActiveGeode;
      
      /**
       * The aggregate transformation matrix.
       */   
      Matrix mMatrix;
      
      
      /**
       * Called once for each visited triangle.
       *
       * @param v1 the triangle's first vertex
       * @param v2 the triangle's second vertex
       * @param v3 the triangle's third vertex
       * @param treatVertexDataAsTemporary whether or not to treat the vertex data
       * as temporary
       */
      void operator()(const Vec3& v1,
                      const Vec3& v2,
                      const Vec3& v3,
                      bool treatVertexDataAsTemporary)
      {
         Triangle t;
           
         t.mVertices[0] = v1*mMatrix;
         t.mVertices[1] = v2*mMatrix;
         t.mVertices[2] = v3*mMatrix;
         
         t.mGeode = mActiveGeode;
         
         if(mDetail)
         {
            mDetailTriangles.push_back(t);
         }
         else
         {
            mTriangles.push_back(t);
         }
      }
};

/**
 * A visitor that collects all triangles from a scene graph.
 */
class TriangleVisitor : public NodeVisitor
{
   public:
   
      /**
       * The triangle recorder object.
       */
      TriangleFunctor<TriangleRecorder> mRecorder;
      
      
      /**
       * Constructor.
       */
      TriangleVisitor()
         : NodeVisitor(TRAVERSE_ACTIVE_CHILDREN)
      {
         mRecorder.mDetail = false;
      }
      
      /**
       * Applies this visitor to a node.
       *
       * @param node the node to visit
       */
      virtual void apply(Node& node)
      {
         bool prevDetail = mRecorder.mDetail;
         
         if(node.getName().find("/dynamic") != string::npos)
         {
            return;
         }
         else if(node.getName().find("/detail") != string::npos)
         {
            mRecorder.mDetail = true;
         }
         
         NodeVisitor::apply(node);
         
         mRecorder.mDetail = prevDetail;
      }
      
      /**
       * Applies this visitor to a geode.
       *
       * @param node the geode to visit
       */
      virtual void apply(Geode& node)
      {
         mRecorder.mMatrix.makeIdentity();
         
         NodePath np = getNodePath();
               
         for(NodePath::iterator it = np.begin();
             it != np.end();
             it++)
         {
            Transform* transform = dynamic_cast<Transform*>(*it);
            
            if(transform != NULL)
            {
               transform->computeLocalToWorldMatrix(mRecorder.mMatrix, this);
            }
         }
         
         mRecorder.mActiveGeode = &node;
         
         for(unsigned int i=0;i<node.getNumDrawables();i++)
         {
            Drawable* d = node.getDrawable(i);

            if(d->supports(mRecorder))
            {
               d->accept(mRecorder);
            }
         }
      }
};

/**
 * A small value, used for determining whether a point lies on
 * a plane.
 */
const float epsilon = 0.0001f;

/**
 * Splits a line segment across a plane, returning a new vertex that
 * lies on the plane between the two vertices.
 *
 * @param v1 the first vertex of the segment
 * @param v2 the second vertex of the segment
 * @param plane the plane
 * @return the new vertex
 */
Vec3 SplitLineSegment(const Vec3& v1, const Vec3& v2, const Plane& plane)
{
   float t = (-plane[0]*v1[0] - plane[1]*v1[1] - plane[2]*v1[2] - plane[3]) /
             (plane[0]*(v2[0]-v1[0]) + plane[1]*(v2[1]-v1[1]) + plane[2]*(v2[2]-v1[2]));
   
   return v1 + (v2-v1)*t;
}

/**
 * The result of a plane/polygon intersection test.
 */
enum PlanePolygonIntersectResult
{
   AbovePlane,
   BelowPlane,
   IntersectsPlane,
   OnPlane
};

/**
 * The result of a triangle/portal intersection test.
 */
enum TrianglePortalIntersectResult
{
   NotObscured,
   PartiallyObscured,
   CompletelyObscured
};

struct WorkingBSPInternalNode;
struct WorkingBSPLeafNode;

/**
 * Represents a portal between BSP leaves.
 */
struct Portal
{
   /**
    * The vertices of the portal.
    */
   vector<Vec3> mVertices;
   
   /**
    * The portal plane.
    */
   Plane mPlane;
   
   /**
    * The internal node that contains this portal.
    */
   WorkingBSPInternalNode* mInternalNode;
   
   /**
    * The leaves below this portal.
    */
   set<WorkingBSPLeafNode*> mLeftLeaves;
   
   /**
    * The leaves above this portal.
    */
   set<WorkingBSPLeafNode*> mRightLeaves;
   
   /**
    * Flags this portal as internal for purposes of grouping leaves.
    */
   bool mInternal;
   
   /**
    * Visited flag, used for searching.
    */
   bool mVisited;
   
   /**
    * The set of possibly visible portals.
    */
   set<Portal*> mPossiblyVisiblePortals;
   
   
   /**
    * Default constructor.
    */
   Portal()
      : mInternal(false), mVisited(false)
   {}
   
   /**
    * Test the intersection of this portal against a plane.
    *
    * @param plane the plane to test against
    * @return the result of the intersection test
    */
   PlanePolygonIntersectResult Intersect(const Plane& plane)
   {
      bool above = false, below = false;
      
      for(vector<Vec3>::iterator it = mVertices.begin();
          it != mVertices.end();
          it++)
      {
         float dist = plane.distance(*it);
         
         if(dist < -epsilon)
         {
            below = true;
         }
         else if(dist > +epsilon)
         {
            above = true;
         }
      }
      
      if(above && below)
      {
         return IntersectsPlane;
      }
      else if(above && !below)
      {
         return AbovePlane;
      }
      else if(below && !above)
      {
         return BelowPlane;
      }
      else
      {
         return OnPlane;
      }
   }
   
   /**
    * Tests the planar intersection of a portal against a triangle.
    *
    * @param triangle the triangle to test against
    * @return the result of the intersection test
    */
   TrianglePortalIntersectResult Intersect(const Triangle& triangle)
   {
      Plane trianglePlane(
         triangle.mVertices[0],
         triangle.mVertices[1],
         triangle.mVertices[2]
      );
      
      if(!equivalent(-mPlane[0], trianglePlane[0], epsilon) ||
         !equivalent(-mPlane[1], trianglePlane[1], epsilon) ||
         !equivalent(-mPlane[2], trianglePlane[2], epsilon) ||
         !equivalent(-mPlane[3], trianglePlane[3], epsilon))
      {
         return NotObscured;
      }
      
      unsigned int i, j;
      
      Plane abPlane(
         triangle.mVertices[1], 
         triangle.mVertices[0],
         triangle.mVertices[0] - mPlane.getNormal()
      );
      
      Plane bcPlane(
         triangle.mVertices[2],
         triangle.mVertices[1],
         triangle.mVertices[1] - mPlane.getNormal()
      );
      
      Plane caPlane(
         triangle.mVertices[0],
         triangle.mVertices[2],
         triangle.mVertices[2] - mPlane.getNormal()
      );
      
      unsigned int portalVerticesWithinTriangle = 0;
         
      for(i=0;i<mVertices.size();i++)
      {
         if(abPlane.distance(mVertices[i]) > -epsilon &&
            bcPlane.distance(mVertices[i]) > -epsilon &&
            caPlane.distance(mVertices[i]) > -epsilon)
         {
            portalVerticesWithinTriangle++;
         }
      }
      
      if(portalVerticesWithinTriangle == mVertices.size())
      {
         return CompletelyObscured;
      }
      else if(portalVerticesWithinTriangle > 0)
      {
         return PartiallyObscured;
      }
          
      int triangleVerticesWithinPortal = 0;
      
      vector<Plane> edgePlanes;
      
      for(i=0;i<mVertices.size();i++)
      {
         j = (i+1)%mVertices.size();
         
         Plane edgePlane(
            mVertices[j],
            mVertices[i],
            mVertices[i] + mPlane.getNormal()
         );
         
         if(edgePlane.getNormal().length() > epsilon)
         {
            edgePlanes.push_back(edgePlane);
         }
      }
      
      for(i=0;i<3;i++)
      {
         for(j=0;j<edgePlanes.size();j++)
         {
            if(edgePlanes[j].distance(triangle.mVertices[i]) <= -epsilon)
            {
               break;
            }
         }
         
         if(j==edgePlanes.size())
         {
            triangleVerticesWithinPortal++;
         }
      }
      
      if(triangleVerticesWithinPortal == 0)
      {
         return NotObscured;
      }
      else
      {
         return PartiallyObscured;
      }
   }

   /**
    * Splits this portal across the specified plane.
    *
    * @param plane the plane to split this portal across
    * @param abovePortals the portals above the plane
    * @param belowPortals the portals below the plane
    */
   void Split(const Plane& plane,
              vector<Portal>& abovePortals,
              vector<Portal>& belowPortals)
   {
      vector<Vec3> belowVertices, aboveVertices;
      
      unsigned int i, j;
      
      for(i=0;i<mVertices.size();i++)
      {
         j = (i+1)%mVertices.size();
         
         float iDist = plane.distance(mVertices[i]),
               jDist = plane.distance(mVertices[j]);
         
         if(iDist < -epsilon)
         {
            belowVertices.push_back(mVertices[i]);
            
            if(jDist > +epsilon)
            {
               Vec3 newVertex = SplitLineSegment(
                  mVertices[i],
                  mVertices[j],
                  plane
               );
               
               belowVertices.push_back(newVertex);
               aboveVertices.push_back(newVertex);
            }
         }
         else if(iDist > +epsilon)
         {
            aboveVertices.push_back(mVertices[i]);
            
            if(jDist < -epsilon)
            {
               Vec3 newVertex = SplitLineSegment(
                  mVertices[i],
                  mVertices[j],
                  plane
               );
               
               aboveVertices.push_back(newVertex);
               belowVertices.push_back(newVertex);
            }
         }
         else
         {
            belowVertices.push_back(mVertices[i]);
            aboveVertices.push_back(mVertices[i]);
         }
      }
      
      Portal abovePortal, belowPortal;
      
      abovePortal.mVertices = aboveVertices;
      belowPortal.mVertices = belowVertices;
      
      abovePortal.mPlane = mPlane;
      belowPortal.mPlane = mPlane;
      
      abovePortal.mInternalNode = mInternalNode;
      belowPortal.mInternalNode = mInternalNode;
      
      abovePortal.mLeftLeaves = mLeftLeaves;
      belowPortal.mLeftLeaves = mLeftLeaves;
      
      abovePortal.mRightLeaves = mRightLeaves;
      belowPortal.mRightLeaves = mRightLeaves;
      
      abovePortal.mPossiblyVisiblePortals = mPossiblyVisiblePortals;
      belowPortal.mPossiblyVisiblePortals = mPossiblyVisiblePortals;
      
      abovePortals.push_back(abovePortal);
      belowPortals.push_back(belowPortal);
   }
   
   /**
    * Clips this portal against a triangle with which it intersects.
    *
    * @param triangle the triangle against which to clip this portal
    * @param portals the clipped portals
    */
   void Clip(const Triangle& triangle, vector<Portal>& portals)
   {
      Plane trianglePlane(
         triangle.mVertices[0],
         triangle.mVertices[1],
         triangle.mVertices[2]
      );
      
      Plane edgePlanes[3];
         
      unsigned int i, j;
      
      for(i=0;i<3;i++)
      {
         j = (i+1)%3;
         
         edgePlanes[i].set(
            triangle.mVertices[j],
            triangle.mVertices[i],
            triangle.mVertices[i] - mPlane.getNormal()
         );
      }
         
      vector<Portal> abovePortals, belowPortals;
      
      abovePortals.push_back(*this);
      
      for(i=0;i<3;i++)
      {
         vector<Portal> newAbovePortals;
            
         for(j=0;j<abovePortals.size();j++)
         {
            switch(abovePortals[j].Intersect(edgePlanes[i]))
            {
               case AbovePlane:
               case OnPlane:
                  newAbovePortals.push_back(abovePortals[j]);
                  break;
                           
               case BelowPlane:
                  belowPortals.push_back(abovePortals[j]);
                  break;
                           
               case IntersectsPlane:
                  abovePortals[j].Split(edgePlanes[i], newAbovePortals, belowPortals);
                  break;
            }
         }
               
         abovePortals = newAbovePortals;
      }
      
      portals.insert(
         portals.end(),
         belowPortals.begin(),
         belowPortals.end()
      );
   }
   
   /**
    * Computes the area of this portal.
    *
    * @return the portal area
    */
   float Area()
   {
      float totalArea = 0.0f;
      
      for(unsigned int i=1;i<mVertices.size()-1;i++)
      {
         Vec3 v1 = (mVertices[i] - mVertices[0]),
              v2 = (mVertices[i+1] - mVertices[0]);
              
         totalArea += (v1^v2).length();
      }
      
      return totalArea*0.5f;
   }
   
   /**
    * Flips this portal around.
    */
   void Flip()
   {
      mPlane.flip();
      
      reverse(mVertices.begin(), mVertices.end());
      
      swap(mLeftLeaves, mRightLeaves);
   }
};


/**
 * Clips a set of portal vertices against a view volume formed by two other sets
 * of portal vertices.
 *
 * @param sourceVertices the source portal vertices
 * @param targetVertices the target portal vertices
 * @param left true if the source is below the target, false if above
 * @param inVertices the set of portal vertices to clip
 * @param outVertices the location in which to place the clipped vertices
 * @return true if any portion of the clipped portal lies within the view volume,
 * false if the clipped portal lies entirely outside
 */
bool ClipToViewVolume(vector<Vec3>& sourceVertices,
                      vector<Vec3>& targetVertices,
                      bool left,
                      vector<Vec3>& inVertices,
                      vector<Vec3>& outVertices)
{
   unsigned int i, j, k;
   
   bool outside = false;
   
   outVertices = inVertices;
   
   vector<Vec3> aboveVertices;
   
   for(i=0;i<targetVertices.size() && !outside;i++)
   {
      j = (i+1)%targetVertices.size();
      
      Plane edgePlane(
         left ? targetVertices[i] : targetVertices[j],
         left ? targetVertices[j] : targetVertices[i],
         sourceVertices[0]
      );
      
      for(k=1;k<sourceVertices.size();k++)
      {
         if(edgePlane.distance(sourceVertices[k]) > 0.0f)
         {
            edgePlane.set(
               left ? targetVertices[i] : targetVertices[j],
               left ? targetVertices[j] : targetVertices[i],
               sourceVertices[k]
            );
         }
      }
      
      if(edgePlane.getNormal().length() > epsilon)
      {
         aboveVertices.clear();
      
         outside = true;
      
         for(j=0;j<outVertices.size();j++)
         {
            k = (j+1)%outVertices.size();
      
            float jDist = edgePlane.distance(outVertices[j]),
                  kDist = edgePlane.distance(outVertices[k]);
         
            if(jDist < -epsilon)
            {
               if(kDist > +epsilon)
               {
                  Vec3 newVertex = SplitLineSegment(
                     outVertices[j],
                     outVertices[k],
                     edgePlane
                  );
                  
                  aboveVertices.push_back(newVertex);
               }
            }
            else if(jDist > +epsilon)
            {
               outside = false;
               
               aboveVertices.push_back(outVertices[j]);
               
               if(kDist < -epsilon)
               {
                  Vec3 newVertex = SplitLineSegment(
                     outVertices[j],
                     outVertices[k],
                     edgePlane
                  );
                  
                  aboveVertices.push_back(newVertex);
               }
            }
            else
            {
               aboveVertices.push_back(outVertices[j]);
            }
         }
         
         outVertices = aboveVertices;
      }
   }
   
   return !outside;
}

/**
 * The root of the scene graph.
 */
Node* scene;

/**
 * A node in the working BSP tree.
 */
struct WorkingBSPNode : public Referenced
{
   /**
    * The parent of this node, or NULL for root.
    */
   osg::ref_ptr<WorkingBSPNode> mParent;
   
   
   /**
    * Computes a set of BSP tree statistics.
    *
    * @param maximumHeight the maximum height of the tree
    * @param minimumHeight the minimum height of the tree
    * @param averageHeight the average height of the tree
    * @param numberOfTriangles the total number of triangles
    * @param numberOfNodes the total number of nodes
    */
   virtual void GetStatistics(int* maximumHeight,
                              int* minimumHeight,
                              float* averageHeight,
                              int* numberOfTriangles,
                              int* numberOfNodes) = 0;
   
   /**
    * Creates the portals that connect leaf nodes.
    *
    * @param nodePath the path followed to this node
    */
   virtual void CreatePortals(vector<WorkingBSPInternalNode*>& nodePath) {}
   
   /**
    * Splits a group of portals across the partitioning planes
    * of the BSP tree.
    *
    * @param portals the portal group to split
    */
   virtual void SplitPortals(vector<Portal>& portals) {}
   
   /**
    * Determines which leaves contain the specified portal.
    *
    * @param portal the portal to place
    * @param left if true, place in left leaves; if false, in
    * right leaves
    */
   virtual void PlacePortal(Portal& portal, bool left) = 0;
   
   /**
    * Clips the portals in each leaf against that leaf's triangles.
    */
   virtual void ClipPortals() = 0;
   
   /**
    * Finds and marks all outside leaves.
    *
    * @param pt the last point in the fill sequence
    */
   virtual void FillOutside(const Vec3& pt) = 0;
   
   /**
    * Merges highly connected leaves into groups to simplify PVS calculation.
    */
   virtual void MergeHighlyConnectedLeaves() = 0;
   
   /**
    * Computes the potentially visible sets of this BSP tree.
    */
   virtual void ComputePotentiallyVisibleSets() = 0;

   /**
    * Clips the geode identified by the given path.
    *
    * @param path the path to the geode
    */
   virtual void ClipGeode(NodePath& path) = 0;
   
   /**
    * Regroups the geometry of the BSP tree.
    *
    * @return the newly created BSP group
    */
   virtual Group* Regroup() = 0;
   
   /**
    * Converts the working representation of the BSP tree into its final
    * form.
    *
    * @return the final node
    */
   virtual BSPNode* CreateFinalTree() = 0;
};

/**
 * The bounding sphere of the input geometry.
 */
BoundingSphere bound;

/**
 * The number of portals before clipping.
 */
int numberOfPortalsBeforeClipping = 0;

/**
 * An internal node in the working BSP tree.
 */
struct WorkingBSPInternalNode : public WorkingBSPNode
{
   /**
    * The partitioning plane.
    */
   Plane mPartitioningPlane;
   
   /**
    * The portals that lie on this partitioning plane.
    */
   vector<Portal> mPortals;
   
   /**
    * The left child (represents the space below the plane).
    */
   osg::ref_ptr<WorkingBSPNode> mLeftChild;
   
   /**
    * The right child (represents the space above the plane).
    */
   osg::ref_ptr<WorkingBSPNode> mRightChild;
   
   
   /**
    * Computes a set of BSP tree statistics.
    *
    * @param maximumHeight the maximum height of the tree
    * @param minimumHeight the minimum height of the tree
    * @param averageHeight the average height of the tree
    * @param numberOfTriangles the total number of triangles
    * @param numberOfNodes the total number of nodes
    */
   virtual void GetStatistics(int* maximumHeight,
                              int* minimumHeight,
                              float* averageHeight,
                              int* numberOfTriangles,
                              int* numberOfNodes)
   {
      int leftMaxHeight, leftMinHeight, leftNumTris, leftNumNodes,
          rightMaxHeight, rightMinHeight, rightNumTris, rightNumNodes;
      
      float leftAverageHeight, rightAverageHeight;
      
      mLeftChild->GetStatistics(
         &leftMaxHeight, 
         &leftMinHeight,
         &leftAverageHeight,
         &leftNumTris,
         &leftNumNodes
      );
      
      mRightChild->GetStatistics(
         &rightMaxHeight, 
         &rightMinHeight,
         &rightAverageHeight,
         &rightNumTris,
         &rightNumNodes
      );
      
      *maximumHeight = 1 + maximum(leftMaxHeight, rightMaxHeight);
      *minimumHeight = 1 + minimum(leftMinHeight, rightMinHeight);
      
      *averageHeight = 1.0f +
                       (leftAverageHeight*leftNumNodes + rightAverageHeight*rightNumNodes)/
                       (float)(leftNumNodes + rightNumNodes);
      
      *numberOfTriangles = leftNumTris + rightNumTris;
      *numberOfNodes = leftNumNodes + rightNumNodes + 1;
   }
   
   /**
    * Creates the portals that connect leaf nodes.
    *
    * @param nodePath the path followed to this node
    */
   virtual void CreatePortals(vector<WorkingBSPInternalNode*>& nodePath)
   {
      nodePath.push_back(this);
      
      Portal portal;
      
      Matrix matrix = Matrix::rotate(
         Vec3(0, 0, 1),
         mPartitioningPlane.getNormal()
      );
      
      Vec3 center = 
         bound.center() - 
         mPartitioningPlane.getNormal()*mPartitioningPlane.distance(bound.center());
         
      matrix.setTrans(center);

      float maxSize = bound.radius()*2;
      
      Vec3 ul( -maxSize, +maxSize, 0.0f ),
           ur( +maxSize, +maxSize, 0.0f ),
           ll( -maxSize, -maxSize, 0.0f ),
           lr( +maxSize, -maxSize, 0.0f );

      portal.mVertices.push_back(lr*matrix);
      portal.mVertices.push_back(ur*matrix);
      portal.mVertices.push_back(ul*matrix);
      portal.mVertices.push_back(ll*matrix);
      
      portal.mInternalNode = this;
      
      portal.mPlane = mPartitioningPlane;
      
      for(unsigned int i=0;i<nodePath.size()-1;i++)
      {
         WorkingBSPInternalNode* node = nodePath[i];
         
         if(portal.Intersect(node->mPartitioningPlane) == IntersectsPlane)
         {
            vector<Portal> abovePortals, belowPortals;
            
            portal.Split(node->mPartitioningPlane, abovePortals, belowPortals);
            
            if(nodePath[i+1] == node->mLeftChild.get())
            {
               portal = belowPortals[0];
            }
            else // nodePath[i+1] == node->mRightChild.get()
            {
               portal = abovePortals[0];
            }
         }
      }
      
      mPortals.push_back(portal);
      
      mLeftChild->SplitPortals(mPortals);
      mRightChild->SplitPortals(mPortals);
      
      static int portalsPlaced = 0;
      
      for(vector<Portal>::iterator it = mPortals.begin();
          it != mPortals.end();
          it++)
      {
         numberOfPortalsBeforeClipping++;
         
         mLeftChild->PlacePortal(*it, true);
         mRightChild->PlacePortal(*it, false);
         
         portalsPlaced++;
         
         if(portalsPlaced % 2500 == 0)
         {
            cout << ".";
         }
      }
      
      mLeftChild->CreatePortals(nodePath);
      mRightChild->CreatePortals(nodePath);
      
      nodePath.pop_back();
   }
   
   /**
    * Splits a group of portals across the partitioning planes
    * of the BSP tree.
    *
    * @param portals the portal group to split
    */
   virtual void SplitPortals(vector<Portal>& portals)
   {
      vector<Portal> abovePortals, belowPortals;
      
      for(vector<Portal>::iterator it = portals.begin();
          it != portals.end();
          it++)
      {
         switch((*it).Intersect(mPartitioningPlane))
         {
            case AbovePlane:
               abovePortals.push_back(*it);
               break;
               
            case BelowPlane:
               belowPortals.push_back(*it);
               break;
            
            case IntersectsPlane:
               (*it).Split(mPartitioningPlane, abovePortals, belowPortals);
               break;
            default:
               break;
         }
      }

      mLeftChild->SplitPortals(belowPortals);
      mRightChild->SplitPortals(abovePortals);
      
      portals.clear();
      
      portals.insert(
         portals.end(),
         belowPortals.begin(),
         belowPortals.end()
      );
      
      portals.insert(
         portals.end(),
         abovePortals.begin(),
         abovePortals.end()
      );
   }
   
   /**
    * Determines which leaves contain the specified portal.
    *
    * @param portal the portal to place
    * @param left if true, place in left leaves; if false, in
    * right leaves
    */
   virtual void PlacePortal(Portal& portal, bool left)
   {
      switch(portal.Intersect(mPartitioningPlane))
      {
         case BelowPlane:
            mLeftChild->PlacePortal(portal, left);
            break;
            
         case AbovePlane:
            mRightChild->PlacePortal(portal, left);
            break;
         
         case OnPlane:
         case IntersectsPlane:
            mLeftChild->PlacePortal(portal, left);
            mRightChild->PlacePortal(portal, left);
            break;
      }
   }
   
   /**
    * Clips the portals in each leaf against that leaf's triangles.
    */
   virtual void ClipPortals()
   {
      mLeftChild->ClipPortals();
      mRightChild->ClipPortals();
   }
   
   /**
    * Finds and marks all outside leaves.
    *
    * @param pt the last point in the fill sequence
    */
   virtual void FillOutside(const Vec3& pt)
   {
      if(mPartitioningPlane.distance(pt) < 0.0f)
      {
         mLeftChild->FillOutside(pt);
      }
      else
      {
         mRightChild->FillOutside(pt);
      }
   }
   
   /**
    * Merges highly connected leaves into groups to simplify PVS calculation.
    */
   virtual void MergeHighlyConnectedLeaves()
   {
      mLeftChild->MergeHighlyConnectedLeaves();
      mRightChild->MergeHighlyConnectedLeaves();
   }
   
   /**
    * Computes the potentially visible sets of this BSP tree.
    */
   virtual void ComputePotentiallyVisibleSets()
   {
      mLeftChild->ComputePotentiallyVisibleSets();
      mRightChild->ComputePotentiallyVisibleSets();
   }
   
   /**
    * Clips the geode identified by the given path.
    *
    * @param path the path to the geode
    */
   virtual void ClipGeode(NodePath& path)
   {
      vector<NodePath> abovePaths, belowPaths;
      
      SplitGeode(
         mPartitioningPlane, 
         path,
         abovePaths,
         belowPaths
      );
      
      if(belowPaths.size() > 0)
      {
         mLeftChild->ClipGeode(belowPaths[0]);
      }
      
      if(abovePaths.size() > 0)
      {
         mRightChild->ClipGeode(abovePaths[0]);
      }
   }
   
   /**
    * Regroups the geometry of the BSP tree.
    *
    * @return the newly created BSP group
    */
   virtual Group* Regroup()
   {
      Group* group = new Group;
     
      group->addChild(mLeftChild->Regroup());
      group->addChild(mRightChild->Regroup());
       
      return group;
   }
   
   /**
    * Converts the working representation of the BSP tree into its final
    * form.
    *
    * @return the final node
    */
   virtual BSPNode* CreateFinalTree()
   {
      return new BSPInternalNode(
         mPartitioningPlane,
         mLeftChild->CreateFinalTree(),
         mRightChild->CreateFinalTree()
      );
   }
};

/**
 * The total number of leaf nodes, used to calculate the average PVS
 * size.
 */
int totalLeafNodes = 0;

/**
 * The sum of all PVS sizes, used to calculate the average PVS size.
 */
int totalPVSSize = 0;

/**
 * The total number of leaf groups.
 */
int totalLeafGroups = 0;

/**
 * If true, render portals between cells for debugging.
 */
bool debugPortals = false;

/**
 * The portal debug geometry.
 */
Geometry* portalGeometry = NULL;

/**
 * If true, don't merge leaves with lots of portals.
 */
bool noMerge = false;

/**
 * If true, ignore any leaves outside the geometry.
 */
bool noOutside = false;

/**
 * If true, show the path of the outside fill.
 */
bool traceOutside = false;

/**
 * The fill trace geometry.
 */
Geometry* traceGeometry = NULL;

/**
 * The number of leaves on the outside.
 */
int totalLeavesOutside = 0;

/**
 * If true, don't clip the geodes to the BSP leaves.
 */
bool noClip = false;

/**
 * If true, don't regroup the geodes according to BSP tree position.
 */
bool noRegroup = false;

/**
 * If true, don't optimize the scene graph before writing it.
 */
bool noOptimize = false;

/**
 * The master list of clipped, oriented portals.
 */
vector<Portal*> orientedPortals;

/**
 * The master list of potentially visible sets.
 */
vector<PotentiallyVisibleSet*> potentiallyVisibleSets;

/**
 * A visitor that builds a potentially visible set.
 */
class PotentiallyVisibleSetVisitor : public NodeVisitor
{
   public:
   
      /**
       * The potentially visible set.
       */
      osg::ref_ptr<PotentiallyVisibleSet> mPotentiallyVisibleSet;
      
      
      /**
       * Constructor.
       */
      PotentiallyVisibleSetVisitor()
         : NodeVisitor(TRAVERSE_ACTIVE_CHILDREN)
      {
         mPotentiallyVisibleSet = new PotentiallyVisibleSet;
      }
      
      /**
       * Applies this visitor to a node.
       *
       * @param node the node to visit
       */
      virtual void apply(Node& node)
      {
         if(node.getName().find("/dynamic") == string::npos)
         {
            NodeVisitor::apply(node);
         }
      }
      
      /**
       * Applies this visitor to a node.
       *
       * @param node the node to visit
       */
      virtual void apply(Group& node)
      {
         if(node.getName().find("/dynamic") == string::npos)
         {
            NodeVisitor::apply(node);
          
            unsigned int i;
              
            if(node.getNumParents() > 0)
            {
               for(i=0;i<node.getNumChildren();i++)
               {
                  if(mPotentiallyVisibleSet->GetNodesToEnable().count(node.getChild(i)) > 0)
                  {
                     mPotentiallyVisibleSet->GetNodesToEnable().insert(&node);
                     break;
                  }
               }
            
               if(i==node.getNumChildren())
               {
                  for(i=0;i<node.getNumChildren();i++)
                  {
                     if(mPotentiallyVisibleSet->GetNodesToDisable().count(node.getChild(i)) == 0)
                     {
                        break;
                     }
                  }
               
                  if(i==node.getNumChildren())
                  {
                     for(i=0;i<node.getNumChildren();i++)
                     {
                        mPotentiallyVisibleSet->GetNodesToDisable().erase(node.getChild(i));
                     }
                     
                     mPotentiallyVisibleSet->GetNodesToDisable().insert(&node);
                  }
               }
            }
         }
      }
      
      /**
       * Applies this visitor to a node.
       *
       * @param node the node to visit
       */
      virtual void apply(Geode& node)
      {
         if(node.getName().find("/dynamic") == string::npos &&
            node.getName() != "debug")
         {
            if(mPotentiallyVisibleSet->GetNodesToEnable().count(&node) == 0)
            {
               mPotentiallyVisibleSet->GetNodesToDisable().insert(&node);
            }
         }
      }
};

/**
 * A leaf node in the working BSP tree.
 */
struct WorkingBSPLeafNode : public WorkingBSPNode
{
   /**
    * The triangles in this leaf.
    */
   vector<Triangle> mTriangles;
   
   /**
    * The unclipped portals in this leaf.
    */
   vector<Portal*> mPortals;
   
   /**
    * Whether or not this cell is known to be outside the geometry.
    */
   bool mOutside;
   
   /**
    * The clipped portals.
    */
   vector<Portal> mClippedPortals;
   
   /**
    * The surface area of the leaf.
    */
   float mSurfaceArea;
   
   /**
    * The covered area of the leaf.
    */
   float mCoveredArea;
   
   /**
    * The set of leaves in this leaf's merged group.
    */
   set<WorkingBSPLeafNode*>* mLeafGroup;
   
   /**
    * The other leaves potentially visible from this leaf.
    */
   set<WorkingBSPLeafNode*> mPotentiallyVisibleLeaves;
   
   /**
    * The geodes contained in this leaf.
    */
   vector<Geode*> mGeodes;
   
   
   /**
    * Computes a set of BSP tree statistics.
    *
    * @param maximumHeight the maximum height of the tree
    * @param minimumHeight the minimum height of the tree
    * @param averageHeight the average height of the tree
    * @param numberOfTriangles the total number of triangles
    * @param numberOfNodes the total number of nodes
    */
   virtual void GetStatistics(int* maximumHeight,
                              int* minimumHeight,
                              float* averageHeight,
                              int* numberOfTriangles,
                              int* numberOfNodes)
   {
      *maximumHeight = *minimumHeight = *numberOfNodes = 1;
      *averageHeight = 1.0f;
      *numberOfTriangles = mTriangles.size();
   }
   
   /**
    * Determines which leaves contain the specified portal.
    *
    * @param portal the portal to place
    * @param left if true, place in left leaves; if false, in
    * right leaves
    */
   virtual void PlacePortal(Portal& portal, bool left)
   {
      mPortals.push_back(&portal);
      
      if(left)
      {
         portal.mLeftLeaves.insert(this);
      }
      else
      {
         portal.mRightLeaves.insert(this);
      }
   }
   
   /**
    * Clips the portals in each leaf against that leaf's triangles.
    */
   virtual void ClipPortals()
   {
      static int portalsClipped = 0;
      
      mSurfaceArea = 0.0f;
      
      for(vector<Portal*>::iterator p = mPortals.begin();
          p != mPortals.end();
          p++)
      {
         Portal newPortal = *(*p);
       
         mSurfaceArea += newPortal.Area();
           
         if(newPortal.mLeftLeaves.count(this) == 0)
         {
            newPortal.Flip();
         }
            
         bool completelyObscured = false;
         
         vector<Triangle*> obscuringTriangles;
         
         for(vector<Triangle>::iterator t = mTriangles.begin();
             t != mTriangles.end() && !completelyObscured;
             t++)
         {
            switch(newPortal.Intersect(*t))
            {
               case NotObscured:
                  break;
                  
               case PartiallyObscured:
                  obscuringTriangles.push_back(&(*t));
                  break;
                  
               case CompletelyObscured:
                  completelyObscured = true;
                  break;
            }
         }
         
         if(!completelyObscured)
         {
            if(obscuringTriangles.size() == 0)
            {
               mClippedPortals.push_back(newPortal);
            }
            else
            {
               vector<Portal> newPortals;
               
               newPortals.push_back(newPortal);
               
               for(vector<Triangle*>::iterator ot = obscuringTriangles.begin();
                   ot != obscuringTriangles.end();
                   ot++)
               {
                  vector<Portal> nextNewPortals;
                  
                  for(vector<Portal>::iterator np = newPortals.begin();
                      np != newPortals.end();
                      np++)
                  {
                     switch((*np).Intersect(*(*ot)))
                     {
                        case NotObscured:
                           nextNewPortals.push_back(*np);
                           break;
               
                        case PartiallyObscured:
                           (*np).Clip(*(*ot), nextNewPortals);
                           break;
               
                        case CompletelyObscured:
                           break;
                     }
                  }
                  
                  newPortals = nextNewPortals;
               }
               
               mClippedPortals.insert(
                  mClippedPortals.end(),
                  newPortals.begin(),
                  newPortals.end()
               );
            }
         }
         
         portalsClipped++;
         
         if(portalsClipped % 5000 == 0)
         {
            cout << ".";
         }
      }
      
      mCoveredArea = mSurfaceArea;
      
      for(vector<Portal>::iterator cp = mClippedPortals.begin();
          cp != mClippedPortals.end();
          cp++)
      {
         if(debugPortals)
         {
            Vec3Array* v3a = dynamic_cast<Vec3Array*>(portalGeometry->getVertexArray());
            
            int first = v3a->size();
               
            for(unsigned int i=0;i<(*cp).mVertices.size();i++)
            {
               v3a->push_back((*cp).mVertices[i]);
            }
               
            portalGeometry->addPrimitiveSet(
               new DrawArrays(GL_POLYGON, first, (*cp).mVertices.size())
            );
         }
      
         mCoveredArea -= (*cp).Area();
            
         orientedPortals.push_back(&(*cp));
      }
   }
   
   /**
    * Finds and marks all outside leaves.
    *
    * @param pt the last point in the fill sequence
    */
   virtual void FillOutside(const Vec3& pt)
   {
      mOutside = true;
      
      totalLeavesOutside++;
      
      for(vector<Portal>::iterator it = mClippedPortals.begin();
          it != mClippedPortals.end();
          it++)
      {
         for(set<WorkingBSPLeafNode*>::iterator n = (*it).mRightLeaves.begin();
             n != (*it).mRightLeaves.end();
             n++)
         {
            if(!(*n)->mOutside)
            {
               for(vector<Portal>::iterator it2 = (*n)->mClippedPortals.begin();
                   it2 != (*n)->mClippedPortals.end();
                   it2++)
               {
                  if((*it2).mRightLeaves.count(this) > 0)
                  {
                     Vec3 portalCenter;
                     
                     for(vector<Vec3>::iterator v = (*it).mVertices.begin();
                         v != (*it).mVertices.end();
                         v++)
                     {
                        portalCenter += *v;
                     }
                     
                     portalCenter /= (*it).mVertices.size();
                     
                     if(traceOutside)
                     {
                        Vec3Array* v3a = dynamic_cast<Vec3Array*>(traceGeometry->getVertexArray());
               
                        int first = v3a->size();
                  
                        v3a->push_back(pt);
                        v3a->push_back(portalCenter);
                        
                        traceGeometry->addPrimitiveSet(
                           new DrawArrays(GL_LINES, first, 2)
                        );
                     }
                     
                     (*n)->FillOutside(portalCenter);
                     
                     break;
                  }
               }
            }
         }
      }
   }
   
   /**
    * Merges a new leaf node into this leaf's group.
    *
    * @param leaf the leaf node to merge
    * @param candidatePortals the list of portals to merge across
    */
   void MergeLeaf(WorkingBSPLeafNode* leaf,
                  vector<Portal*>& candidatePortals)
   {
      mLeafGroup->insert(leaf);
      
      leaf->mLeafGroup = mLeafGroup;
      
      
      // Add new leaf's portals to candidate list
         
      for(vector<Portal>::iterator p = leaf->mClippedPortals.begin();
          p != leaf->mClippedPortals.end();
          p++)
      {
         set<WorkingBSPLeafNode*>::iterator pl;
         for(pl = (*p).mRightLeaves.begin();
             pl != (*p).mRightLeaves.end();
             pl++)
         {
            vector<Portal>::iterator plp; 
            for(plp = (*pl)->mClippedPortals.begin();
                plp != (*pl)->mClippedPortals.end();
                plp++)
            {
               if((*plp).mRightLeaves.count(leaf) > 0 &&
                  equivalent((*p).Area(), (*plp).Area(), epsilon))
               {
                  break;
               }
            }
            
            if(plp == (*pl)->mClippedPortals.end())
            {
               break;
            }
         }
         
         if(pl == (*p).mRightLeaves.end())
         {
            candidatePortals.push_back(&(*p));
         }
      }
      
      
      // Delete any portals that lead back into the group
      
      for(set<WorkingBSPLeafNode*>::iterator l = mLeafGroup->begin();
          l != mLeafGroup->end();
          l++)
      {
         for(vector<Portal>::iterator p = (*l)->mClippedPortals.begin();
             p != (*l)->mClippedPortals.end();
             p++)
         {
            if(!(*p).mInternal)
            {
               set<WorkingBSPLeafNode*>::iterator pl; 
               for(pl = (*p).mRightLeaves.begin();
                   pl != (*p).mRightLeaves.end();
                   pl++)
               {
                  if(mLeafGroup->count(*pl) == 0)
                  {
                     break;
                  }
               }
            
               if(pl == (*p).mRightLeaves.end())
               {
                  (*p).mInternal = true;
               
                  vector<Portal*>::iterator r;
                  
                  for(r = orientedPortals.begin();
                      r != orientedPortals.end();
                      r++)
                  {
                     if(*r == &(*p))
                     {
                        orientedPortals.erase(r);
                        break;
                     }
                  }
                  
                  for(r = candidatePortals.begin();
                      r != candidatePortals.end();
                      r++)
                  {
                     if(*r == &(*p))
                     {
                        candidatePortals.erase(r);
                        break;
                     }
                  }
               }
            }
         }
      }
   }
   
   /**
    * Merges highly connected leaves into groups to simplify PVS calculation.
    */
   virtual void MergeHighlyConnectedLeaves()
   {
      if(!mOutside && mLeafGroup == NULL)
      {
         if(mCoveredArea/mSurfaceArea < epsilon)
         {
            mLeafGroup = new set<WorkingBSPLeafNode*>;
            
            float gCoveredArea = mCoveredArea,
                  gSurfaceArea = mSurfaceArea;
            
            vector<Portal*> candidatePortals;
            
            MergeLeaf(this, candidatePortals);
            
            while(gCoveredArea/gSurfaceArea < epsilon &&
                  candidatePortals.size() > 0)
            {
               Portal* p = candidatePortals.front();
               
               candidatePortals.erase(candidatePortals.begin());
               
               for(set<WorkingBSPLeafNode*>::iterator l = p->mRightLeaves.begin();
                   l != p->mRightLeaves.end();
                   l++)
               {
                  if(!(*l)->mOutside && (*l)->mLeafGroup == NULL)
                  {
                     MergeLeaf(*l, candidatePortals);
                  
                     gCoveredArea += (*l)->mCoveredArea;
                     gSurfaceArea += (*l)->mSurfaceArea;
                     gSurfaceArea -= p->Area()*2;
                        
                     totalLeafGroups--;
                  }
               }
            }
            
            cout << ".";
         }
      }
   }
   
   /**
    * Expands the set of portals potentially visible from the source portal.
    *
    * @param potentiallyVisiblePortals the set of potentially visible portals to
    * expand
    * @param possiblyVisiblePortals the set of possibly visible portals, formed by
    * intersecting the possibly visible portals of the source with those of each
    * new target portal in the portal sequence
    * @param sourceVertices the vertices of the source portal
    * @param targetVertices the vertices of the target portal
    * @param viewVolume the view volume
    */
   void ExpandPotentiallyVisiblePortals(set<Portal*>& potentiallyVisiblePortals,
                                        set<Portal*>& possiblyVisiblePortals,
                                        vector<Vec3>& sourceVertices,
                                        vector<Vec3>& targetVertices)
   {
      if(mLeafGroup == NULL)
      {
         mLeafGroup = new set<WorkingBSPLeafNode*>;
         
         mLeafGroup->insert(this);
      }
      
      vector<Vec3> newTargetVertices, newSourceVertices;
      
      for(set<WorkingBSPLeafNode*>::iterator l = mLeafGroup->begin();
          l != mLeafGroup->end();
          l++)
      {   
         for(vector<Portal>::iterator p = (*l)->mClippedPortals.begin();
             p != (*l)->mClippedPortals.end();
             p++)
         {
            if(!(*p).mInternal && !(*p).mVisited && possiblyVisiblePortals.count(&(*p)) > 0)
            {
               if(ClipToViewVolume(sourceVertices, targetVertices, true, (*p).mVertices, newTargetVertices) &&
                  ClipToViewVolume(newTargetVertices, targetVertices, false, sourceVertices, newSourceVertices))
               {
                  potentiallyVisiblePortals.insert(&(*p));
                  
                  bool canSeeNewPortals = false;
            
                  for(set<Portal*>::iterator pvp = (*p).mPossiblyVisiblePortals.begin();
                      pvp != (*p).mPossiblyVisiblePortals.end() && !canSeeNewPortals;
                      pvp++)
                  {
                     if(potentiallyVisiblePortals.count(*pvp) == 0)
                     {
                        canSeeNewPortals = true;
                     }
                  }
               
                  if(canSeeNewPortals)
                  {
                     (*p).mVisited = true;
                     
                     /*
                     set<Portal*> newPossiblyVisiblePortals;
                     
                     set_intersection(
                        possiblyVisiblePortals.begin(),
                        possiblyVisiblePortals.end(),
                        (*p).mPossiblyVisiblePortals.begin(),
                        (*p).mPossiblyVisiblePortals.end(),
                        insert_iterator< set<Portal*> >(
                           newPossiblyVisiblePortals,
                           newPossiblyVisiblePortals.end()
                        )
                     );
                     */
                     
                     for(set<WorkingBSPLeafNode*>::iterator lpl = (*p).mRightLeaves.begin();
                         lpl != (*p).mRightLeaves.end();
                         lpl++)
                     {
                        (*lpl)->ExpandPotentiallyVisiblePortals(
                           potentiallyVisiblePortals,
                           possiblyVisiblePortals, // newPossiblyVisiblePortals,
                           newSourceVertices,
                           newTargetVertices
                        );
                     }
                     
                     (*p).mVisited = false;
                  }
               }
            }
         }
      }
   }
   
   /**
    * Computes the potentially visible sets of this BSP tree.
    */
   virtual void ComputePotentiallyVisibleSets()
   {
      if(mLeafGroup == NULL)
      {
         mLeafGroup = new set<WorkingBSPLeafNode*>;
         
         mLeafGroup->insert(this);
      }
      
      for(set<WorkingBSPLeafNode*>::iterator lg = mLeafGroup->begin();
          lg != mLeafGroup->end();
          lg++)
      {
         mPotentiallyVisibleLeaves.insert(*lg);
      
         for(vector<Portal>::iterator it = (*lg)->mClippedPortals.begin();
             it != (*lg)->mClippedPortals.end();
             it++)
         {
            if(!(*it).mInternal)
            {
               set<WorkingBSPLeafNode*>::iterator l;
         
               for(l = (*it).mRightLeaves.begin();
                   l != (*it).mRightLeaves.end();
                   l++)
               {
                  mPotentiallyVisibleLeaves.insert(*l);
               }
         
               for(set<Portal*>::iterator pvp = (*it).mPossiblyVisiblePortals.begin();
                   pvp != (*it).mPossiblyVisiblePortals.end();
                   pvp++)
               {
                  for(l = (*pvp)->mRightLeaves.begin();
                      l != (*pvp)->mRightLeaves.end();
                      l++)
                  {
                     mPotentiallyVisibleLeaves.insert(*l);
                  }
               }
            }
         }
      }
      
      totalPVSSize += mPotentiallyVisibleLeaves.size();
   }
   
   /**
    * Clips the geode identified by the given path.
    *
    * @param path the path to the geode
    */
   virtual void ClipGeode(NodePath& path)
   {
      Geode* geode = dynamic_cast<Geode*>(path.back());
      
      mGeodes.push_back(geode);
   }
   
   /**
    * Regroups the geometry of the BSP tree.
    *
    * @return the newly created BSP group
    */
   virtual Group* Regroup()
   {
      Group* group = new Group;
     
      for(unsigned int i=0;i<mGeodes.size();i++)
      {
         group->addChild(mGeodes[i]);
         
         Node::ParentList pl = mGeodes[i]->getParents();
         
         for(unsigned int j=0;j<pl.size();j++)
         {
            if(pl[j] != group)
            {
               pl[j]->removeChild(mGeodes[i]);
            }
         }
      }
      
      return group;
   }
   
   /**
    * Converts the working representation of the BSP tree into its final
    * form.
    *
    * @return the final node
    */
   virtual BSPNode* CreateFinalTree()
   {
      PotentiallyVisibleSetVisitor pvsv;
      
      for(set<WorkingBSPLeafNode*>::iterator l = mPotentiallyVisibleLeaves.begin();
          l != mPotentiallyVisibleLeaves.end();
          l++)
      {
         if(noClip)
         {
            for(vector<Triangle>::iterator t = (*l)->mTriangles.begin();
                t != (*l)->mTriangles.end();
                t++)
            {
               pvsv.mPotentiallyVisibleSet->GetNodesToEnable().insert((*t).mGeode.get());
            }
         }
         else
         {
            pvsv.mPotentiallyVisibleSet->GetNodesToEnable().insert(
               (*l)->mGeodes.begin(),
               (*l)->mGeodes.end()
            );
         }
      }
      
      scene->accept(pvsv);
      
      for(vector<PotentiallyVisibleSet*>::iterator pvs = potentiallyVisibleSets.begin();
          pvs != potentiallyVisibleSets.end();
          pvs++)
      {
         NodeSet diff1, diff2;
         
         set_symmetric_difference(
            (*pvs)->GetNodesToDisable().begin(),
            (*pvs)->GetNodesToDisable().end(),
            pvsv.mPotentiallyVisibleSet->GetNodesToDisable().begin(),
            pvsv.mPotentiallyVisibleSet->GetNodesToDisable().end(),
            insert_iterator<NodeSet>(diff1, diff1.end())
         );
         
         set_symmetric_difference(
            (*pvs)->GetNodesToEnable().begin(),
            (*pvs)->GetNodesToEnable().end(),
            pvsv.mPotentiallyVisibleSet->GetNodesToEnable().begin(),
            pvsv.mPotentiallyVisibleSet->GetNodesToEnable().end(),
            insert_iterator<NodeSet>(diff2, diff2.end())
         );
         
         if(diff1.size() == 0 && diff2.size() == 0)
         {
            return new BSPLeafNode(*pvs);
         }
      }
      
      potentiallyVisibleSets.push_back(pvsv.mPotentiallyVisibleSet.get());
      
      return new BSPLeafNode(pvsv.mPotentiallyVisibleSet.get());
   }
};

/**
 * Computes the rank of a potential partioning plane.
 *
 * @param trianglesAbovePlane the number of triangles entirely above the plane
 * @param trianglesBelowPlane the number of triangles entirely below the plane
 * @param trianglesSplitByPlane the number of triangles split by the plane
 * @param trianglesOnPlane the number of triangles on the plane
 * @return the rank of the plane
 */
float ComputeRank(int trianglesAbovePlane,
                  int trianglesBelowPlane,
                  int trianglesSplitByPlane,
                  int trianglesOnPlane)
{
   int triangleImbalance;
   
   if(trianglesAbovePlane > trianglesBelowPlane)
   {
      triangleImbalance = trianglesAbovePlane - trianglesBelowPlane;
   }
   else
   {
      triangleImbalance = trianglesBelowPlane - trianglesAbovePlane;
   }
   
   return trianglesOnPlane - trianglesSplitByPlane*2 - triangleImbalance;
}

/**
 * Determines the relationship between a plane and a triangle.
 *
 * @param plane the plane of interest
 * @param triangle the triangle of interest
 * @return the result of the intersection test
 */
PlanePolygonIntersectResult PlaneTriangleIntersect(const Plane& plane,
                                                   const Triangle& triangle)
{
   bool above = false, below = false;
   
   for(int i=0;i<3;i++)
   {
      float dist = plane.distance(triangle.mVertices[i]);
      
      if(dist < -epsilon) below = true;
      else if(dist > +epsilon) above = true;
   }
   
   if(above && below)
   {
      return IntersectsPlane;
   }
   else if(above)
   {
      return AbovePlane;
   }
   else if(below)
   {
      return BelowPlane;
   }
   else
   {
      return OnPlane;
   }
}

/**
 * Splits a triangle across a plane.
 *
 * @param triangle the triangle to split
 * @param plane the plane to split across
 * @param aboveTriangles the list of triangles above
 * the plane
 * @param belowTriangles the list of triangles below
 * the plane
 */
void SplitTriangle(const Triangle& triangle,
                   const Plane& plane,
                   vector<Triangle>& aboveTriangles,
                   vector<Triangle>& belowTriangles)
{
   Vec3 fanCenter;
   
   bool foundCenter = false;
   
   vector<Vec3> fanEdge;
   
   unsigned int i(0);
   unsigned int lastVertex(0);
   
   for(i=0;i<3;i++)
   {
      fanEdge.push_back(triangle.mVertices[i]);
      
      unsigned int j = (i+1)%3;
      
      float iDist = plane.distance(triangle.mVertices[i]),
            jDist = plane.distance(triangle.mVertices[j]);
            
      if((iDist < -epsilon && jDist > +epsilon) ||
         (iDist > +epsilon && jDist < -epsilon))
      {
         Vec3 newVertex = SplitLineSegment(
            triangle.mVertices[i],
            triangle.mVertices[j],
            plane
         );
         
         if(!foundCenter)
         { 
            fanCenter = newVertex;
            foundCenter = true;
            lastVertex = fanEdge.size()-1;
         }
         else
         {
            fanEdge.push_back(newVertex);
         }
      }
   }
   
   for(i=(lastVertex+1)%fanEdge.size();
       i!=lastVertex;
       i=(i+1)%fanEdge.size())
   {
      unsigned int j = (i+1)%fanEdge.size();
      
      Triangle t;
      
      t.mVertices[0] = fanCenter;
      t.mVertices[1] = fanEdge[i];
      t.mVertices[2] = fanEdge[j];
      t.mGeode = triangle.mGeode;
      
      switch(PlaneTriangleIntersect(plane, t))
      {
         case AbovePlane:
            aboveTriangles.push_back(t);
            break;
            
         case BelowPlane:
            belowTriangles.push_back(t);
            break;
            
         case OnPlane:
         case IntersectsPlane:
            cout << "Error: triangle split failed" << endl;
            break;
      }
   }
}

/**
 * The number of triangles to consider as candidates for
 * the partitioning plane.
 */
const int candidates = 32;

/**
 * The maximum number of triangles to store in each leaf.
 */
unsigned int leafSize = 0;

/**
 * Builds a BSP (sub)tree out of the specified triangles.
 *
 * @param candidateTriangles the triangles that may be used as partitioning
 * planes
 * @param nonCandidateTriangles the triangles which already lie on a partitioning
 * plane
 * @return the newly created (sub)tree
 */
WorkingBSPNode* BuildTree(const vector<Triangle>& candidateTriangles,
                          const vector<Triangle>& nonCandidateTriangles)
{
   if(candidateTriangles.size() + nonCandidateTriangles.size() <= leafSize)
   {
      WorkingBSPLeafNode* node = new WorkingBSPLeafNode;
      
      node->mTriangles = candidateTriangles;
      
      node->mTriangles.insert(
         node->mTriangles.end(),
         nonCandidateTriangles.begin(),
         nonCandidateTriangles.end()
      );
      
      node->mOutside = false;
      
      node->mLeafGroup = NULL;
      
      totalLeafNodes++;
      
      return node;
   }
   
   float bestRank = -FLT_MAX;
   Plane bestPlane;

   static int candidatesConsidered = 0;
   
   unsigned int i, j, remainingCandidates;
   
   remainingCandidates = candidates;
   
   for(i=0;i<candidateTriangles.size();i++)
   {
      Plane plane;
      
      plane.set(
         candidateTriangles[i].mVertices[0], 
         candidateTriangles[i].mVertices[1], 
         candidateTriangles[i].mVertices[2]
      );
      
      if(plane.getNormal().length() > epsilon)
      {  
         int trianglesAbovePlane = 0,
             trianglesBelowPlane = 0,
             trianglesSplitByPlane = 0,
             trianglesOnPlane = 0;
               
         for(j=0;j<candidateTriangles.size();j++)
         {
            switch(PlaneTriangleIntersect(plane, candidateTriangles[j]))
            {
               case AbovePlane:
                  trianglesAbovePlane++;
                  break;
                  
               case BelowPlane:
                  trianglesBelowPlane++;
                  break;
                  
               case IntersectsPlane:
                  trianglesSplitByPlane++;
                  break;
                  
               case OnPlane:
                  trianglesOnPlane++;
                  break;
            }
         }
         
         for(j=0;j<nonCandidateTriangles.size();j++)
         {
            switch(PlaneTriangleIntersect(plane, nonCandidateTriangles[j]))
            {
               case AbovePlane:
                  trianglesAbovePlane++;
                  break;
                  
               case BelowPlane:
                  trianglesBelowPlane++;
                  break;
                  
               case IntersectsPlane:
                  trianglesSplitByPlane++;
                  break;
               default:
                  break;
            }
         }
         
         float rank = ComputeRank(
            trianglesAbovePlane,
            trianglesBelowPlane,
            trianglesSplitByPlane,
            trianglesOnPlane
         );
         
         if(rank > bestRank)
         {
            bestRank = rank;
            bestPlane = plane;
         }
         
         candidatesConsidered++;
         
         if(candidatesConsidered % 500 == 0)
         {
            cout << ".";
         }
         
         if(remainingCandidates-- < 0)
         {
            break;
         }
      }
   }
   
   if(bestPlane.getNormal().length() < epsilon)
   {
      WorkingBSPLeafNode* node = new WorkingBSPLeafNode;
      
      node->mTriangles = candidateTriangles;
      
      node->mTriangles.insert(
         node->mTriangles.end(),
         nonCandidateTriangles.begin(),
         nonCandidateTriangles.end()
      );
      
      node->mOutside = false;
      
      node->mLeafGroup = NULL;
      
      totalLeafNodes++;
      
      return node;
   }
   else
   {
      WorkingBSPInternalNode* node = new WorkingBSPInternalNode;
      
      node->mPartitioningPlane = bestPlane;
      
      vector<Triangle> aboveCandidates, belowCandidates,
                       aboveNonCandidates, belowNonCandidates;
       
      for(i=0;i<candidateTriangles.size();i++)
      {
         switch(PlaneTriangleIntersect(node->mPartitioningPlane, candidateTriangles[i]))
         {
            case AbovePlane:
               aboveCandidates.push_back(candidateTriangles[i]);
               break;
                     
            case BelowPlane:
               belowCandidates.push_back(candidateTriangles[i]);
               break;
               
            case IntersectsPlane:
               SplitTriangle(
                  candidateTriangles[i], 
                  node->mPartitioningPlane, 
                  aboveCandidates, 
                  belowCandidates
               );
               break;
               
            case OnPlane:
               Plane plane;
               plane.set(
                  candidateTriangles[i].mVertices[0],
                  candidateTriangles[i].mVertices[1],
                  candidateTriangles[i].mVertices[2]
               );
               if(node->mPartitioningPlane.getNormal()*plane.getNormal() < 0.0f)
               {
                  belowNonCandidates.push_back(candidateTriangles[i]);
               }
               else
               {
                  aboveNonCandidates.push_back(candidateTriangles[i]);
               }
               break;
         }
      }
      
      for(i=0;i<nonCandidateTriangles.size();i++)
      {
         switch(PlaneTriangleIntersect(node->mPartitioningPlane, nonCandidateTriangles[i]))
         {
            case AbovePlane:
               aboveNonCandidates.push_back(nonCandidateTriangles[i]);
               break;
                     
            case BelowPlane:
               belowNonCandidates.push_back(nonCandidateTriangles[i]);
               break;
               
            case IntersectsPlane:
               SplitTriangle(
                  nonCandidateTriangles[i], 
                  node->mPartitioningPlane, 
                  aboveNonCandidates, 
                  belowNonCandidates
               );
               break;
               
            case OnPlane:
               Plane plane;
               plane.set(
                  nonCandidateTriangles[i].mVertices[0],
                  nonCandidateTriangles[i].mVertices[1],
                  nonCandidateTriangles[i].mVertices[2]
               );
               if(node->mPartitioningPlane.getNormal()*plane.getNormal() < 0.0f)
               {
                  belowNonCandidates.push_back(nonCandidateTriangles[i]);
               }
               else
               {
                  aboveNonCandidates.push_back(nonCandidateTriangles[i]);
               }
               break;
         }
      }
      
      node->mLeftChild = BuildTree(belowCandidates, belowNonCandidates);
      node->mRightChild = BuildTree(aboveCandidates, aboveNonCandidates);
      
      node->mLeftChild->mParent = node;
      node->mRightChild->mParent = node;
      
      return node;
   }
}

/**
 * Collects the paths to all the geodes in the scene graph.
 */
class GeodeCollector : public NodeVisitor
{
   public:
      
      vector<NodePath> mPaths;
      
      /**
       * Constructor.
       */
      GeodeCollector()
         : NodeVisitor(TRAVERSE_ACTIVE_CHILDREN)
      {}
      
      /**
       * Applies this visitor to a node.
       *
       * @param node the node to visit
       */
      virtual void apply(Node& node)
      {
         if(node.getName().find("/dynamic") == string::npos)
         {
            NodeVisitor::apply(node);
         }
      }
      
      /**
       * Applies this visitor.
       *
       * @param node the node to apply the visitor on
       */
      virtual void apply(Geode& node)
      {
         if(!node.containsDrawable(portalGeometry) &&
            !node.containsDrawable(traceGeometry))
         {
            mPaths.push_back(getNodePath());
         }
      }
};



/**
 * Flattens the transform and state of a Geode by acculumating the
 * transforms/states along the node path.
 *
 * @param path the path to the Geode
 */
void FlattenGeodeTransformAndState(NodePath& path)
{
   osg::ref_ptr<StateSet> stateSet = new StateSet;
   
	stateSet->clear();
   
   Matrix matrix;
   
   for(NodePath::iterator it = path.begin();
       it != path.end();
       it++)
   {
      if((*it)->getStateSet() != NULL)
      {
         stateSet->merge(*(*it)->getStateSet());
      }
      
      Transform* transform = dynamic_cast<Transform*>(*it);
                  
      if(transform != NULL)
      {
         transform->computeLocalToWorldMatrix(matrix, NULL);
      }
   }
   
   Geode* geode = dynamic_cast<Geode*>(path.back());
   
   geode->setStateSet(stateSet.get());
   
   unsigned int i, j;
   
   for( i = 0; i < geode->getNumDrawables(); i++ )
   {
      if( Geometry* geometry = geode->getDrawable(i)->asGeometry() )
      {
         if( Vec3Array* vertexArray = dynamic_cast< osg::Vec3Array* >( geometry->getVertexArray() ) )
         {
            for( j = 0; j < vertexArray->size(); j++ )
            {
               (*vertexArray)[j] = (*vertexArray)[j]*matrix;
            }
         }
         
         if( Vec3Array* v3a = dynamic_cast< osg::Vec3Array* >( geometry->getNormalArray() ) )
         {
            for( j = 0; j < v3a->size(); j++ )
            {
               (*v3a)[j] = Matrix::transform3x3((*v3a)[j], matrix);
            }
         }
      }
   }
}

/**
 * Sort predicate for sorting portals according to the number of portals that
 * they may possibly see.
 *
 * @param one the first portal to compare
 * @param two the second portal to compare
 * @return true if the first portal has fewer possibly visible portals, false
 * otherwise
 */
bool FewerPossiblyVisible(Portal* one, Portal* two)
{
   return one->mPossiblyVisiblePortals.size() < two->mPossiblyVisiblePortals.size();
}

/**
 * Finds the portals possibly visible from each portal.
 */
void FindPossiblyVisiblePortals()
{
   unsigned int i, j;
   
   //static int portalsScanned = 0;
   
   if(noOutside)
   {
      set<Portal*> outsidePortals;
   
      for(i=0;i<orientedPortals.size();i++)
      {
         bool outside = true;
         
         for(set<WorkingBSPLeafNode*>::iterator l = orientedPortals[i]->mLeftLeaves.begin();
             l != orientedPortals[i]->mLeftLeaves.end() && outside;
             l++)
         {
            outside = (*l)->mOutside;
         }
         
         if(outside)
         {
            outsidePortals.insert(orientedPortals[i]);
         }
      }
      
      for(set<Portal*>::iterator p = outsidePortals.begin();
          p != outsidePortals.end();
          p++)
      {
         for(vector<Portal*>::iterator op = orientedPortals.begin();
             op != orientedPortals.end();
             op++)
         {
            if(*op == *p)
            {
               orientedPortals.erase(op);
               break;
            }
         }
      }
   }
   
   for(i=0;i<orientedPortals.size();i++)
   {
      for(j=0;j<orientedPortals.size();j++)
      {
         if(i != j)
         {
            PlanePolygonIntersectResult r1 =
               orientedPortals[j]->Intersect(orientedPortals[i]->mPlane);
            
            PlanePolygonIntersectResult r2 =
               orientedPortals[i]->Intersect(orientedPortals[j]->mPlane);
         
            if((r1 == AbovePlane || r1 == IntersectsPlane) &&
               (r2 == BelowPlane || r2 == IntersectsPlane))
            {
               orientedPortals[i]->mPossiblyVisiblePortals.insert(
                  orientedPortals[j]
               );
            }
         }
      }
      
      if(i % 1000 == 0)
      {
         cout << ".";
      }
   }
   
   sort(orientedPortals.begin(), orientedPortals.end(), FewerPossiblyVisible);
}

/**
 * Computes the potentially visible portals of each oriented portal.
 */
void ComputePotentiallyVisiblePortals()
{
   int portalNumber = 0;
   
   for(vector<Portal*>::iterator p = orientedPortals.begin();
       p != orientedPortals.end();
       p++)
   {
      (*p)->mVisited = true;
      
      set<Portal*> potentiallyVisiblePortals;
      
      for(set<WorkingBSPLeafNode*>::iterator l = (*p)->mRightLeaves.begin();
          l != (*p)->mRightLeaves.end();
          l++)
      {
         for(vector<Portal>::iterator lp = (*l)->mClippedPortals.begin();
             lp != (*l)->mClippedPortals.end();
             lp++)
         {
            if((*p)->mPossiblyVisiblePortals.count(&(*lp)) > 0)
            {
               (*lp).mVisited = true;
               
               potentiallyVisiblePortals.insert(&(*lp));
             
               /*
               set<Portal*> newPossiblyVisiblePortals;
                  
               set_intersection(
                  (*p)->mPossiblyVisiblePortals.begin(),
                  (*p)->mPossiblyVisiblePortals.end(),
                  (*lp).mPossiblyVisiblePortals.begin(),
                  (*lp).mPossiblyVisiblePortals.end(),
                  insert_iterator< set<Portal*> >(
                     newPossiblyVisiblePortals,
                     newPossiblyVisiblePortals.end()
                  )
               );
               */
                  
               for(set<WorkingBSPLeafNode*>::iterator lpl = (*lp).mRightLeaves.begin();
                   lpl != (*lp).mRightLeaves.end();
                   lpl++)
               {
                  (*lpl)->ExpandPotentiallyVisiblePortals(
                     potentiallyVisiblePortals,
                     (*p)->mPossiblyVisiblePortals, // newPossiblyVisiblePortals,
                     (*p)->mVertices,
                     (*lp).mVertices
                  );
               }
               
               (*lp).mVisited = false;
            }
         }
      }
      
      (*p)->mVisited = false;
      
      (*p)->mPossiblyVisiblePortals = potentiallyVisiblePortals;
      
      if(++portalNumber % 10 == 0)
      {
         cout << ".";
      }
   }
}

/**
 * Transforms the specified input node into a BSP-ified
 * output node.
 *
 * @param input the input node to transform
 * @return the newly created output node
 */
Node* Compile(Node* input)
{
   TriangleVisitor* tv = new TriangleVisitor;
   
   input->accept(*tv);
   
   cout << "Scanned " << 
        tv->mRecorder.mTriangles.size() + tv->mRecorder.mDetailTriangles.size() <<
        " triangles" << endl;
   
   cout << "Building BSP tree";
   
   osg::ref_ptr<WorkingBSPNode> tree = BuildTree(
      tv->mRecorder.mTriangles,
      tv->mRecorder.mDetailTriangles
   );
   
   cout << endl;
   
   int maxHeight, minHeight, numTris, numNodes;
   float averageHeight;
   
   tree->GetStatistics( 
      &maxHeight, 
      &minHeight, 
      &averageHeight, 
      &numTris,
      &numNodes
   );
   
   cout << "Maximum BSP tree height: " << maxHeight << endl;
   cout << "Minimum BSP tree height: " << minHeight << endl;
   cout << "Average BSP tree height: " << averageHeight << endl;
   cout << "Total number of triangles: " << numTris << endl;
   cout << "Total number of nodes: " << numNodes << endl;
   
   cout << "Creating portals";
   
   bound = input->getBound();

   vector<WorkingBSPInternalNode*> vbspin = vector<WorkingBSPInternalNode*>();
   tree->CreatePortals(vbspin);
   
   cout << endl;
   
   cout << "Number of portals before clipping: " <<
        numberOfPortalsBeforeClipping*2 << endl;
   
   cout << "Clipping portals";
   
   if(debugPortals)
   {
      Group* group = dynamic_cast<Group*>(input);
      
      if(group != NULL)
      {
         Geode* portalGeode = new Geode;
      
         portalGeode->setName("debug");
         
         group->addChild(portalGeode);
      
         Material* mat = new Material;
         mat->setDiffuse(Material::FRONT_AND_BACK, Vec4(1.f,0.f,1.f,0.5f));
         mat->setAmbient(Material::FRONT_AND_BACK, Vec4(1.f,0.f,1.f,1.0f));
         mat->setEmission(Material::FRONT_AND_BACK, Vec4(0.f,0.f,0.f,1.f));

         FrontFace* ff = new FrontFace;
         ff->setMode(FrontFace::CLOCKWISE);
         
         StateSet* ss = portalGeode->getOrCreateStateSet();
         ss->setAttributeAndModes(mat, StateAttribute::ON);
         ss->setAttributeAndModes(ff, StateAttribute::ON);
         ss->setMode(GL_CULL_FACE, StateAttribute::ON);
         ss->setMode(GL_BLEND, StateAttribute::ON);
         ss->setRenderingHint(StateSet::TRANSPARENT_BIN);
      
         portalGeometry = new Geometry;
      
         portalGeode->addDrawable(portalGeometry);
      
         portalGeometry->setVertexArray(new Vec3Array);
      }
   }
   
   tree->ClipPortals();
   
   cout << endl;
   
   cout << "Number of portals after clipping: " <<
        orientedPortals.size() << endl;
   
   if(traceOutside)
   {
      Group* group = dynamic_cast<Group*>(input);
      
      if(group != NULL)
      {
         Geode* traceGeode = new Geode;
      
         traceGeode->setName("debug");
         
         group->addChild(traceGeode);
      
         traceGeometry = new Geometry;
      
         traceGeode->addDrawable(traceGeometry);
      
         traceGeometry->setVertexArray(new Vec3Array);
      }
   }
   
   if(traceOutside || noOutside)
   {
      Vec3 wayOut = bound.center() + Vec3(bound.radius()*2, 0, 0);
      
      tree->FillOutside(wayOut);
      
      cout << "Leaves outside: " << totalLeavesOutside << "/" << totalLeafNodes << endl;
   }
   
   if(!noMerge)
   {
      cout << "Merging highly connected leaves";
    
      totalLeafGroups = totalLeafNodes;
        
      tree->MergeHighlyConnectedLeaves();
      
      cout << endl;
      
      cout << "Leaf groups: " << totalLeafGroups << "/" << totalLeafNodes << endl;
      
      cout << "New number of portals: " << orientedPortals.size() << endl;
   }
   
   cout << "Finding possibly visible portals";
   
   FindPossiblyVisiblePortals();
   
   cout << endl;
   
   cout << "Computing potentially visible sets";
   
   ComputePotentiallyVisiblePortals();
   
   tree->ComputePotentiallyVisibleSets();
   
   cout << endl;
   
   cout << "Average PVS size: " << totalPVSSize / (float)totalLeafNodes
        << "/" << totalLeafNodes << endl;
   
   if(!noClip)
   {
      cout << "Clipping geodes";
   
      GeodeCollector gc;
      
      input->accept(gc);
   
      vector<NodePath>::iterator it;
      
      for(it = gc.mPaths.begin();
          it != gc.mPaths.end();
          it++)
      {
         tree->ClipGeode(*it);
         
         cout << ".";
      }
      
      cout << endl;
      
      if(!noRegroup)
      {
         cout << "Regrouping geodes" << endl;
         
         gc.mPaths.clear();
         
         input->accept(gc);
         
         for(it = gc.mPaths.begin();
             it != gc.mPaths.end();
             it++)
         {
            FlattenGeodeTransformAndState(*it);
         }
         
         Group* newGroup = new Group;
         
         newGroup->addChild(tree->Regroup());
         
         newGroup->addChild(input);
         
         input = newGroup;
      }
   }

   if(!noOptimize)
   {
      cout << "Optimizing";
      
      Optimizer optimizer;
      
      optimizer.optimize(
         input, 
         Optimizer::SHARE_DUPLICATE_STATE | Optimizer::REMOVE_REDUNDANT_NODES
      );

      TriStripVisitor tsv = TriStripVisitor();
      input->accept( tsv );
      
      cout << endl;
   }
   
   BSPCullCallback* bspcc = new BSPCullCallback;
   
   scene = input;
   
   bspcc->SetBSPTree(
      tree->CreateFinalTree()
   );
   
   input->setCullCallback(bspcc);
   
   return input;
}

/**
 * Program entry point.
 *
 * @param argc the number of command line arguments
 * @param argv the array of command line arguments
 */
int main(int argc, char *argv[])
{
   string inFile, outFile;
   
   for(int i=1;i<argc;i++)
   {
      if(argv[i][0] == '-')
      {
         string command = argv[i] + 1;

         if(command == "leaf_size")
         {
            i += 1;
            
            if(i == argc || argv[i][0] == '-')
            {
               cerr << "Missing argument: number of triangles" << endl;
            }
            else
            {
               leafSize = atoi(argv[i]);
               
               cout << "Using leaf size of " << leafSize << endl;
            }
         }
         else if(command == "debug_portals")
         {
            debugPortals = true;
         }
         else if(command == "no_outside")
         {
            noOutside = true;
         }
         else if(command == "trace_outside")
         {
            traceOutside = true;
         }
         else if(command == "no_merge")
         {
            noMerge = true;
         }
         else if(command == "no_clip")
         {
            noClip = true;
         }
         else if(command == "no_regroup")
         {
            noRegroup = true;
         }
         else if(command == "no_optimize")
         {
            noOptimize = true;
         }
         else
         {
            cerr << "Unrecognized argument: " << command << endl;
         }
      }
      else if(inFile == "")
      {
         inFile = argv[i];
      }
      else if(outFile == "")
      {
         outFile = argv[i];
      }
   }
   
   if(inFile == "" || outFile == "")
   {
      cout << "usage: bspCompiler [-options] in_file out_file" << endl;
      cout << endl;
      cout << "where options include:" << endl;
      cout << "    -leaf_size <number of triangles>" << endl;
      cout << "    -debug_portals" << endl;
      cout << "    -no_outside" << endl;
      cout << "    -trace_outside" << endl;
      cout << "    -no_merge" << endl;
      cout << "    -no_clip" << endl;
      cout << "    -no_regroup" << endl;
      cout << "    -no_optimize" << endl;
      return 1;
   }
   
   cout << "Reading " << inFile << endl;
   
   Node* input = readNodeFile(inFile);
   
   if(input == NULL)
   {
      cout << "Couldn't read " << inFile << endl;
      return 1;
   }
   
   Node* output = Compile(input);

   if(output != NULL)
   {
      cout << "Writing " << outFile << endl;
    
      if(!writeNodeFile(*output, outFile))
      {
         cout << "Couldn't write " << outFile << endl;
         return 1;
      }
   }
   
   return 0;
}
