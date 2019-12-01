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
 * Matthew "w00by" Stokes
 */

#include <prefix/dtutilprefix.h>
#include <dtUtil/nodecollector.h>
#include <dtUtil/collectorutil.h>
#include <dtUtil/log.h>
#include <dtUtil/bits.h>

#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_ALL_START
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osgSim/DOFTransform>
#include <osg/Switch>
#include <osgSim/MultiSwitch>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/LOD>
DT_DISABLE_WARNING_END

DT_DISABLE_WARNING_START_CLANG("-Woverloaded-virtual")

namespace dtUtil
{
   //Define all the different flags so that users can OR the variables together to create a mask
   const NodeCollector::NodeFlag NodeCollector::GroupFlag = dtUtil::Bits::Add(0,1);
   const NodeCollector::NodeFlag NodeCollector::DOFTransformFlag = dtUtil::Bits::Add(0,2);
   const NodeCollector::NodeFlag NodeCollector::MatrixTransformFlag = dtUtil::Bits::Add(0,4);
   const NodeCollector::NodeFlag NodeCollector::SwitchFlag = dtUtil::Bits::Add(0,8);
   const NodeCollector::NodeFlag NodeCollector::MultiSwitchFlag = dtUtil::Bits::Add(0,16);
   const NodeCollector::NodeFlag NodeCollector::GeodeFlag = dtUtil::Bits::Add(0,32);
   const NodeCollector::NodeFlag NodeCollector::LODFlag = dtUtil::Bits::Add(0,64);

   //A Flag that will return all nodes and Drawable / Material objects that it finds
   const NodeCollector::NodeFlag NodeCollector::AllNodeTypes = (NodeCollector::GroupFlag | 
                                                                NodeCollector::DOFTransformFlag | 
                                                                NodeCollector::MatrixTransformFlag | 
                                                                NodeCollector::SwitchFlag |
                                                                NodeCollector::MultiSwitchFlag |
                                                                NodeCollector::GeodeFlag |
                                                                NodeCollector::LODFlag);
  
   //GroupVisitor Class
   //This object is used to traverse nodes and visit different the different kinds of nodes / objects that it runs into
   class GroupVisitor : public osg::NodeVisitor
   {
   public:
      /**
       * Constructor for the GroupVisitor Class 
       * @param NewNodeCollector The NodeCollector Object that it is going to manage
       * @param mask The different node / object types that are going to be looked for
       * @param nodeNamesIgnored A name of a node / object that you don't want to look for
       */
      GroupVisitor(NodeCollector* NewNodeCollector, NodeCollector::NodeFlag mask, const std::string & nodeNamesIgnored)
      : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
      , mNodeManager(NewNodeCollector)
      , mNodeMask(mask)
      , mNodeNamesIgnored(nodeNamesIgnored)
      {

      }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

      /**
       * Function that traverses through a geode and visits all of the Drawable Objects associated with the Geode and 
       * the Material Objects that are associated with the Drawable Objects.  If it finds an Object that it is looking
       * for than it will pass this on to be added to the Objects respective map.
       * @param geode A Geode Object that you wish to visit
       */
      virtual void apply(osg::Geode& geode)
      {
         if ((dtUtil::Bits::Has(mNodeMask, NodeCollector::GeodeFlag)) && (geode.getName() != mNodeNamesIgnored))
         {
            mNodeManager->AddGeode(geode.getName(), geode);
            dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Added Geode Node: " + geode.getName());
         }
         
         traverse(geode);
      }

