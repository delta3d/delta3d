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

#ifndef DIRECTORQT_SCRIPT_ITEM
#define DIRECTORQT_SCRIPT_ITEM

#include <dtDirectorQt/actionitem.h>


namespace dtDirector
{
   /**
    * Draws a macro in the graph view.
    *
    * A macro is a subgraph of nodes represented by
    * a single node in the parent graph.
    */
   class ScriptItem : public ActionItem
   {
      Q_OBJECT
   public:

      /**
       * Constructor.
       *
       * @param[in]  node      The Director Node.
       * @param[in]  readOnly  True if this node is read only.
       * @param[in]  imported  True if this node is an imported node.
       * @param[in]  parent    The parent item.
       * @param[in]  scene     The scene.
       */
      ScriptItem(Node* node, bool readOnly, bool imported, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Draws the node.
       */
      virtual void Draw();

      /**
       * Draws a glow around the node.
       */
      virtual void DrawGlow();

      /**
       * Retrieves whether the node is contained.
       *
       * @param[in]  node  The node.
       *
       * @return true if the node is contained.
       */
      virtual bool HasNode(Node* node);

   protected:

      /**
       * Event handler when the context menu event has triggered.
       */
      void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

      /**
       * Re-implementations of functions that are ambiguous to both
       * inherited classes.
       */
      void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

   public slots:

      /**
       * Opens a new editor window to edit this script.
       */
      void EditScript();

   protected:
   };
}

#endif
