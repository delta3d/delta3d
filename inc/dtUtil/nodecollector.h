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

#ifndef DELTA_NODE_COLLECTOR
#define DELTA_NODE_COLLECTOR

#include <dtCore/observerptr.h>
#include <dtUtil/export.h>

#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_ALL_START
#include <osg/Referenced>
DT_DISABLE_WARNING_END
#include <map>
#include <string>

//Forward Declare the necessary osg classes that will be used by the NodeCollector class
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
   class MatrixTransform;
   class Switch;
   class Node;
   class Geode;
   class LOD;
}
//Forward Declare the necessary osgSim classes that will be used by the NodeCollector class
namespace osgSim
{
   class DOFTransform;
   class MultiSwitch;
}
/// @endcond


namespace dtUtil
{
   /** NodeCollector is used to gather osg Group
    * nodes, DOFTransform nodes, MatrixTransform nodes, Switch Nodes
    * and Geode nodes (which have Drawable objects and Material objects).  It 
    * stores the different nodes into corresponding maps which may than
    * be retrieved by the user.  In the case of Geode nodes it creates maps
    * for Drawable objects and Material Objects which can be retrieved.
    *
    * For example, to find the osg::Switch named "switch1":
    * @code
    * dtUtil::NodeCollector *collector = new dtUtil::NodeCollector(modelNode, dtUtil::NodeCollector::SwitchFlag);
    * osg::Switch* sw = collector->GetSwitch("switch1");
    * @endcode
    */
   class DT_UTIL_EXPORT NodeCollector : public osg::Referenced
   {
   public:

      //Type Definitions for the four different node maps
      typedef std::map<std::string, dtCore::ObserverPtr <osg::Group> >             GroupNodeMap;
      typedef std::map<std::string, dtCore::ObserverPtr <osgSim::DOFTransform> >   TransformNodeMap;
      typedef std::map<std::string, dtCore::ObserverPtr <osg::MatrixTransform> >   MatrixTransformNodeMap;
      typedef std::map<std::string, dtCore::ObserverPtr <osg::Switch> >            SwitchNodeMap;
      typedef std::map<std::string, dtCore::ObserverPtr <osgSim::MultiSwitch> >    MultiSwitchNodeMap;
      typedef std::map<std::string, dtCore::ObserverPtr <osg::LOD> >               LODNodeMap;

      //Type Definitions for the two different geode node maps
      typedef std::map<std::string, dtCore::ObserverPtr<osg::Geode> >     GeodeNodeMap;

      ///Type Definition that is used to declare flags that allow the user to request searches for different types of nodes or geode nodes.
      typedef unsigned NodeFlag;

      ///NodeFlags that represent the four different types of nodes that can be searched for
      static const NodeFlag GroupFlag;
      static const NodeFlag DOFTransformFlag;
      static const NodeFlag MatrixTransformFlag;
      static const NodeFlag SwitchFlag;
      static const NodeFlag MultiSwitchFlag;
      static const NodeFlag GeodeFlag;
      static const NodeFlag LODFlag;

      ///NodeFlag that when defined will represent all kinds of nodes and Geode nodes
      static const NodeFlag AllNodeTypes;

      /**
       * Blank Constructor that is defined to do nothing.
       * @note If this is used then you must call use the CollectNodes function in order to generate any maps with this class.
       */
      NodeCollector();

      /**
       * Constructor that when called will automatically generate the node maps or geode maps that you request
       * @param nodeToLoad The starting node who's children you wish to traverse.
       * @param mask The different types of nodes you want to collect off of the loaded node.
       * @param nodeNameIgnored The name of a node that you do not want to collect.
       */
      NodeCollector(osg::Node* nodeToLoad, NodeCollector::NodeFlag mask, const std::string& nodeNameIgnored = "");

