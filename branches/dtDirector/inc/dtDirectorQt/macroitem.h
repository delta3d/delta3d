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

#ifndef DIRECTORQT_MACRO_ITEM
#define DIRECTORQT_MACRO_ITEM

#include <dtDirectorQt/nodeitem.h>

namespace dtDirector
{
   class DirectorGraphData;

   /**
    * Draws a macro in the graph view.
    *
    * A macro is a subgraph of nodes represented by
    * a single node in the parent graph.
    */
   class MacroItem : public NodeItem
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  graph   The Director Graph.
       * @param[in]  parent  The parent item.
       * @param[in]  scene   The scene.
       */
      MacroItem(DirectorGraphData* graph, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Draws the node.
       */
      virtual void Draw();

      /**
       * Finds all of the links.
       */
      virtual void FindLinks();

      /**
       * Checks if this item contains a node of the given ID.
       *
       * @param[in]  id  The ID to check.
       */
      virtual bool HasID(const dtCore::UniqueId& id);

      /**
       * Retrieves the graph.
       */
      DirectorGraphData* GetGraph() {return mGraph.get();}

      /**
       * Event handler when this item changes.
       */
      QVariant itemChange(GraphicsItemChange change, const QVariant &value);

   protected:

      /**
       * Event handler when the user double clicks this node.
       *
       * @param[in]  event  The mouse event.
       */
      void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

      dtCore::RefPtr<DirectorGraphData> mGraph;
   };
}

#endif
