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

#ifndef NODE_TABS
#define NODE_TABS

#include <dtDirectorQt/export.h>
#include <dtDirector/nodetype.h>

#include <QtGui/QToolBox>

namespace dtDirector
{
   class DirectorEditor;
   class DirectorGraph;

   /**
   * @class EditorScene
   */
   class DT_DIRECTOR_QT_EXPORT NodeTabs : public QToolBox
   {
      Q_OBJECT

   public:

      /**
      * Constructor.
      *
      * @param[in] parent The parent editor.
      */
      NodeTabs(QWidget* parent);

      /**
       * Sets the editor.
       */
      void SetEditor(DirectorEditor* editor);

      /**
       * Refresh the scene to display newly loaded nodes
       *
       * @param nodeType The type of nodes to display in the scene
       */
      void RefreshNodes(NodeType::NodeTypeEnum nodeType);

   signals:
      void CreateNode(const QString& name, const QString& category);

   public slots:
      void OnCreateNodeEvent(const QString& name, const QString& category);

   private:

      DirectorEditor* mpEditor;
      DirectorGraph* mpGraph;
   };
} // namespace dtDirector

#endif // NODE_TABS