      /**
       * Function that when called will automatically generate the node maps or geode maps that you request
       * @param nodeToLoad The starting node who's children you wish to traverse.
       * @param mask The different types of nodes you want to collect off of the loaded node.
       * @param nodeNameIgnored The name of a node that you do not want to collect.
       * @note This function was originally intended to be used in conjunction with the blank constructor or after a call of the ClearAllNodes function
       */
      void CollectNodes(osg::Node* NodeToLoad, NodeCollector::NodeFlag mask, const std::string& nodeNamesIgnored = "");

      /**
       * Function that is defined to clear all the maps of their contents.
       */
      void ClearAll();

      /////////////////////////////////////////////////////////////////////////////////
      //The Following Five Methods are all used to return constant pointers to requested nodes//
      /////////////////////////////////////////////////////////////////////////////////

      /**
       * Function that is used to request a CONST pointer to a Group Node
       * @param name A String that represents the name of the node you are looking for
       * @return A CONST pointer to the node you were looking for or NULL if the node was not found
       */
      const osg::Group* GetGroup(const std::string& name) const;

      /**
       * Function that is used to request a CONST pointer to a DOFTransform Node
       * @param name A String that represents the name of the node you are looking for
       * @return A CONST pointer to the node you were looking for or NULL if the node was not found
       */
      const osgSim::DOFTransform* GetDOFTransform(const std::string& name) const;

      /**
       * Function that is used to request a CONST pointer to a MatrixTransform Node
       * @param name A String that represents the name of the node you are looking for
       * @return A CONST pointer to the node you were looking for or NULL if the node was not found
       */
      const osg::MatrixTransform* GetMatrixTransform(const std::string& name) const;

      /**
       * Function that is used to request a CONST pointer to a Switch Node
       * @param name A String that represents the name of the node you are looking for
       * @return A CONST pointer to the node you were looking for or NULL if the node was not found
       */
      const osg::Switch* GetSwitch(const std::string& name) const;

      /**
      * Function that is used to request a CONST pointer to a MultiSwitch Node
      * @param name A String that represents the name of the node you are looking for
      * @return A CONST pointer to the node you were looking for or NULL if the node was not found
      */
      const osgSim::MultiSwitch* GetMultiSwitch(const std::string& name) const;

      /**
       * Function that is used to request a CONST pointer to a Geode Node
       * @param name A String that represents the name of the node you are looking for
       * @return A CONST pointer to the node you were looking for or NULL if the node was not found
       */
      const osg::Geode* GetGeode(const std::string& name) const;

      /**
       * Function that is used to request a CONST pointer to an LOD Node
       * @param name A String that represents the name of the node you are looking for
       * @return A CONST pointer to the node you were looking for or NULL if the node was not found
       */
      const osg::LOD* GetLOD(const std::string& name) const;

      /////////////////////////////////////////////////////////////////////////////////
      //The Following Five Methods are all used to return pointers to requested nodes//
      /////////////////////////////////////////////////////////////////////////////////

      /**
       * Function that is used to request a pointer to a Group Node
       * @param name A String that represents the name of the node you are looking for
       * @return A pointer to the node you were looking for or NULL if the node was not found
       */
      osg::Group* GetGroup(const std::string& name);

      /**
       * Function that is used to request a pointer to a DOFTransform Node
       * @param name A String that represents the name of the node you are looking for
       * @return A pointer to the node you were looking for or NULL if the node was not found
       */
      osgSim::DOFTransform* GetDOFTransform(const std::string& name);

      /**
       * Function that is used to request a pointer to a MatrixTransform Node
       * @param name A String that represents the name of the node you are looking for
       * @return A pointer to the node you were looking for or NULL if the node was not found
       */
      osg::MatrixTransform* GetMatrixTransform(const std::string& name);

      /**
       * Function that is used to request a pointer to a Switch Node
       * @param name A String that represents the name of the node you are looking for
       * @return A pointer to the node you were looking for or NULL if the node was not found
       */
      osg::Switch* GetSwitch(const std::string& name);

      /**
      * Function that is used to request a pointer to a MultiSwitch Node
      * @param name A String that represents the name of the node you are looking for
      * @return A pointer to the node you were looking for or NULL if the node was not found
      */
      osgSim::MultiSwitch* GetMultiSwitch(const std::string& name);

