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
       * Updates all nodes in the graph.
       *
       * @param[in]  simDelta  Elapsed sim time.
       * @param[in]  delta     Elapsed real time.
       */
      void Update(float simDelta, float delta);

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
      osg::Vec2   mPosition;

      std::vector<dtCore::RefPtr<DirectorGraph> > mSubGraphs;

      std::vector<dtCore::RefPtr<EventNode> >  mEventNodes;
      std::vector<dtCore::RefPtr<ActionNode> > mActionNodes;
      std::vector<dtCore::RefPtr<ValueNode> >  mValueNodes;
   };
}

#endif // DIRECTOR_GRAPH