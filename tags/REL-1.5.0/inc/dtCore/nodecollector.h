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
#ifndef DELTA_DOF_CONTAINER
#define DELTA_DOF_CONTAINER

#include <osg/Node>
#include <osg/MatrixTransform>
#include <osgSim/DOFTransform>
#include <osgSim/MultiSwitch>
#include <dtUtil/enumeration.h>

#include <dtCore/refptr.h>
#include <dtCore/export.h>

#include <list>

namespace dtUtil
{
   class Log;
}

namespace dtCore
{
      class DT_CORE_EXPORT NodeCollectorTypes : public dtUtil::Enumeration
      {
         DECLARE_ENUM(NodeCollectorTypes);

         public:

            // DOFS For movement of articulation parts
            static const NodeCollectorTypes DOF_LOAD_NODE_COLLECTOR_TYPE;
            
            // HOT SPOTS for offsets / placement helpers
            static const NodeCollectorTypes HOT_SPOT_LOAD_NODE_COLLECTOR_TYPE;

            // MultiSwitches for selecting different model states
            static const NodeCollectorTypes MULTISWITCH_NODE_COLLECTOR_TYPE;

            // LOAD both DOFS and HOT SPOTS
            static const NodeCollectorTypes LOAD_ALL_NODE_TYPES;

         protected:
            NodeCollectorTypes(const std::string &name) : dtUtil::Enumeration(name)
            {
               AddInstance(this);
            }
      };


   /** The NodeCollector is used to store particular nodes given a piece of scene
     * graph.  Once the graph is searched, you can retrieve the particular node
     * types by name or by index.
     * To use:
     * @code
     * myObj->LoadFile("myTank.ive");
     * RefPtr<NodeCollector> coll = new NodeCollector( myObj->GetOSGNode() );
     * osgSim::DOFTransform *turret = coll->GetDOFByName("turretDOF");
     * @endcode
     *
     * You can also do a more manual approach, like this:
     * @code
     * RefPtr<NodeCollector> coll = new NodeCollector(NULL);
     * coll->AddHotSpotsFromModelNode( myObj->GetOSGNode(), 'x' );
     * const osg::MatrixTransform* xform = coll->GetHotSpotByName("x_marks_the_spot");
     * @endcode
     *
     * @note Destructor will empty all containers.
     */
   class DT_CORE_EXPORT NodeCollector : public osg::Referenced
   {
      public:
         /// Constructor
         NodeCollector(osg::Node* NodeToLoad, const NodeCollectorTypes& type = NodeCollectorTypes::LOAD_ALL_NODE_TYPES, bool LogErroneus = false);

         /**
         * Function : AddDOFSFromModelNode
         * Purpose  : Fill in the std::list full of all the models dofs
         * @param   : osg::Node *nodepath - called from a dtCore::Object->GetOSGNode 
         *            after loaded in
         * Outs     : member variable std::list filled in
         * @return  : NONE
         */
         void AddDOFSFromModelNode(osg::Node *nodepath);
         
         /**
         * Function : GetDOFAtPosition
         * Purpose  : To get a dof for use
         * @param   : loc - where in array of dof you want
         * Outs     : NONE
         * @return  : osgSim::DOFTransform* that you will modify
         */
         osgSim::DOFTransform* GetDOFAtPosition(const unsigned int loc);
      
         /**
         * Function : GetDOFByName
         * Purpose  : Get a dof for use
         * @param   : std::string &DofName - name of dof to get
         * Outs     : NONE
         * @return  : osgSim::DOFTransform* that you will modify
         */
         osgSim::DOFTransform* GetDOFByName(const std::string &DofName);
       
         /**
         * Function : GetDOFListSize
         * Purpose  : Get the amounts of dof active
         * @param   : NONE
         * Outs     : NONE
         * @return  : Size of the std::list
         */
         const unsigned int GetDOFListSize();
        
         /**
         * Function : CleanDofTransformList
         * Purpose  : Clears the list out
         * @param   : NONE
         * Outs     : list cleared called
         * @return  : N/A
         */
         void CleanDofTransformList();

         /**
         * Collects all osg::MatrixTransform nodes that have names beginning with 
         * the supplied character.
         * @param osg::Node *nodepath : usually retrieved from a dtCore::Object::GetOSGNode()
         *            after a file is loaded in
         * @param TestLetter : The letter to check MatrixTransform node names with.
         */
         void AddHotSpotsFromModelNode(osg::Node *nodepath, char TestLetter = 'h');
         
