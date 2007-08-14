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

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/nodecollector.h>
#include <dtCore/collectorutil.h>
#include <dtUtil/log.h>
#include <dtUtil/bits.h>

#include <osg/Node>
#include <osg/MatrixTransform>
#include <osgSim/DOFTransform>
#include <osg/Switch>
#include <osg/Drawable>
#include <osg/Geode>
#include <osg/Material>


namespace dtCore
{
   //Define all the different flags so that users can OR the variables together to create a mask
   const NodeCollector::NodeFlag NodeCollector::GroupFlag = dtUtil::Bits::Add(0,1);
   const NodeCollector::NodeFlag NodeCollector::DOFTransformFlag = dtUtil::Bits::Add(0,2);
   const NodeCollector::NodeFlag NodeCollector::MatrixTransformFlag = dtUtil::Bits::Add(0,4);
   const NodeCollector::NodeFlag NodeCollector::SwitchFlag = dtUtil::Bits::Add(0,8);
   const NodeCollector::NodeFlag NodeCollector::GeodeFlag = dtUtil::Bits::Add(0,16);

   //A Flag that will return all nodes and Drawable / Material objects that it finds
   const NodeCollector::NodeFlag NodeCollector::AllNodeTypes = (NodeCollector::GroupFlag | 
                                                                NodeCollector::DOFTransformFlag | 
                                                                NodeCollector::MatrixTransformFlag | 
                                                                NodeCollector::SwitchFlag |
                                                                NodeCollector::GeodeFlag);
  
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      /**
       * Function that traverses through a geode and visits all of the Drawable Objects associated with the Geode and 
       * the Material Objects that are associated with the Drawable Objects.  If it finds an Object that it is looking
       * for than it will pass this on to be added to the Objects respective map.
       * @param geode A Geode Object that you wish to visit
       */
      virtual void apply(osg::Geode& geode)
      {
         if((dtUtil::Bits::Has(mNodeMask, NodeCollector::GeodeFlag)) && (geode.getName() != mNodeNamesIgnored))
         {
            mNodeManager->AddGeode(geode.getName(), geode);
            LOG_DEBUG("Added Geode Node: " + geode.getName());
         }
         
         traverse(geode);
      }

      /**
       * Function that Checks to see if the Group Node is a node that we wish to collect and than traverses down the
       * to any of the Group Nodes children.
       * @param group A Group Node Object that will be checked
       */
      virtual void apply(osg::Group & group)
      { 
        //Check if we should add this node using the flag
        if( dtUtil::Bits::Has(mNodeMask, NodeCollector::GroupFlag)  && ( group.getName() != mNodeNamesIgnored ) )
        {   
               //Add Group to node collector's std::map
               mNodeManager->AddGroup(group.getName(), group);
               LOG_DEBUG("Added Group Node: " + group.getName());
        }
         //Traverse through the Group Nodes Children
         traverse(group);
      }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
      /**
      * Function that Checks to see if the Transform Node is a node that we wish to collect and than traverses down the
      * to any of the Transform Nodes children.
      * @param transform A Transform Node Object that will be checked
      */
      virtual void apply(osg::Transform & transform)
      { 
         //Dynamically Cast any Transform Objects found to osgSim::DOFTransform Objects
         osgSim::DOFTransform * DOF = dynamic_cast<osgSim::DOFTransform *>(& transform);
         
         //Check if we should add this node using the flag
         if( (DOF != NULL) && (dtUtil::Bits::Has(mNodeMask, NodeCollector::DOFTransformFlag)) && ( transform.getName() != mNodeNamesIgnored ) )
         {
            //Add DOFTransform to node collector's std::map
            mNodeManager->AddDOFTransform(transform.getName(), (*DOF));
            LOG_DEBUG("Added DOFTransform Node: " + transform.getName());
         }
         //Traverse through the Transform Nodes Children
         traverse(transform);
      }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////     
      /**
      * Function that Checks to see if the MatrixTransform Node is a node that we wish to collect and than traverses down the
      * to any of the MatrixTransform Nodes children.
      * @param matrix_transform A MatrixTransform Node Object that will be checked
      */
      virtual void apply(osg::MatrixTransform & matrix_transform)
      { 
         //Check if we should add this node using the flag
         if( dtUtil::Bits::Has(mNodeMask, NodeCollector::MatrixTransformFlag) && ( matrix_transform.getName() != mNodeNamesIgnored ) )
         {
            //Add MatrixTransform to node collector's std::map
            mNodeManager->AddMatrixTransform(matrix_transform.getName(), matrix_transform);
            LOG_DEBUG("Added MatrixTransform Node: " + matrix_transform.getName());
         }
         //Traverses through the MatrixTransform Nodes Children
         traverse(matrix_transform);
      }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////     
      /**
      * Function that Checks to see if the Switch Node is a node that we wish to collect and than traverses down the
      * to any of the Switch Nodes children.
      * @param switch A Switch Node Object that will be checked
      */
      virtual void apply(osg::Switch & pSwitch)
      { 
         //Check if we should add this node using the flag
         if( dtUtil::Bits::Has(mNodeMask, NodeCollector::SwitchFlag) && ( pSwitch.getName() != mNodeNamesIgnored ) )
         {
            //Add Switch to node collector's std::map
             mNodeManager->AddSwitch(pSwitch.getName(), pSwitch);
             LOG_DEBUG("Added Switch Node: " + pSwitch.getName());
         }
         //Traverses through the Switch Nodes Children
         traverse(pSwitch);
      }

