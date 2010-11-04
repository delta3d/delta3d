/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTOR_GRAPH
#define DIRECTOR_GRAPH

#include <dtDirector/export.h>

#include <dtDirector/eventnode.h>
#include <dtDirector/actionnode.h>
#include <dtDirector/valuenode.h>

#include <dtCore/uniqueid.h>


namespace dtDirector
{
   class Director;

   /**
    * Director Graph structure that contains all nodes
    * within the director script.
    */
   class DT_DIRECTOR_EXPORT DirectorGraph: public dtDAL::PropertyContainer
   {
   public:

      /**
       * Constructor.
       */
      DirectorGraph(Director* director);

      /**
       * This method is called in init, which instructs the director
       * to create its properties.  Methods implementing this should
       * be sure to call their parent class's buildPropertyMap method to
       * ensure all properties in the proxy inheritance hierarchy are
       * correctly added to the property map.
       *
       * @see GetDeprecatedProperty to handle old properties that need
       *       to be removed.
       */
      virtual void BuildPropertyMap(bool isParent = false);

      /**
       * Accessors for the Graph ID.
       */
      void SetID(const dtCore::UniqueId& id) {mID = id;}
      const dtCore::UniqueId& GetID() {return mID;}

      /**
       * Retrieves a graph of the given ID.
       *
       * @param[in]  id  The ID of the graph.
       *
       * @return     A pointer to the graph found, or NULL
       *             if not found.
       */
      DirectorGraph* GetGraph(const dtCore::UniqueId& id);
      
      /**
       * Retrieves a node of the given ID.
       *
       * @param[in]  id  The ID of the node.
       *
       * @return     A pointer to the node found, or NULL
       *             if not found.
       */
      Node* GetNode(const dtCore::UniqueId& id);

      /**
       * Retrieves a list of nodes that are of a certain type.
       *
       * @param[in]   name             The type name of the node.
       * @param[in]   category         The type category of the node.
       * @param[out]  outNodes         A list of nodes found.
       * @param[in]   searchSubGraphs  True to recursively check sub graphs for nodes.
       */
      void GetNodes(const std::string& name, const std::string& category, std::vector<Node*>& outNodes, bool searchSubGraphs = true);

      /**
       * Retrieves a list of nodes that are of a certain type,
       * and contain a property with a given value.
       *
       * @param[in]   name             The type name of the node.
       * @param[in]   category         The type category of the node.
       * @param[in]   property         The name of the property to find.
       * @param[in]   value            The value of the property.
       * @param[out]  outNodes         A list of nodes found.
       * @param[in]   searchSubGraphs  True to recursively check sub graphs for nodes.
       */
      void GetNodes(const std::string& name, const std::string& category, const std::string& property, const std::string& value, std::vector<Node*>& outNodes, bool searchSubGraphs = true);

      /**
       * Retrieves a list of all nodes in the graph.
       *
       * @param[out]  outNodes  A list of nodes found.
       */
      void GetAllNodes(std::vector<Node*>& outNodes);

      /**
       * Retrieves a value node with the given name.
       * @note  If there are more than one value with the
       *         same name, only the first one will be returned.
       *
       * @param[in]  name             The name of the value to find.
       * @param[in]  searchSubgraphs  True if you wish to search for the value node in any subgraphs.
       *
       * @return     A pointer to the value node, NULL if not found.
       */
      ValueNode* GetValueNode(const std::string& name, bool searchSubgraphs = true);

      /**
       * Adds a node to the graph.
       *
       * @param[in]  node  The node.
       */
      bool AddNode(Node* node);

      /**
       * Deletes a graph.
       *
       * @param[in]  graph  The graph to remove.
       *
       * @return     True if the graph was found and removed.
       */
      bool DeleteGraph(const dtCore::UniqueId& id);

      /**
       * Deletes a node.
       *
       * @param[in]  node  The node to remove.
       *
       * @return     True if the node was found and removed.
       */
      bool DeleteNode(const dtCore::UniqueId& id);

      /**
       * Retrieves the director.
       *
       * @return  The director.
       */
      Director* GetDirector() {return mDirector;}

      /**
       * Accessors for the enabled flag.
       */
      void SetEnabled(bool enabled) {mEnabled = enabled;}
      bool GetEnabled() {return mEnabled;}

      /**
       * Retrieves the UI color of the node.
       *
       * @return  The Color.
       */
      osg::Vec4 GetColor() const {return mColor;}

      /**
       * Sets the UI color of the node.
       *
       * @param[in]  color  The color.
       */
      void SetColor(const osg::Vec4& color) { mColor = color; }
      void SetColorRGB(const osg::Vec3& color) { SetColor(osg::Vec4(color, 225)/255.0f); }
      void SetColorRGBA(int r, int g, int b, int a = 225) { SetColor(osg::Vec4(r, g, b, a)/255.0f); }


      /**
       * Accessors for the graph name.
       */
      void SetName(const std::string& name) {mName = name;}
      const std::string& GetName() {return mName;}

      /**
       * Accessors for the comment of the script.
       */
      void SetComment(const std::string& comment) {mComment = comment;}
      std::string& GetComment() {return mComment;}

      /**
       * Accessors for the custom editor used for this graph.
       */
      void SetEditor(const std::string& editor) {mEditor = editor;}
      std::string& GetEditor() {return mEditor;}

      /**
       * Accessors for the graph position.
       */
      void SetPosition(const osg::Vec2& pos) {mPosition = pos;}
      const osg::Vec2& GetPosition() {return mPosition;}

      /**
       * Adds a sub-graph to this graph.
       *
       * @param[in]  graph  The graph to add, use NULL
       *                    to create a new one.
       *
       * @return     A pointer to the graph that was added.
       */
      DirectorGraph* AddGraph(DirectorGraph* graph = NULL);

      /**
       * Removes a sub-graph from this graph.
       *
       * @param[in]  graph  The graph to remove.
       */
      void RemoveGraph(DirectorGraph* graph);

      /**
       * Accessors for the node lists.
       */
      std::vector<dtCore::RefPtr<EventNode> >& GetEventNodes() {return mEventNodes;}
      std::vector<dtCore::RefPtr<ActionNode> >& GetActionNodes() {return mActionNodes;}
      std::vector<dtCore::RefPtr<ValueNode> >& GetValueNodes() {return mValueNodes;}

      /**
       * Retrieves a list of all external input, output, or value
       * nodes that should be visible from the parent.
       */
      std::vector<dtCore::RefPtr<EventNode> >  GetInputNodes();
      std::vector<dtCore::RefPtr<ActionNode> > GetOutputNodes();
      std::vector<dtCore::RefPtr<ValueNode> >  GetExternalValueNodes();

      /**
       * Accessor for sub graphs.
       */
      std::vector<dtCore::RefPtr<DirectorGraph> >& GetSubGraphs() {return mSubGraphs;}

      dtCore::UniqueId mID;

      Director*      mDirector;
      DirectorGraph* mParent;

      bool        mEnabled;

      std::string mName;
      std::string mComment;
      std::string mEditor;
      osg::Vec2   mPosition;

      std::vector<dtCore::RefPtr<DirectorGraph> > mSubGraphs;

      std::vector<dtCore::RefPtr<EventNode> >  mEventNodes;
      std::vector<dtCore::RefPtr<ActionNode> > mActionNodes;
      std::vector<dtCore::RefPtr<ValueNode> >  mValueNodes;

      protected:
         osg::Vec4 mColor;

   };
}

#endif // DIRECTOR_GRAPH