         /**
         * Function : GetHotSpotAtPosition
         * Purpose  : To get a hot spot for use
         * @param   : loc - where in array of dof you want
         * Outs     : NONE
         * @return  : osg::MatrixTransform* that you will modify
         */
         const osg::MatrixTransform* GetHotSpotAtPosition(const unsigned int loc);
      
         /**
         * Function : GetHotSpotByName
         * Purpose  : Get a hot spot for use
         * @param   : std::string &HotSpotName - name of hot spot to get
         * Outs     : NONE
         * @return  :osg::MatrixTransform that you will modify
         */
          const osg::MatrixTransform* GetHotSpotByName(const std::string &HotSpotName);

         /**
         * Function : GetHotSpotListSize
         * Purpose  : Get the amounts of hot spots active
         * @param   : NONE
         * Outs     : NONE
         * @return  : Size of the std::list
         */
         const unsigned int GetHotSpotListSize();
         
         /**
         * Function : CleanHotSpotList
         * Purpose  : Clears the list out
         * @param   : NONE
         * Outs     : list cleared called
         * @return  : N/A
         */
         void CleanHotSpotList();

         /**
         * Function : AddMultiSwitchesFromModelNode
         * Purpose  : Fill in the std::list full of all the models MultiSwitches
         * @param   : osg::Node *nodepath - called from a dtCore::Object->GetOSGNode 
         *            after loaded in
         * Outs     : member variable std::list filled in
         * @return  : NONE
         */
         void AddMultiSwitchesFromModelNode(osg::Node *nodepath);
         
         /**
         * Function : GetMultiSwitchAtPosition
         * Purpose  : To get a MultiSwitch for use
         * @param   : loc - where in array of MultiSwitch you want
         * Outs     : NONE
         * @return  : osgSim::MultiSwitch* that you will modify
         */
         osgSim::MultiSwitch* GetMultiSwitchAtPosition(const unsigned int loc);
      
         /**
         * Function : GetMultiSwitchByName
         * Purpose  : Get a MultiSwitch for use
         * @param   : std::string &MultiSwitchName - name of dof to get
         * Outs     : NONE
         * @return  : osgSim::MultiSwitch* that you will modify
         */
         osgSim::MultiSwitch* GetMultiSwitchByName(const std::string &MultiSwitchName);
       
         /**
         * Function : GetMultiSwitchListSize
         * Purpose  : Get the amounts of dof active
         * @param   : NONE
         * Outs     : NONE
         * @return  : Size of the std::list
         */
         const unsigned int GetMultiSwitchListSize();
        
         /**
         * Function : CleanMultiSwitchList
         * Purpose  : Clears the list out
         * @param   : NONE
         * Outs     : list cleared called
         * @return  : N/A
         */
         void CleanMultiSwitchList();

         /**
         * Function  : SetNodeCollectorFlag
         * @param    : The type you want it to be.
         * @return  : N/A
         */
         void SetNodeCollectorFlag(const NodeCollectorTypes& type) {NodeCollectorFlag = &type;}

         /**
         * Function  : GetNodeCollectorFlag
         * @return  :  Return the node flag as an int
         */
         const NodeCollectorTypes& GetNodeCollectorFlag() {return *NodeCollectorFlag;}

      protected:
         /// Destructor
         virtual ~NodeCollector();

         dtUtil::Log* mLogger;

      private:
         /**
         * Var Name  : mDOFTransforms
         * Type      : std::list<osgSim::DOFTransform>
         * Purpose   : Holds onto our dofs loaded in
         */
         std::list<dtCore::RefPtr<osgSim::DOFTransform> > mDOFTransforms;

         /**
         * Var Name  : mHotSpots
         * Type      : std::list<osgSim::MatrixTransform>
         * Purpose   : Holds onto our hot spot data
         */
         std::list<dtCore::RefPtr<osg::MatrixTransform> > mHotSpots;

         /**
         * Var Name  : mMultiSwitches
         * Type      : std::list<osgSim::MultiSwitch>
         * Purpose   : Holds onto our multiSwitch data
         */
         std::list<dtCore::RefPtr<osgSim::MultiSwitch> > mMultiSwitches;

         /**
         * /brief   : Purpose - Logs a message once if enabled throws onto this list
         */
         std::list<std::string> mLoggedMessages;

         /// Set once at class constructor
         bool mIsLoggingEnabled;

         /// Classes Util Flag
         const NodeCollectorTypes* NodeCollectorFlag;
   };
} // namespace

#endif