      /**
       * Function that Checks to see if the Group Node is a node that we wish to collect and than traverses down the
       * to any of the Group Nodes children.
       * @param group A Group Node Object that will be checked
       */
      virtual void apply(osg::Group& group)
      { 
        //Check if we should add this node using the flag
        if (dtUtil::Bits::Has(mNodeMask, NodeCollector::GroupFlag)  && (group.getName() != mNodeNamesIgnored))
        {   
               //Add Group to node collector's std::map
               mNodeManager->AddGroup(group.getName(), group);
               dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
               logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Added Group Node: " + group.getName());
        }
         //Traverse through the Group Node's Children
         traverse(group);
      }
//////////////////////////////////////////////////////////////////////////////////////////////////////////// 
      /**
      * Function that Checks to see if the Transform Node is a node that we wish to collect and than traverses down the
      * to any of the Transform Nodes children.
      * @param transform A Transform Node Object that will be checked
      */
      virtual void apply(osg::Transform& transform)
      { 
         //Dynamically Cast any Transform Objects found to osgSim::DOFTransform Objects
         osgSim::DOFTransform * DOF = dynamic_cast<osgSim::DOFTransform *>(& transform);
         
         //Check if we should add this node using the flag
         if ((DOF != NULL) && (dtUtil::Bits::Has(mNodeMask, NodeCollector::DOFTransformFlag)) && (transform.getName() != mNodeNamesIgnored))
         {
            //Add DOFTransform to node collector's std::map
            mNodeManager->AddDOFTransform(transform.getName(), (*DOF));
            dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Added DOFTransform Node: " + transform.getName());
         }
         //Traverse through the Transform Node's Children
         traverse(transform);
      }
////////////////////////////////////////////////////////////////////////////////////////////////////////////     
      /**
      * Function that Checks to see if the MatrixTransform Node is a node that we wish to collect and than traverses down the
      * to any of the MatrixTransform Nodes children.
      * @param matrix_transform A MatrixTransform Node Object that will be checked
      */
      virtual void apply(osg::MatrixTransform& matrix_transform)
      { 
         //Check if we should add this node using the flag
         if (dtUtil::Bits::Has(mNodeMask, NodeCollector::MatrixTransformFlag) && (matrix_transform.getName() != mNodeNamesIgnored))
         {
            //Add MatrixTransform to node collector's std::map
            mNodeManager->AddMatrixTransform(matrix_transform.getName(), matrix_transform);
            dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Added MatrixTransform Node: " + matrix_transform.getName());
         }
         //Traverses through the MatrixTransform Node's Children
         traverse(matrix_transform);
      }
////////////////////////////////////////////////////////////////////////////////////////////////////////////     
      /**
       * Function that Checks to see if the Switch Node is a node that we wish to collect and than traverses down the
       * to any of the Switch Nodes children.
       * @param switch A Switch Node Object that will be checked
       */
      virtual void apply(osg::Switch& pSwitch)
      { 
         //Check if we should add this node using the flag
         if (dtUtil::Bits::Has(mNodeMask, NodeCollector::SwitchFlag) && (pSwitch.getName() != mNodeNamesIgnored))
         {
            //Add Switch to node collector's std::map
            mNodeManager->AddSwitch(pSwitch.getName(), pSwitch);
            dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Added Switch Node: " + pSwitch.getName());
         }
         //Traverses through the Switch Node's Children
         traverse(pSwitch);
      }
////////////////////////////////////////////////////////////////////////////////////////////////////////////     
      /**
      * Function that Checks to see if the MultiSwitch Node is a node that we wish to collect and than traverses down the
      * to any of the MultiSwitch Nodes children.
      * @param MultiSwitch A MultiSwitch Node Object that will be checked
      */
      virtual void apply(osgSim::MultiSwitch& pMultiSwitch)
      { 
        //Check if we should add this node using the flag
        if (dtUtil::Bits::Has(mNodeMask, NodeCollector::MultiSwitchFlag) && (pMultiSwitch.getName() != mNodeNamesIgnored))
        {
          //Add MultiSwitch to node collector's std::map
          mNodeManager->AddMultiSwitch(pMultiSwitch.getName(), pMultiSwitch);
          dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
          logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Added MultiSwitch Node: " + pMultiSwitch.getName());
        }
        //Traverses through the MultiSwitch Node's Children
        traverse(pMultiSwitch);
      }

////////////////////////////////////////////////////////////////////////////////////////////////////////////     
      /**
      * Function that Checks to see if the LOD Node is a node that we wish to collect and than traverses down the
      * to any of the LOD Nodes children.
      * @param LOD A LOD Node Object that will be checked
      */
      virtual void apply(osg::LOD& pLOD)
      { 
         //Check if we should add this node using the flag
         if (dtUtil::Bits::Has(mNodeMask, NodeCollector::LODFlag) && (pLOD.getName() != mNodeNamesIgnored))
         {
            //Add MultiSwitch to node collector's std::map
            mNodeManager->AddLOD(pLOD.getName(), pLOD);
            dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
            logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Added LOD Node: " + pLOD.getName());
         }
         //Traverses through the LOD Node's Children
         traverse(pLOD);
      }
   private:
      //Manages the nodes that we wish to collect
      NodeCollector* mNodeManager;
      
