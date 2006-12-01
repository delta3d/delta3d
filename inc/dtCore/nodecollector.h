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
#include <osgSim/DOFTransform>
#include <osg/MatrixTransform>
#include <dtCore/refptr.h>
#include <dtCore/export.h>
#include <dtUtil/enumeration.h>
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

            // LOAD both DOFS and HOT SPOTS
            static const NodeCollectorTypes LOAD_ALL_NODE_TYPES;

         protected:
            NodeCollectorTypes(const std::string &name) : dtUtil::Enumeration(name)
            {
               AddInstance(this);
            }
      };

   /**
   * Class Name   : NodeCollector
   * Derives From : NONE
   * Purpose      : To hold onto dofs and other model nodes of an object loaded in for referencing
   * /brief       : Make a new  class, call load function from model node,
                     then get nodes by name or position for your use.
   * Other Notes  : Destructor clean list, dont have to manually do so
   *                unless you want to reuse the class
   */
   class DT_CORE_EXPORT NodeCollector : public osg::Referenced
   {
      public:
         /// Constructor
         NodeCollector(osg::Node* NodeToLoad, const NodeCollectorTypes& type = NodeCollectorTypes::LOAD_ALL_NODE_TYPES, bool LogErroneus = false);

      protected:
         /// Destructor
         virtual ~NodeCollector();

      public:
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
         * Function : AddHotSpotsFromModelNode
         * Purpose  : Fill in the std::list full of all the models hot spots
         * @param   : osg::Node *nodepath - called from a dtCore::Object->GetOSGNode 
         *            after loaded in
         * Outs     : member variable std::list filled in
         * @return  : NONE
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
         * /brief   : Purpose - Logs a message once if enabled throws onto this list
         */
         std::list<std::string> mLoggedMessages;

         /// Set once at class constructor
         bool mIsLoggingEnabled;

         /// Classes Util Flag
         const NodeCollectorTypes* NodeCollectorFlag;

      protected:
         dtUtil::Log* mLogger;
   };
} // namespace

#endif
