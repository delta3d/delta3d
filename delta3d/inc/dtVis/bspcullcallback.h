#ifndef DELTA_BSPCULLCALLBACK
#define DELTA_BSPCULLCALLBACK

// bspcullcallback.h: Declaration of the BSPCullCallback class.
//
//////////////////////////////////////////////////////////////////////


#include <set>

#include <osg/Export>
#include <osg/NodeCallback>

#include <osgDB/Registry>

namespace dtVis
{
   class BSPNode;
   
   
   /**
    * A cull callback that contains a BSP tree with sets of potentially
    * visible geodes in its leaves.
    */
   class SG_EXPORT BSPCullCallback : public osg::NodeCallback
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
         META_Object(dtVis, BSPCullCallback);
         
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
         osg::ref_ptr<BSPNode> mBSPTree;
   };
   
   
   /**
    * A set of OSG node references.
    */
   typedef std::set< osg::ref_ptr<osg::Node> > NodeSet;
   
   
   /**
    * The potentially visible set type.
    */
   class SG_EXPORT PotentiallyVisibleSet : public osg::Object
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
         META_Object(dtVis, PotentiallyVisibleSet);
      
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
   class SG_EXPORT BSPNode : public osg::Referenced
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
   class SG_EXPORT BSPInternalNode : public BSPNode
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
         osg::ref_ptr<BSPNode> mLeftChild;
         
         /**
          * The right child of this node.
          */
         osg::ref_ptr<BSPNode> mRightChild;
   };
   
   
   /**
    * A leaf node in the BSP tree.
    */
   class SG_EXPORT BSPLeafNode : public BSPNode
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
         osg::ref_ptr<PotentiallyVisibleSet> mPotentiallyVisibleSet;
   };
};


#endif // DELTA_BSPCULLCALLBACK
