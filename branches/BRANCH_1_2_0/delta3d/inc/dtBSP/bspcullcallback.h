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
*/

#ifndef DELTA_BSPCULLCALLBACK
#define DELTA_BSPCULLCALLBACK

// bspcullcallback.h: Declaration of the BSPCullCallback class.
//
//////////////////////////////////////////////////////////////////////


#include <set>

#include <osg/Export>
#include <osg/NodeCallback>
#include <osgDB/Registry>

#include <dtCore/refptr.h>

/** Used by the bspCompiler to calculate the BSP of a given set of geometry.
*/
namespace dtBSP
{
   class BSPNode;
   
   
   /**
    * A cull callback that contains a BSP tree with sets of potentially
    * visible geodes in its leaves.
    */
   class OSG_EXPORT BSPCullCallback : public osg::NodeCallback
   {
      public:
   
         /**
          * Default constructor.
          */
         BSPCullCallback();
        
         /**
          * Copy constructor.
          *
          * @param bspCullCallback the BSP cull callback to copy
          * @param copyOp the copy operation
          */
         BSPCullCallback(const BSPCullCallback& bspCullCallback,
                         const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);

         /**
          * OSG node class helper macro.
          */
         META_Object(dtBSP, BSPCullCallback);
         
         /**
          * Sets the BSP tree associated with this cull callback.
          *
          * @param bspTree the new BSP tree
          */
         void SetBSPTree(BSPNode* bspTree);
         
         /**
          * Returns the BSP tree associated with this cull callback.
          *
          * @return the current BSP tree
          */
         BSPNode* GetBSPTree();
         
         /**
          * Callback method.
          *
          * @param node the node being visited
          * @param nv the active node visitor
          */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
         
         
      private:
         
         /**
          * The root node of the BSP tree.
          */
         dtCore::RefPtr<BSPNode> mBSPTree;
   };
   
   
   /**
    * A set of OSG node references.
    */
   typedef std::set< dtCore::RefPtr<osg::Node> > NodeSet;
   
   
   /**
    * The potentially visible set type.
    */
   class OSG_EXPORT PotentiallyVisibleSet : public osg::Object
   {
      public:
         
         /**
          * Default constructor.
          */
         PotentiallyVisibleSet();
        
         /**
          * Copy constructor.
          *
          * @param pvs the potentially visible set to copy
          * @param copyOp the copy operation
          */
         PotentiallyVisibleSet(const PotentiallyVisibleSet& pvs,
                               const osg::CopyOp& copyOp = osg::CopyOp::SHALLOW_COPY);
                         
         /**
          * OSG node class helper macro.
          */
         META_Object(dtBSP, PotentiallyVisibleSet);
      
         /**
          * Returns the set of nodes to disable.
          *
          * @return the set of nodes to disable
          */
         NodeSet& GetNodesToDisable();
         
         /**
          * Returns the set of nodes to enable.
          *
          * @return the set of nodes to enable
          */
         NodeSet& GetNodesToEnable();
         
         
      private:
      
         /**
          * The set of nodes to disable.
          */
         NodeSet mNodesToDisable;
         
         /**
          * The set of nodes to enable.
          */
         NodeSet mNodesToEnable;
   };
   
   
   /**
    * A BSP tree node.
    */
   class OSG_EXPORT BSPNode : public osg::Referenced
   {
      public:

         /**
          * Returns the potentially visible set for the specified eyepoint.
          *
          * @param eyepoint the eyepoint of the viewer in the local coordinate system
          * @return the potentially visible set
          */
         virtual PotentiallyVisibleSet* GetPotentiallyVisibleSet(const osg::Vec3& eyepoint) = 0;
   };
   
   
   /**
    * A non-leaf node in the BSP tree.
    */
   class OSG_EXPORT BSPInternalNode : public BSPNode
   {
      public:
      
         /**
          * Constructor.
          *
          * @param partitioningPlane the partitioning plane
          * @param leftChild the left child of the node
          * @param rightChild the right child of the node
          */
         BSPInternalNode(const osg::Plane& partitioningPlane,
                         BSPNode* leftChild, BSPNode* rightChild);
         
         /**
          * Returns this internal node's partitioning plane.
          *
          * @return the node's partitioning plane
          */
         const osg::Plane& GetPartitioningPlane();
         
         /**
          * Returns the left child of this node (the child that represents the
          * space below the partitioning plane).
          *
          * @return the left child of this node
          */
         BSPNode* GetLeftChild();
         
         /**
          * Returns the right child of this node (the child that represents the
          * space above the partitioning plane).
          *
          * @return the right child of this node
          */
         BSPNode* GetRightChild();
         
         /**
          * Returns the potentially visible set for the specified eyepoint.
          *
          * @param eyepoint the eyepoint of the viewer in the local coordinate system
          * @return the potentially visible set
          */
         virtual PotentiallyVisibleSet* GetPotentiallyVisibleSet(const osg::Vec3& eyepoint);
         
         
      private:
      
         /**
          * The partioning plane.
          */
         osg::Plane mPartitioningPlane;
      
         /**
          * The left child of this node.
          */
         dtCore::RefPtr<BSPNode> mLeftChild;
         
         /**
          * The right child of this node.
          */
         dtCore::RefPtr<BSPNode> mRightChild;
   };
   
   
   /**
    * A leaf node in the BSP tree.
    */
   class OSG_EXPORT BSPLeafNode : public BSPNode
   {
      public:
      
         /**
          * Constructor.
          *
          * @param potentiallyVisibleSet the potentially visible set associated
          * with this leaf node
          */
         BSPLeafNode(PotentiallyVisibleSet* potentiallyVisibleSet);
         
         /**
          * Returns the potentially visible set of this leaf node.
          *
          * @return the current potentially visible set
          */
         PotentiallyVisibleSet* GetPotentiallyVisibleSet();
         
         /**
          * Returns the potentially visible set for the specified eyepoint.
          *
          * @param eyepoint the eyepoint of the viewer in the local coordinate system
          * @return the potentially visible set
          */
         virtual PotentiallyVisibleSet* GetPotentiallyVisibleSet(const osg::Vec3& eyepoint);
         
         
      private:
      
         /**
          * The potentially visible set for this leaf node.
          */
         dtCore::RefPtr<PotentiallyVisibleSet> mPotentiallyVisibleSet;
   };
};


#endif // DELTA_BSPCULLCALLBACK
