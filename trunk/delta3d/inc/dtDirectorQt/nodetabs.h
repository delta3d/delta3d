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
#include <dtDirector/director.h>

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
       * Returns whether this tab is empty of node types.
       */
      bool IsEmpty() const;

      /**
       * Sets the editor.
       */
      void SetEditor(DirectorEditor* editor, dtDirector::Director* nodeSceneDirector);

      /**
       * Refresh the scene to display newly loaded nodes
       *
       * @param nodeType    The type of nodes to display in the scene
       */
      void RefreshNodes(NodeType::NodeTypeEnum nodeType);

      /**
       * Refresh the scene with nodes found with a search string.
       *
       * @param searchText  The nodes to search for.
       */
      void SearchNodes(const QString& searchText, DirectorGraph* graph);

      /**
       * Refresh the scene with all reference values from a given graph.
       *
       * @param[in]  graph  The graph to find references for.
       */
      void SearchReferenceNodes(DirectorGraph* graph);

      /**
       * Adds a custom editor macro to the node selection.
       *
       * @param[in]  editor  The name of the custom editor.
       */
      void AddCustomEditor(const std::string& editor);

   signals:
      void CreateNode(const QString& name, const QString& category, const QString& refName);

   private:

      bool mIsEmpty;

      DirectorEditor* mpEditor;
      dtCore::RefPtr<dtDirector::Director> mpDirector;
      DirectorGraph* mpGraph;
   };
} // namespace dtDirector

#endif // NODE_TABS