      /**
       * Function that is used to request a pointer to a Geode Node
       * @param name A String that represents the name of the node you are looking for
       * @return A pointer to the node you were looking for or NULL if the node was not found
       */
      osg::Geode* GetGeode(const std::string& name);

      /**
       * Function that is used to request a pointer to an LOD Node
       * @param name A String that represents the name of the node you are looking for
       * @return A pointer to the node you were looking for or NULL if the node was not found
       */
      osg::LOD* GetLOD(const std::string& name);


      /////////////////////////////////////////////////////////////////////////////////
      //The Following Five Methods are all used to add nodes to their respective maps//
      /////////////////////////////////////////////////////////////////////////////////

      /**
       * Function that is used to add a Group Node to the Group Node map
       * @param name A String that represents the name of the Node
       * @param node The Group Node that you wish to add to the map
       */
      void AddGroup(const std::string& name, osg::Group& node);

      /**
       * Function that is used to remove a Group Node to the Group Node map
       * @param name A String that represents the name of the Node
       */
      void RemoveGroup(const std::string& name);

      /**
       * Function that is used to add a DOFTransform Node to the DOFTransform Node map
       * @param name A String that represents the name of the Node
       * @param node The DOFTransform Node that you wish to add to the map
       */
      void AddDOFTransform(const std::string& name, osgSim::DOFTransform& node);

      /**
       * Function that is used to remove a DOF Transform Node to the DOF Transform Node map
       * @param name A String that represents the name of the Node
       */
      void RemoveDOFTransform(const std::string& name);

      /**
       * Function that is used to add a MatrixTransform Node to the MatrixTransform Node map
       * @param name A String that represents the name of the Node
       * @param node The MatrixTransform Node that you wish to add to the map
       */
      void AddMatrixTransform(const std::string& name, osg::MatrixTransform& node);

      /**
       * Function that is used to remove a Matrix Transform Node to the Matrix Transform Node map
       * @param name A String that represents the name of the Node
       */
      void RemoveMatrixTransform(const std::string& name);

      /**
       * Function that is used to add a Switch Node to the Switch Node map
       * @param name A String that represents the name of the Node
       * @param node The Switch Node that you wish to add to the map
       */
      void AddSwitch(const std::string& name, osg::Switch& node);

      /**
       * Function that is used to remove a Switch Node to the Switch Node map
       * @param name A String that represents the name of the Node
       */
      void RemoveSwitch(const std::string& name);

      /**
      * Function that is used to add a MultiSwitch Node to the MultiSwitch Node map
      * @param name A String that represents the name of the Node
      * @param node The MultiSwitch Node that you wish to add to the map
      */
      void AddMultiSwitch(const std::string& name, osgSim::MultiSwitch& node);

      /**
      * Function that is used to remove a MultiSwitch Node to the MultiSwitch Node map
      * @param name A String that represents the name of the Node
      */
      void RemoveMultiSwitch(const std::string& name);

      /**
       * Function that is used to add a Geode Node to the Geode Node map
       * @param name A String that represents the name of the Node
       * @param node The Geode Node that you wish to add to the map
       */
      void AddGeode(const std::string& name, osg::Geode& node);

      /**
       * Function that is used to remove a Geode Node to the Geode Node map
       * @param name A String that represents the name of the Node
       */
      void RemoveGeode(const std::string& name);

      /**
       * Function that is used to add an LOD Node to the LOD Node map
       * @param name A String that represents the name of the Node
       * @param node The LOD Node that you wish to add to the map
       */
      void AddLOD(const std::string& name, osg::LOD& node);

      /**
       * Function that is used to remove an LOD Node to the LOD Node map
       * @param name A String that represents the name of the Node
       */
      void RemoveLOD(const std::string& name);


      ////////////////////////////////////////////////////////////////////////////////
      //The Following Methods are all used to return CONST maps populated with nodes//
      ////////////////////////////////////////////////////////////////////////////////

