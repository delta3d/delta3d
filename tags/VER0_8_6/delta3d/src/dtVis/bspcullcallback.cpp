// bspcullcallback.cpp: Implementation of the BSPCullCallback class.
//
//////////////////////////////////////////////////////////////////////

#include <osgUtil/CullVisitor>

#include "dtVis/bspcullcallback.h"
#include "dtCore/notify.h"

using namespace dtVis;
using namespace std;


/**
 * Reads a BSP node from the input stream.
 *
 * @param fr the input reader
 */
BSPNode* readBSPNode(osgDB::Input& fr)
{
   if(fr.matchSequence("BSPInternalNode {"))
   {
      fr += 2;
      
      osg::Plane plane;
      
      if(fr.matchSequence("PartitioningPlane %f %f %f %f"))
      {
         fr[1].getFloat(plane[0]);
         fr[2].getFloat(plane[1]);
         fr[3].getFloat(plane[2]);
         fr[4].getFloat(plane[3]);
         
         fr += 5;
      }
      
      BSPNode* leftChild = readBSPNode(fr);
      BSPNode* rightChild = readBSPNode(fr);
      
      BSPNode* node = new BSPInternalNode(plane, leftChild, rightChild);
      
      ++fr;
      
      return node;
   }
   else if(fr.matchSequence("BSPLeafNode {"))
   {
      fr += 2;
      
      PotentiallyVisibleSet* pvs = dynamic_cast<PotentiallyVisibleSet*>(
         fr.readObject()
      );
      
      BSPNode* node = new BSPLeafNode(pvs);
      
      ++fr;
      
      return node;
   }
   else
   {
      return NULL;
   }
}

/**
 * Object reader function.
 *
 * @param obj the object to populate
 * @param fr the input reader
 * @return true if the read was successful, false otherwise
 */
bool BSPCullCallback_readLocalData(osg::Object& obj, osgDB::Input& fr)
{
   BSPCullCallback& bspcc = dynamic_cast<BSPCullCallback&>(obj);
   
   bspcc.SetBSPTree(
      readBSPNode(fr)
   );
   
   return true;
}

/**
 * Writes a BSP node to the output stream.
 *
 * @param bspNode the node to write
 * @param fw the output writer
 */
void writeBSPNode(BSPNode* bspNode, osgDB::Output& fw)
{
   BSPInternalNode* bspin = dynamic_cast<BSPInternalNode*>(bspNode);
   
   if(bspin != NULL)
   {
      fw.indent() << "BSPInternalNode {" << endl;
      
      fw.moveIn();
      
      fw.indent() << "PartitioningPlane " << bspin->GetPartitioningPlane() << endl;
      
      writeBSPNode(bspin->GetLeftChild(), fw);
      
      writeBSPNode(bspin->GetRightChild(), fw);
      
      fw.moveOut();
      
      fw.indent() << "}" << endl;
   }
   else
   {
      BSPLeafNode* bspln = dynamic_cast<BSPLeafNode*>(bspNode);
   
      fw.indent() << "BSPLeafNode {";
      
      fw << endl;
         
      fw.moveIn();
      
      fw.writeObject(*bspln->GetPotentiallyVisibleSet());
      
      fw.moveOut();
      
      fw.indent() << "}" << endl;
   }
} 

/**
 * Object writer function.
 *
 * @param obj the object to write
 * @param fw the output writer
 * @return true if the write was successful, false otherwise
 */
bool BSPCullCallback_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
   osg::Object& mut_obj = (osg::Object&)obj;
   
   BSPCullCallback& bspcc = dynamic_cast<BSPCullCallback&>(mut_obj);
   
   if(bspcc.GetBSPTree() != NULL)
   {
      writeBSPNode(bspcc.GetBSPTree(), fw);
   }
   
   return true;
}

/**
 * The registration proxy for BSPCullCallback.
 */
osgDB::RegisterDotOsgWrapperProxy bspccRegistrationProxy(
   new BSPCullCallback,
   "BSPCullCallback",
   "Object NodeCallback BSPCullCallback",
   BSPCullCallback_readLocalData,
   BSPCullCallback_writeLocalData
);


/**
 * Object reader function.
 *
 * @param obj the object to populate
 * @param fr the input reader
 * @return true if the read was successful, false otherwise
 */