      //Represents the types of nodes we wish to collect
      NodeCollector::NodeFlag mNodeMask;

      //Represents a default node name that we do NOT want to collect
      std::string mNodeNamesIgnored;
   };


////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////BEGIN NODE COLLECTOR CLASS DEFINITION////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Default Constructor
   NodeCollector::NodeCollector()
   {
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Constructor that when called will automatically generate the node maps or geode maps that you request  
   NodeCollector::NodeCollector(osg::Node* NodeToLoad, NodeCollector::NodeFlag mask, const std::string& nodeNamesIgnored)
   {
      if (NodeToLoad !=  NULL)
      {
         GroupVisitor mVisitor(this, mask, nodeNamesIgnored);
         NodeToLoad->accept(mVisitor);
      }
      else
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Null Pointer Sent to constructor.");
      }
   }

   //Destructor
   NodeCollector::~NodeCollector()
   {
      ClearAll();
   }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that when called will automatically generate the node maps or geode maps that you request
   void NodeCollector::CollectNodes(osg::Node* NodeToLoad, NodeCollector::NodeFlag mask, const std::string & nodeNamesIgnored)
   {
      if (NodeToLoad !=  NULL)
      {
         GroupVisitor mVisitor(this, mask, nodeNamesIgnored);
         NodeToLoad->accept(mVisitor);
      }
      else
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Null Pointer Sent to function.");
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is defined to clear all the maps of their contents.
   void NodeCollector::ClearAll()
   {
      mGroupNodeMap.clear();
      mTranformNodeMap.clear();
      mMatrixTransformNodeMap.clear();
      mSwitchNodeMap.clear();
      mMultiSwitchNodeMap.clear();
      mGeodeNodeMap.clear();
      mLODNodeMap.clear();
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to add a Geode Node to the Geode Node map
   void NodeCollector::AddGeode(const std::string& key, osg::Geode& data)
   {
      if (!CollectorUtil::AddNode(key, &data, mGeodeNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Can NOT add Geode with Duplicate Key: " + key);
      }

   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Function that is used to remove a Geode Node from the Geode Node map
   void NodeCollector::RemoveGeode(const std::string& key)
   {
      if (!CollectorUtil::RemoveNode(key, mGeodeNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Cannot remove geode node \"" + key + "\" because it does not exist");
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to add a Group Node to the Group Node map
   void NodeCollector::AddGroup(const std::string& key, osg::Group& data)
   {      
      if (!CollectorUtil::AddNode(key, & data, mGroupNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Can Not Add Group Node With Duplicate Key: " + key);
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Function that is used to remove a Group Node from the Group Node map
   void NodeCollector::RemoveGroup(const std::string& key)
   {
      if (!CollectorUtil::RemoveNode(key, mGroupNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Cannot remove group node \"" + key + "\" because it does not exist");
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
   //Function that is used to add a DOFTransform Node to the DOFTransform Node map
   void NodeCollector::AddDOFTransform(const std::string& key, osgSim::DOFTransform& data)
   {
      if (!CollectorUtil::AddNode(key, &data, mTranformNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Can Not Add DOF Node With Duplicate Key: " + key);
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Function that is used to remove a DOFTransform Node from the DOFTransform Node map
   void NodeCollector::RemoveDOFTransform(const std::string& key)
   {
      if (!CollectorUtil::RemoveNode(key, mTranformNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Cannot remove DOF transform node \"" + key + "\" because it does not exist");
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
   //Function that is used to add a MatrixTransform Node to the MatrixTransform Node map
   void NodeCollector::AddMatrixTransform(const std::string& key, osg::MatrixTransform& data)
   {
     if (!CollectorUtil::AddNode(key, &data, mMatrixTransformNodeMap))
     {
        dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
        logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Can Not Add Matrix Transform Node With Duplicate Key: " + key);
     }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Function that is used to remove a MatrixTransform Node from the MatrixTransform Node map
   void NodeCollector::RemoveMatrixTransform(const std::string& key)
   {
      if (!CollectorUtil::RemoveNode(key, mMatrixTransformNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Cannot remove matrix transform node \"" + key + "\" because it does not exist");
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to add a Switch Node to the Switch Node map
   void NodeCollector::AddSwitch(const std::string& key, osg::Switch& data)
   {
      if (!CollectorUtil::AddNode(key, &data, mSwitchNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Can Not Add Switch Node With Duplicate Key: " + key);
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Function that is used to remove a Switch Node from the Switch Node map
   void NodeCollector::RemoveSwitch(const std::string& key)
   {
      if (!CollectorUtil::RemoveNode(key, mSwitchNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Cannot remove switch node \"" + key + "\" because it does not exist");
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to add a MultiSwitch Node to the MultiSwitch Node map
   void NodeCollector::AddMultiSwitch(const std::string& key, osgSim::MultiSwitch& data)
   {
     if (!CollectorUtil::AddNode(key, &data, mMultiSwitchNodeMap))
     {
       dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
       logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Can Not Add MulitSwitch Node With Duplicate Key: " + key);
     }
   }


////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Function that is used to remove a MultiSwitch Node from the MultiSwitch Node map
   void NodeCollector::RemoveMultiSwitch(const std::string& key)
   {
     if (!CollectorUtil::RemoveNode(key, mMultiSwitchNodeMap))
     {
       dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
       logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Cannot remove multi switch node \"" + key + "\" because it does not exist");
     }
   }

   /////////////////////////////////////////////////////////////////////////////
   void NodeCollector::AddLOD(const std::string& key, osg::LOD& data)
   {
      if (!CollectorUtil::AddNode(key, &data, mLODNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Can Not Add LOD Node With Duplicate Key: " + key);
      }
   }


   /////////////////////////////////////////////////////////////////////////////
   void NodeCollector::RemoveLOD(const std::string& key)
   {
      if (!CollectorUtil::RemoveNode(key, mLODNodeMap))
      {
         dtUtil::Log& logger = dtUtil::Log::GetInstance("nodecollector.cpp");
         logger.LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, "Cannot remove LOD node \"" + key + "\" because it does not exist");
      }
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////CONST RETURNS///////////////////////////////////////////////////////
///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a CONST pointer to a Geode Node
   const osg::Geode* NodeCollector::GetGeode(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mGeodeNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a CONST pointer to a Group Node
   const osg::Group* NodeCollector::GetGroup(const std::string& nodeName) const
   {
      // TODO: Consolidate both const and non-const versions of this method.

      const osg::Group* group = CollectorUtil::FindNodePointer(nodeName, mGroupNodeMap);

      // This if-chain is not ideal. However this reduces complexity
      // in the Group visitor and reduces the need for redundant references,
      // in the Group map, such as for other node types that extend Group which
      // already have their own maps.
      // This chain is ordered for most commonly accessed nodes to the least.
      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mTranformNodeMap);
      }

      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mMatrixTransformNodeMap);
      }

      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mMultiSwitchNodeMap);
      }

      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mSwitchNodeMap);
      }

      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mLODNodeMap);
      }

      return group;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a CONST pointer to a DOFTransform Node
   const osgSim::DOFTransform* NodeCollector::GetDOFTransform(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mTranformNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a CONST pointer to a MatrixTransform Node
   const osg::MatrixTransform* NodeCollector::GetMatrixTransform(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mMatrixTransformNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a CONST pointer to a Switch Node
   const osg::Switch* NodeCollector::GetSwitch(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mSwitchNodeMap);
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a CONST pointer to a MultiSwitch Node
   const osgSim::MultiSwitch* NodeCollector::GetMultiSwitch(const std::string& nodeName) const
   {
     return CollectorUtil::FindNodePointer(nodeName, mMultiSwitchNodeMap);
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::LOD* NodeCollector::GetLOD(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mLODNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////NON CONST RETURNS/////////////////////////////////////////////////////
///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a pointer to a Geode Node
   osg::Geode* NodeCollector::GetGeode(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mGeodeNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a pointer to a Group Node
   osg::Group* NodeCollector::GetGroup(const std::string& nodeName)
   {
      // TODO: Consolidate both const and non-const versions of this method.

      osg::Group* group = CollectorUtil::FindNodePointer(nodeName, mGroupNodeMap);

      // This if-chain is not ideal. However this reduces complexity
      // in the Group visitor and reduces the need for redundant references,
      // in the Group map, such as for other node types that extend Group which
      // already have their own maps.
      // This chain is ordered for most commonly accessed nodes to the least.
      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mTranformNodeMap);
      }
      
      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mMatrixTransformNodeMap);
      }
      
      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mMultiSwitchNodeMap);
      }
      
      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mSwitchNodeMap);
      }
      
      if (group == NULL)
      {
         group = CollectorUtil::FindNodePointer(nodeName, mLODNodeMap);
      }

      return group;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a pointer to a DOFTransform Node
   osgSim::DOFTransform* NodeCollector::GetDOFTransform(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mTranformNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a pointer to a MatrixTransform Node
   osg::MatrixTransform* NodeCollector::GetMatrixTransform(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mMatrixTransformNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a pointer to a Switch Node
   osg::Switch* NodeCollector::GetSwitch(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mSwitchNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a pointer to a MultiSwitch Node
   osgSim::MultiSwitch* NodeCollector::GetMultiSwitch(const std::string& nodeName)
   {
     return CollectorUtil::FindNodePointer(nodeName, mMultiSwitchNodeMap);
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::LOD* NodeCollector::GetLOD(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mLODNodeMap);
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////CONST RETURNS///////////////////////////////////////////////////////
//////////////////////////////////////////////Return the Node Maps///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST Group map 
   const NodeCollector::GroupNodeMap& NodeCollector::GetGroupNodeMap() const
   {
      return mGroupNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST DOFTransform map 
   const NodeCollector::TransformNodeMap& NodeCollector::GetTransformNodeMap() const
   {
      return mTranformNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST MatrixTransform map 
   const NodeCollector::MatrixTransformNodeMap& NodeCollector::GetMatrixTransformNodeMap() const
   {
      return mMatrixTransformNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST Switch map 
   const NodeCollector::SwitchNodeMap& NodeCollector::GetSwitchNodeMap() const
   {
      return mSwitchNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST MultiSwitch map 
   const NodeCollector::MultiSwitchNodeMap& NodeCollector::GetMultiSwitchNodeMap() const
   {
     return mMultiSwitchNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST Geode map 
   const NodeCollector::GeodeNodeMap& NodeCollector::GetGeodeNodeMap() const
   {
      return mGeodeNodeMap;
   }

   /////////////////////////////////////////////////////////////////////////////
   const NodeCollector::LODNodeMap& NodeCollector::GetLODNodeMap() const
   {
      return mLODNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////NON CONST RETURNS/////////////////////////////////////////////////////
//////////////////////////////////////////////Return the Node Maps///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a Group map 
   NodeCollector::GroupNodeMap& NodeCollector::GetGroupNodeMap()
   {
      return mGroupNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a DOFTransform map 
   NodeCollector::TransformNodeMap& NodeCollector::GetTransformNodeMap()
   {
      return mTranformNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a MatrixTransform map 
   NodeCollector::MatrixTransformNodeMap& NodeCollector::GetMatrixTransformNodeMap()
   {
      return mMatrixTransformNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a Switch map 
   NodeCollector::SwitchNodeMap& NodeCollector::GetSwitchNodeMap()
   {
      return mSwitchNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a MultiSwitch map 
   NodeCollector::MultiSwitchNodeMap& NodeCollector::GetMultiSwitchNodeMap()
   {
     return mMultiSwitchNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

   ////Function that returns a Geode map 
   NodeCollector::GeodeNodeMap& NodeCollector::GetGeodeNodeMap()
   {
      return mGeodeNodeMap;
   }

   /////////////////////////////////////////////////////////////////////////////
   NodeCollector::LODNodeMap& NodeCollector::GetLODNodeMap()
   {
      return mLODNodeMap;
   }

////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   

 }//namespace dtUtil

 DT_DISABLE_WARNING_END