      /**
       * Function that returns a CONST Group map
       * @return A map with the names of Group Nodes and osg Group Nodes
       */
      const NodeCollector::GroupNodeMap& GetGroupNodeMap() const;

      /**
       * Function that returns a CONST Transform map
       * @return A map with the names of Transform Nodes and osg Transform Nodes
       */
      const NodeCollector::TransformNodeMap& GetTransformNodeMap() const;

      /**
       * Function that returns CONST a MatrixTransform map
       * @return A map with the names of MatrixTransform Nodes and osg MatrixTransform Nodes
       */
      const NodeCollector::MatrixTransformNodeMap& GetMatrixTransformNodeMap() const;

      /**
       * Function that returns a CONST Switch map
       * @return A map with the names of Switch Nodes and osg Switch Nodes
       */
      const NodeCollector::SwitchNodeMap& GetSwitchNodeMap() const;

      /**
      * Function that returns a CONST MultiSwitch map
      * @return A map with the names of MultiSwitch Nodes and osg MultiSwitch Nodes
      */
      const NodeCollector::MultiSwitchNodeMap& GetMultiSwitchNodeMap() const;

      /**
       * Function that returns CONST a Geode map
       * @return A map with the names of Geode Nodes and osg Geode Nodes
       */
      const NodeCollector::GeodeNodeMap& GetGeodeNodeMap() const;

      /**
       * Function that returns CONST an LOD map
       * @return A map with the names of Geode LOD and osg LOD Nodes
       */
      const NodeCollector::LODNodeMap& GetLODNodeMap() const;

      //////////////////////////////////////////////////////////////////////////
      //The Following Methods are all used to return maps populated with nodes//
      //////////////////////////////////////////////////////////////////////////

      /**
       * Function that returns a Group map
       * @return A map with the names of Group Nodes and osg Group Nodes
       */
      NodeCollector::GroupNodeMap& GetGroupNodeMap();

      /**
       * Function that returns a Transform map
       * @return A map with the names of Transform Nodes and osg Transform Nodes
       */
      NodeCollector::TransformNodeMap& GetTransformNodeMap();

      /**
       * Function that returns a MatrixTransform map
       * @return A map with the names of MatrixTransform Nodes and osg MatrixTransform Nodes
       */
      NodeCollector::MatrixTransformNodeMap& GetMatrixTransformNodeMap();

      /**
       * Function that returns a Switch map
       * @return A map with the names of Switch Nodes and osg Switch Nodes
       */
      NodeCollector::SwitchNodeMap& GetSwitchNodeMap();

      /**
      * Function that returns a MultiSwitch map
      * @return A map with the names of MultiSwitch Nodes and osg MultiSwitch Nodes
      */
      NodeCollector::MultiSwitchNodeMap& GetMultiSwitchNodeMap();

      /**
       * Function that returns a Geode map
       * @return A map with the names of Geode Nodes and osg Geode Nodes
       */
      NodeCollector::GeodeNodeMap& GetGeodeNodeMap();

      /**
      * Function that returns an LOD map
      * @return A map with the names of LOD Nodes and osg LOD Nodes
      */
      NodeCollector::LODNodeMap& GetLODNodeMap();

   protected:

      /**
       * Destructor
       */
      virtual ~NodeCollector();

   private:
      /**
       * The following five member variables each represent maps for the different nodes that can be searched for
       */
      NodeCollector::GroupNodeMap           mGroupNodeMap;
      NodeCollector::TransformNodeMap       mTranformNodeMap;
      NodeCollector::MatrixTransformNodeMap mMatrixTransformNodeMap;
      NodeCollector::SwitchNodeMap          mSwitchNodeMap;
      NodeCollector::MultiSwitchNodeMap     mMultiSwitchNodeMap;
      NodeCollector::GeodeNodeMap           mGeodeNodeMap;
      NodeCollector::LODNodeMap             mLODNodeMap;

   };
} // namespace

#endif //DELTA_NODE_COLLECTOR