bool PotentiallyVisibleSet_readLocalData(osg::Object& obj, osgDB::Input& fr)
{
   PotentiallyVisibleSet& pvs = dynamic_cast<PotentiallyVisibleSet&>(obj);
   
   if(fr.matchSequence("NodesToDisable {"))
   {
      fr += 2;
      
      while(!fr.matchSequence("}"))
      {
         dtCore::RefPtr<osg::Node> node = dynamic_cast<osg::Node*>(
            fr.readObject()
         );
         
         if(node.get() != NULL)
         {
            pvs.GetNodesToDisable().insert(node);
         }
      }
      
      ++fr;
   }
   
   if(fr.matchSequence("NodesToEnable {"))
   {
      fr += 2;
      
      while(!fr.matchSequence("}"))
      {
         dtCore::RefPtr<osg::Node> node = dynamic_cast<osg::Node*>(
            fr.readObject()
         );
         
         if(node.get() != NULL)
         {
            pvs.GetNodesToEnable().insert(node);
         }
      }
      
      ++fr;
   }
   
   return true;
}

/**
 * Object writer function.
 *
 * @param obj the object to write
 * @param fw the output writer
 * @return true if the write was successful, false otherwise
 */
bool PotentiallyVisibleSet_writeLocalData(const osg::Object& obj, osgDB::Output& fw)
{
   osg::Object& mut_obj = (osg::Object&)obj;
   
   PotentiallyVisibleSet& pvs = dynamic_cast<PotentiallyVisibleSet&>(mut_obj);
   
   fw.indent() << "NodesToDisable {";
      
   if(pvs.GetNodesToDisable().size() > 0)
   {
      fw << endl;
      
      fw.moveIn();
   
      for(NodeSet::iterator it = pvs.GetNodesToDisable().begin();
          it != pvs.GetNodesToDisable().end();
          it++)
      {
         fw.writeObject(*(*it).get());
      }
   
      fw.moveOut();
   
      fw.indent();
   }
   
   fw << "}" << endl;
   
   fw.indent() << "NodesToEnable {";
   
   if(pvs.GetNodesToEnable().size() > 0)
   {
      fw << endl;
      
      fw.moveIn();
   
      for(NodeSet::iterator it = pvs.GetNodesToEnable().begin();
          it != pvs.GetNodesToEnable().end();
          it++)
      {
         fw.writeObject(*(*it).get());
      }
   
      fw.moveOut();
   
      fw.indent();
   }
   
   fw << "}" << endl;
   
   return true;
}

/**
 * The registration proxy for PotentiallyVisibleSet.
 */
osgDB::RegisterDotOsgWrapperProxy pvsRegistrationProxy(
   new PotentiallyVisibleSet,
   "PotentiallyVisibleSet",
   "Object PotentiallyVisibleSet",
   PotentiallyVisibleSet_readLocalData,
   PotentiallyVisibleSet_writeLocalData
);


/**
 * Default constructor.
 */
BSPCullCallback::BSPCullCallback()
{}

/**
 * Copy constructor.
 *
 * @param bspGroup the BSP group to copy
 * @param copyOn the copy operation
 */
BSPCullCallback::BSPCullCallback(const BSPCullCallback& bspCullCallback,
                                 const osg::CopyOp& copyOp)
   : NodeCallback(bspCullCallback, copyOp),
     mBSPTree(bspCullCallback.mBSPTree)
{}

/**
 * Sets the BSP tree associated with this cull callback.
 *
 * @param bspTree the new BSP tree
 */
void BSPCullCallback::SetBSPTree(BSPNode* bspTree)
{
   mBSPTree = bspTree;
}

/**
 * Returns the BSP tree associated with this cull callback.
 *
 * @return the current BSP tree
 */
BSPNode* BSPCullCallback::GetBSPTree()
{
   return mBSPTree.get();
}

/**
 * Callback method.
 *
 * @param node the node being visited
 * @param nv the active node visitor
 */
void BSPCullCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
   if(mBSPTree.get() != NULL)
   {
      PotentiallyVisibleSet* pvs = 
         mBSPTree->GetPotentiallyVisibleSet(nv->getEyePoint());
      
      NodeSet::iterator it;
      
      for(it = pvs->GetNodesToDisable().begin();
          it != pvs->GetNodesToDisable().end();
          it++)
      {
         dtCore::RefPtr<osg::Node> nrp = (*it);
         nrp->setNodeMask(0x0);
      }
      
      for(it = pvs->GetNodesToEnable().begin();
          it != pvs->GetNodesToEnable().end();
          it++)
      {
         dtCore::RefPtr<osg::Node> nrp = (*it);
         nrp->setNodeMask(0xFFFFFFFF);
      }
   }
   
   traverse(node, nv);
}


/**
 * Default constructor.
 */
PotentiallyVisibleSet::PotentiallyVisibleSet()
{}

/**
 * Copy constructor.
 *
 * @param pvs the potentially visible set to copy
 * @param copyOp the copy operation
 */
PotentiallyVisibleSet::PotentiallyVisibleSet(const PotentiallyVisibleSet& pvs,
                                             const osg::CopyOp& copyOp)
   : Object(pvs, copyOp),
     mNodesToDisable(pvs.mNodesToDisable),
     mNodesToEnable(pvs.mNodesToEnable)
{}

/**
 * Returns the set of nodes to disable.
 *
 * @return the set of nodes to disable
 */
NodeSet& PotentiallyVisibleSet::GetNodesToDisable()
{
   return mNodesToDisable;
}

/**
 * Returns the set of nodes to enable.
 *
 * @return the set of nodes to enable
 */
NodeSet& PotentiallyVisibleSet::GetNodesToEnable()
{
   return mNodesToEnable;
}         


/**
 * Constructor.
 *
 * @param partitioningPlane the partitioning plane
 * @param leftChild the left child of the node
 * @param rightChild the right child of the node
 */
BSPInternalNode::BSPInternalNode(const osg::Plane& partitioningPlane,
                                 BSPNode* leftChild, BSPNode* rightChild)
   : mPartitioningPlane(partitioningPlane),
     mLeftChild(leftChild),
     mRightChild(rightChild)
{}

/**
 * Returns this internal node's partitioning plane.
 *
 * @return the node's partitioning plane
 */
const osg::Plane& BSPInternalNode::GetPartitioningPlane()
{
   return mPartitioningPlane;
}

/**
 * Returns the left child of this node (the child that represents the
 * space below the partitioning plane).
 *
 * @return the left child of this node
 */
BSPNode* BSPInternalNode::GetLeftChild()
{
   return mLeftChild.get();
}

/**
 * Returns the right child of this node (the child that represents the
 * space above the partitioning plane).
 *
 * @return the right child of this node
 */
BSPNode* BSPInternalNode::GetRightChild()
{
   return mRightChild.get();
}

/**
 * Returns the potentially visible set for the specified eyepoint.
 *
 * @param eyepoint the eyepoint of the viewer in the local coordinate system
 * @return the potentially visible set
 */
PotentiallyVisibleSet* BSPInternalNode::GetPotentiallyVisibleSet(const osg::Vec3& eyepoint)
{
   if(mPartitioningPlane.distance(eyepoint) < 0.0f)
   {
      return mLeftChild->GetPotentiallyVisibleSet(eyepoint);
   }
   else
   {
      return mRightChild->GetPotentiallyVisibleSet(eyepoint);
   }
}


/**
 * Constructor.
 *
 * @param potentiallyVisibleSet the potentially visible set associated
 * with this leaf node
 */
BSPLeafNode::BSPLeafNode(PotentiallyVisibleSet* potentiallyVisibleSet)
   : mPotentiallyVisibleSet(potentiallyVisibleSet)
{}

/**
 * Returns the potentially visible set of this leaf node.
 *
 * @return the current potentially visible set
 */
PotentiallyVisibleSet* BSPLeafNode::GetPotentiallyVisibleSet()
{
   return mPotentiallyVisibleSet.get();
}

/**
 * Returns the potentially visible set for the specified eyepoint.
 *
 * @param eyepoint the eyepoint of the viewer in the local coordinate system
 * @return the potentially visible set
 */
PotentiallyVisibleSet* BSPLeafNode::GetPotentiallyVisibleSet(const osg::Vec3& eyepoint)
{
   return mPotentiallyVisibleSet.get();
}
