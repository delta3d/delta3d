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
   const NodeCollectorTypes NodeCollectorTypes::LOAD_ALL_NODE_TYPES("LOAD_ALL_NODE_TYPES");
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   NodeCollector::NodeCollector(osg::Node* NodeToLoad,const NodeCollectorTypes& type, bool LogErroneus)
   {
      mIsLoggingEnabled = LogErroneus;
      mLogger = &dtUtil::Log::GetInstance("NodeCollector.cpp");
      CleanDofTransformList();
      CleanHotSpotList();

      SetNodeCollectorFlag(type);
      if(NodeCollectorTypes::DOF_LOAD_NODE_COLLECTOR_TYPE == type)
      {
         AddDOFSFromModelNode(NodeToLoad);
      }
      else if(NodeCollectorTypes::HOT_SPOT_LOAD_NODE_COLLECTOR_TYPE == type)
      {
         AddHotSpotsFromModelNode(NodeToLoad);
      }
      else if(NodeCollectorTypes::LOAD_ALL_NODE_TYPES == type)
      {
         AddDOFSFromModelNode(NodeToLoad);
         AddHotSpotsFromModelNode(NodeToLoad);
      }
   }

   NodeCollector::~NodeCollector()
   {
      CleanHotSpotList();
      CleanDofTransformList();
   }

   void NodeCollector::AddDOFSFromModelNode(osg::Node *nodepath)
   {
      osg::Group* currGroup = NULL;

      // check to see if we have a valid (non-NULL) node.
      // if we do have a null node, return NULL.
      if (!nodepath) 
         return;
      
      // We have a valid node, check to see if this is the node we 
      // are looking for. If so, return the current node.
      osgSim::DOFTransform *dofXform = dynamic_cast<osgSim::DOFTransform*>(nodepath);
      if(dofXform)
      {
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
         if((*iter)->getName().c_str() == DofName)
            return iter->get();
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
      if (!nodepath) 
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
}
