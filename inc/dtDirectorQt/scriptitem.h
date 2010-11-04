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
       * @param[in]  node    The Director Node.
       * @param[in]  parent  The parent item.
       * @param[in]  scene   The scene.
       */
      ScriptItem(Node* node, QGraphicsItem* parent = 0, EditorScene* scene = 0);

      /**
       * Draws the node.
       */
      virtual void Draw();

   public slots:

   protected:
   };
}

#endif