   private:
      //Manages the nodes that we wish to collect
      NodeCollector* mNodeManager;
      
      //Represents the types of nodes we wish to collect
      NodeCollector::NodeFlag mNodeMask;

      //Represents a default node name that we do NOT want to collect
      std::string mNodeNamesIgnored;
   };


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////BEGIN NODE COLLECTOR CLASS DEFINITION////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Default Constructor
   NodeCollector::NodeCollector()
   {

   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Constructor that when called will automatically generate the node maps or geode maps that you request  
   NodeCollector::NodeCollector(osg::Node* NodeToLoad, NodeCollector::NodeFlag mask, const std::string & nodeNamesIgnored)
   {
      if(NodeToLoad !=  NULL)
      {
         GroupVisitor mVisitor(this, mask, nodeNamesIgnored);
         NodeToLoad->accept(mVisitor);
      }
      else
      {
         LOG_ERROR("Null Pointer Sent to constructor.");
      }
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that when called will automatically generate the node maps or geode maps that you request
   void NodeCollector::CollectNodes(osg::Node* NodeToLoad, NodeCollector::NodeFlag mask, const std::string & nodeNamesIgnored)
   {
      if(NodeToLoad !=  NULL)
      {
         GroupVisitor mVisitor(this, mask, nodeNamesIgnored);
         NodeToLoad->accept(mVisitor);
      }
      else
      {
         LOG_ERROR("Null Pointer Sent to function.");
      }
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is defined to clear all the maps of their contents.
   void NodeCollector::ClearAll()
   {
      mGroupNodeMap.clear();
      mTranformNodeMap.clear();
      mMatrixTransformNodeMap.clear();
      mSwitchNodeMap.clear();
      mGeodeNodeMap.clear();
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to add a Geode Node to the Geode Node map
   void NodeCollector::AddGeode(const std::string& key, osg::Geode& data)
   {
      if( !CollectorUtil::AddNode(key, &data, mGeodeNodeMap) )
      {
         LOG_WARNING("Can NOT add Geode with Duplicate Key: " + key);
      }

   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to add a Group Node to the Group Node map
   void NodeCollector::AddGroup(const std::string & key, osg::Group & data)
   {      
      if( !CollectorUtil::AddNode(key, & data, mGroupNodeMap) )
      {
         LOG_WARNING("Can Not Add Node With Duplicate Key: " + key);
      }
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
   //Function that is used to add a DOFTransform Node to the DOFTransform Node map
   void NodeCollector::AddDOFTransform(const std::string & key, osgSim::DOFTransform & data)
   {
      if ( !CollectorUtil::AddNode(key, & data, mTranformNodeMap) )
      {
         LOG_WARNING("Can Not Add Node With Duplicate Key: " + key);
      }
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
   //Function that is used to add a MatrixTransform Node to the MatrixTransform Node map
   void NodeCollector::AddMatrixTransform(const std::string & key, osg::MatrixTransform & data)
   {
     if( !CollectorUtil::AddNode(key, & data, mMatrixTransformNodeMap) )
     {
        LOG_WARNING("Can Not Add Node With Duplicate Key: " + key);
     }
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to add a Switch Node to the Switch Node map
   void NodeCollector::AddSwitch(const std::string & key, osg::Switch & data)
   {
      if( !CollectorUtil::AddNode(key, & data, mSwitchNodeMap) )
      {
         LOG_WARNING("Can Not Add Node With Duplicate Key: " + key);
      }
   }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////CONST RETURNS///////////////////////////////////////////////////////
///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a CONST pointer to a Geode Node
   const osg::Geode* NodeCollector::GetGeode(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mGeodeNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a CONST pointer to a Group Node
   const osg::Group* NodeCollector::GetGroup(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mGroupNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a CONST pointer to a DOFTransform Node
   const osgSim::DOFTransform* NodeCollector::GetDOFTransform(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mTranformNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a CONST pointer to a MatrixTransform Node
   const osg::MatrixTransform* NodeCollector::GetMatrixTransform(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mMatrixTransformNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a CONST pointer to a Switch Node
   const osg::Switch* NodeCollector::GetSwitch(const std::string& nodeName) const
   {
      return CollectorUtil::FindNodePointer(nodeName, mSwitchNodeMap);
   }
   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////NON CONST RETURNS/////////////////////////////////////////////////////
///////////////////////////////////ADDED SUPPORT FOR DRAWABLES AND MATERIALS/////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a pointer to a Geode Node
   osg::Geode* NodeCollector::GetGeode(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mGeodeNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that is used to request a pointer to a Group Node
   osg::Group* NodeCollector::GetGroup(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mGroupNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a pointer to a DOFTransform Node
   osgSim::DOFTransform* NodeCollector::GetDOFTransform(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mTranformNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a pointer to a MatrixTransform Node
   osg::MatrixTransform* NodeCollector::GetMatrixTransform(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mMatrixTransformNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Function that is used to request a pointer to a Switch Node
   osg::Switch* NodeCollector::GetSwitch(const std::string& nodeName)
   {
      return CollectorUtil::FindNodePointer(nodeName, mSwitchNodeMap);
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////CONST RETURNS///////////////////////////////////////////////////////
//////////////////////////////////////////////Return the Node Maps///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST Group map 
   const NodeCollector::GroupNodeMap& NodeCollector::GetGroupNodeMap() const
   {
      return mGroupNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST DOFTransform map 
   const NodeCollector::TransformNodeMap& NodeCollector::GetTransformNodeMap() const
   {
      return mTranformNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST MatrixTransform map 
   const NodeCollector::MatrixTransformNodeMap& NodeCollector::GetMatrixTransformNodeMap() const
   {
      return mMatrixTransformNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST Switch map 
   const NodeCollector::SwitchNodeMap& NodeCollector::GetSwitchNodeMap() const
   {
      return mSwitchNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a CONST Geode map 
   const NodeCollector::GeodeNodeMap& NodeCollector::GetGeodeNodeMap() const
   {
      return mGeodeNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////NON CONST RETURNS/////////////////////////////////////////////////////
//////////////////////////////////////////////Return the Node Maps///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a Group map 
   NodeCollector::GroupNodeMap& NodeCollector::GetGroupNodeMap()
   {
      return mGroupNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a DOFTransform map 
   NodeCollector::TransformNodeMap& NodeCollector::GetTransformNodeMap()
   {
      return mTranformNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a MatrixTransform map 
   NodeCollector::MatrixTransformNodeMap& NodeCollector::GetMatrixTransformNodeMap()
   {
      return mMatrixTransformNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //Function that returns a Switch map 
   NodeCollector::SwitchNodeMap& NodeCollector::GetSwitchNodeMap()
   {
      return mSwitchNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   ////Function that returns a Geode map 
   NodeCollector::GeodeNodeMap& NodeCollector::GetGeodeNodeMap()
   {
      return mGeodeNodeMap;
   }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   //Destructor
   NodeCollector::~NodeCollector()
   {

   }

 }//namespace dtCore
