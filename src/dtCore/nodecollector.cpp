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
 * Allen Danklefsen -Morgas on forums.
 */

#include <prefix/dtcoreprefix-src.h>
#include <dtCore/nodecollector.h>
#include <dtUtil/log.h>

namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(NodeCollectorTypes);
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   const NodeCollectorTypes NodeCollectorTypes::DOF_LOAD_NODE_COLLECTOR_TYPE("DOF_LOAD_NODE_COLLECTOR_TYPE");
   const NodeCollectorTypes NodeCollectorTypes::HOT_SPOT_LOAD_NODE_COLLECTOR_TYPE("HOT_SPOT_LOAD_NODE_COLLECTOR_TYPE");
   const NodeCollectorTypes NodeCollectorTypes::MULTISWITCH_NODE_COLLECTOR_TYPE("MULTISWITCH_NODE_COLLECTOR_TYPE");
   const NodeCollectorTypes NodeCollectorTypes::LOAD_ALL_NODE_TYPES("LOAD_ALL_NODE_TYPES");
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   NodeCollector::NodeCollector(osg::Node* NodeToLoad,const NodeCollectorTypes& type, bool LogErroneus)
   {
      mIsLoggingEnabled = LogErroneus;
      mLogger = &dtUtil::Log::GetInstance("NodeCollector.cpp");
      CleanDofTransformList();
      CleanHotSpotList();
      CleanMultiSwitchList();

      SetNodeCollectorFlag(type);
      if(NodeCollectorTypes::DOF_LOAD_NODE_COLLECTOR_TYPE == type)
      {
         AddDOFSFromModelNode(NodeToLoad);
      }
      else if(NodeCollectorTypes::HOT_SPOT_LOAD_NODE_COLLECTOR_TYPE == type)
      {
         AddHotSpotsFromModelNode(NodeToLoad);
      }
      else if(NodeCollectorTypes::MULTISWITCH_NODE_COLLECTOR_TYPE == type)
      {
         AddMultiSwitchesFromModelNode(NodeToLoad);
      }
      else if(NodeCollectorTypes::LOAD_ALL_NODE_TYPES == type)
      {
         AddDOFSFromModelNode(NodeToLoad);
         AddHotSpotsFromModelNode(NodeToLoad);
         AddMultiSwitchesFromModelNode(NodeToLoad);
      }
   }

   NodeCollector::~NodeCollector()
   {
      CleanHotSpotList();
      CleanDofTransformList();
      CleanMultiSwitchList();
   }

   void NodeCollector::AddDOFSFromModelNode(osg::Node *nodepath)
   {
      osg::Group* currGroup = NULL;

      // check to see if we have a valid (non-NULL) node.
      // if we do have a null node, return NULL.
      if (nodepath==NULL) 
         return;
      
      // We have a valid node, check to see if this is the node we 
      // are looking for. If so, return the current node.
      osgSim::DOFTransform *dofXform = dynamic_cast<osgSim::DOFTransform*>(nodepath);
      if(dofXform)
      {
         //std::cout << "NodeCollector found DOF TRANSFORM: Class Name [" <<  dofXform->className() << 
         //   "], Node Name [" << dofXform->getName() << "]" << std::endl;
         mDOFTransforms.push_back(dofXform);
      }

      // We have a valid node, but not the one we are looking for.
      // Check to see if it has children (non-leaf node). If the node
      // has children, check each of the child nodes by recursive call.
      // If one of the recursive calls returns a non-null value we have
      // found the correct node, so return this node.
      // If we check all of the children and have not found the node,
      // return NULL
      currGroup = nodepath->asGroup(); // returns NULL if not a group.
      if ( currGroup ) 
      {
         for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
         { 
            AddDOFSFromModelNode(currGroup->getChild(i));
         }
         return; // We have checked each child node - no match found.
      }
      else 
      {
         return; // leaf node, no match 
      }
   }

   osgSim::DOFTransform* NodeCollector::GetDOFAtPosition(const unsigned int loc)
   {
      if(loc > GetDOFListSize())
         return NULL;
      
      std::list<dtCore::RefPtr<osgSim::DOFTransform> >::iterator iter = mDOFTransforms.begin();

      unsigned int i = 0;
      for(;iter != mDOFTransforms.end(); ++iter)
      {
         if(i == loc)
            return iter->get();
         i++;
      }
      return NULL;
   }

   osgSim::DOFTransform* NodeCollector::GetDOFByName(const std::string &DofName)
   {
      std::list<dtCore::RefPtr<osgSim::DOFTransform> >::iterator iter = mDOFTransforms.begin();
      for(;iter != mDOFTransforms.end(); ++iter)
      {
         osgSim::DOFTransform *dofTransform = (*iter).get();
         if(dofTransform->getName().c_str() == DofName)
            return dofTransform;
      }
      if(mIsLoggingEnabled)
      {
         std::list<std::string>::iterator stringiter = mLoggedMessages.begin();
         bool toContinue = true;
         for(;stringiter != mLoggedMessages.end(); ++stringiter)
         {
         if((*stringiter) == DofName)
            toContinue = false;
         }
         if(toContinue)
         {
            if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               // error message here
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                 "Could not find dof by name %s\n",
                                 DofName.c_str());
               mLoggedMessages.push_back(DofName);   
            }
         }
      }
      return NULL;   
   }

   const unsigned int NodeCollector::GetDOFListSize()
   {
      return mDOFTransforms.size();
   }

   void NodeCollector::CleanDofTransformList()
   {
      mDOFTransforms.clear();
   }

   void NodeCollector::AddHotSpotsFromModelNode(osg::Node *nodepath, char TestLetter)
   {
      osg::Group* currGroup = NULL;

      // check to see if we have a valid (non-NULL) node.
      // if we do have a null node, return NULL.
      if (nodepath==NULL) 
         return;
      
      // We have a valid node, check to see if this is the node we 
      // are looking for. If so, return the current node.
      
      if((currGroup = dynamic_cast<osg::Group*>(nodepath)) != NULL)
      {
         if(currGroup->getName()[0] == TestLetter)
         {
            osg::MatrixTransform *matrixform = dynamic_cast<osg::MatrixTransform*>(nodepath);
            mHotSpots.push_back(matrixform);
         }
      }

      // We have a valid node, but not the one we are looking for.
      // Check to see if it has children (non-leaf node). If the node
      // has children, check each of the child nodes by recursive call.
      // If one of the recursive calls returns a non-null value we have
      // found the correct node, so return this node.
      // If we check all of the children and have not found the node,
      // return NULL
      currGroup = nodepath->asGroup(); // returns NULL if not a group.
      if ( currGroup ) 
      {
         for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
         { 
            AddHotSpotsFromModelNode(currGroup->getChild(i), TestLetter);
         }
         return; // We have checked each child node - no match found.
      }
      else 
      {
         return; // leaf node, no match 
      }
   }

   const osg::MatrixTransform* NodeCollector::GetHotSpotAtPosition(const unsigned int loc)
   {
      if(loc > GetHotSpotListSize())
         return NULL;

      std::list<dtCore::RefPtr<osg::MatrixTransform> >::iterator iter = mHotSpots.begin();

      unsigned int i = 0;
      for(;iter != mHotSpots.end(); ++iter)
      {
         if(i == loc)
            return iter->get();
         i++;
      }
      return NULL;
   }

   const osg::MatrixTransform* NodeCollector::GetHotSpotByName(const std::string &HotSpotName)
   {
      std::list<dtCore::RefPtr<osg::MatrixTransform> >::iterator iter = mHotSpots.begin();
      for(;iter != mHotSpots.end(); ++iter)
      {
         if((*iter)->getName().c_str() == HotSpotName)
            return iter->get();
      }
      if(mIsLoggingEnabled)
      {
         std::list<std::string>::iterator stringiter = mLoggedMessages.begin();
         bool toContinue = true;
         for(;stringiter != mLoggedMessages.end(); ++stringiter)
         {
         if((*stringiter) == HotSpotName)
            toContinue = false;
         }
         if(toContinue)
         {
            if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               // error message here
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                 "Could not find hot spot by name %s\n",
                                 HotSpotName.c_str());
               mLoggedMessages.push_back(HotSpotName);   
            }
         }
      }
      return NULL;   
   }

   const unsigned int NodeCollector::GetHotSpotListSize()
   {
      return mHotSpots.size();
   }

   void NodeCollector::CleanHotSpotList()
   {
      mHotSpots.clear();
   }

   void NodeCollector::AddMultiSwitchesFromModelNode(osg::Node *nodepath)
   {
      osg::Group* currGroup = NULL;

      // check to see if we have a valid (non-NULL) node.
      // if we do have a null node, return NULL.
      if (nodepath==NULL) 
         return;
      
      // We have a valid node, check to see if this is the node we 
      // are looking for. If so, return the current node.
      osgSim::MultiSwitch *multiswitch = dynamic_cast<osgSim::MultiSwitch*>(nodepath);
      if(multiswitch)
      {
         //std::cout << "NodeCollector found MULTISWITCH: Class Name [" <<  multiswitch->className() << 
         //   "], Node Name [" << multiswitch->getName() << "]" << std::endl;
         mMultiSwitches.push_back(multiswitch);
      }

      // We have a valid node, but not the one we are looking for.
      // Check to see if it has children (non-leaf node). If the node
      // has children, check each of the child nodes by recursive call.
      // If one of the recursive calls returns a non-null value we have
      // found the correct node, so return this node.
      // If we check all of the children and have not found the node,
      // return NULL
      currGroup = nodepath->asGroup(); // returns NULL if not a group.
      if ( currGroup ) 
      {
         for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
         { 
            AddMultiSwitchesFromModelNode(currGroup->getChild(i));
         }
         return; // We have checked each child node - no match found.
      }
      else 
      {
         return; // leaf node, no match 
      }
   }

   osgSim::MultiSwitch* NodeCollector::GetMultiSwitchAtPosition(const unsigned int loc)
   {
      if(loc > GetMultiSwitchListSize())
         return NULL;
      
      std::list<dtCore::RefPtr<osgSim::MultiSwitch> >::iterator iter = mMultiSwitches.begin();

      unsigned int i = 0;
      for(;iter != mMultiSwitches.end(); ++iter)
      {
         if(i == loc)
            return iter->get();
         i++;
      }
      return NULL;
   }

   osgSim::MultiSwitch* NodeCollector::GetMultiSwitchByName(const std::string &MultiSwitchName)
   {
      std::list<dtCore::RefPtr<osgSim::MultiSwitch> >::iterator iter = mMultiSwitches.begin();
      for(;iter != mMultiSwitches.end(); ++iter)
      {
         osgSim::MultiSwitch *multiswitch = (*iter).get();
         if(multiswitch->getName().c_str() == MultiSwitchName)
            return multiswitch;
      }
      if(mIsLoggingEnabled)
      {
         std::list<std::string>::iterator stringiter = mLoggedMessages.begin();
         bool toContinue = true;
         for(;stringiter != mLoggedMessages.end(); ++stringiter)
         {
         if((*stringiter) == MultiSwitchName)
            toContinue = false;
         }
         if(toContinue)
         {
            if(mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               // error message here
               mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                                 "Could not find MultiSwitch by name %s\n",
                                 MultiSwitchName.c_str());
               mLoggedMessages.push_back(MultiSwitchName);   
            }
         }
      }
      return NULL;   
   }

   const unsigned int NodeCollector::GetMultiSwitchListSize()
   {
      return mMultiSwitches.size();
   }

   void NodeCollector::CleanMultiSwitchList()
   {
      mMultiSwitches.clear();
   }
}
