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

#ifndef DIRECTOR_EDITOR
#define DIRECTOR_EDITOR

#include <vector>

#include <dtDirectorQt/export.h>

#include <dtDirector/director.h>

#include <dtCore/refptr.h>

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QScrollBar>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>

class QAction;

namespace dtDirector
{
   class DirectorEditor;
   class NodeItem;

   /**
   * @class GraphTabs
   * @brief This allows multiple tabbed editing of director graphs.
   */
   class GraphTabs : public QTabWidget
   {
      Q_OBJECT

   public:

      /**
      * Constructs a document tabs viewer.
      *
      * @param[in]  parent  The parent widget.
      * @param[in]  editor  The director editor that owns this widget.
      */
      GraphTabs(QWidget* parent, DirectorEditor* editor);

   protected:

      /**
      * Event handler when the mouse button is clicked.
      *
      * @param[in]  e  The mouse event.
      */
      void mousePressEvent(QMouseEvent* e);

      /**
      * Event handler when the mouse button is released.
      *
      * @param[in]  e  The mouse event.
      */
      void mouseReleaseEvent(QMouseEvent *e);

      /**
      * Event handler when the mouse button is double clicked.
      *
      * @param[in]  e  The mouse event.
      */
      void mouseDoubleClickEvent(QMouseEvent *e);


      DirectorEditor* mEditor;
   };


   /**
   * @class EditorScene
   */
   class EditorScene : public QGraphicsScene
   {
      Q_OBJECT

   public:

      /**
      * Constructor.
      *
      * @param[in]  parent  The parent widget.
      */
      EditorScene(QWidget* parent = 0);

      /**
       * Sets the currently viewed director graph.
       *
       * @param[in]  graph  The Graph to view.
       */
      void SetGraph(dtDirector::DirectorGraphData* graph);

      /**
       * Refreshes the graph.
       */
      void Refresh();

      /**
       * Retrieves a node item given the nodes ID.
       *
       * @param[in]  id  The ID of the node.
       *
       * @return     A pointer to the NodeItem with the matching ID,
       *             or NULL if not found.
       */
      NodeItem* GetNodeItem(const dtCore::UniqueId& id);

   signals:

   protected:
      
   private:

      dtDirector::DirectorGraphData*   mGraph;

      std::vector<NodeItem*>           mNodes;
   };

   /**
    * @class EditorView
    */
   class EditorView: public QGraphicsView
   {
      Q_OBJECT

   public:

      /**
       * Constructor.
       *
       * @param[in]  scene   The scene.
       * @param[in]  parent  The parent.
       */
      EditorView(EditorScene* scene, QWidget* parent = 0)
         : QGraphicsView(scene, parent)
         , mScene(scene)
      {
         setObjectName("Graph Tab");
         setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
      }

      /**
       * Retrieves the graphic scene.
       */
      EditorScene* GetScene() {return mScene;}

   private:

      EditorScene* mScene;

   };

   /**
    * @class DirectorEditor
    *
    * @brief Editor for editing Director Graphs.
    */
   class DT_DIRECTOR_QT_EXPORT DirectorEditor : public QMainWindow
   {
      Q_OBJECT

   public:
      /**
       * Constructs a new help box.
       *
       * @param[in]  director  The director being edited.
       * @param[in]  parent    The parent widget.
       */
      DirectorEditor(Director* director, QWidget* parent = 0);

      /**
       * Destructor
       */
      virtual ~DirectorEditor();

      /**
       * Opens a Director graph.
       *
       * @param[in]  graph   The graph to open.
       * @param[in]  newTab  True to create a new tab for the graph.
       */
      void OpenGraph(dtDirector::DirectorGraphData* graph, bool newTab = false);

      /**
       * Accessor for the Director.
       */
      Director* GetDirector() {return mDirector;}

   public slots:

      /**
       * Event handler when the parent button is pressed.
       */
      void OnParentButton();

      /**
      * Event handler when the current document tab has changed.
      *
      * @param[in]  index  The index of the new tab.
      */
      void OnGraphTabChanged(int index);

      /**
      * Event handler when the current document tab has closed.
      *
      * @param[in]  index  The index of the closed tab.
      */
      void OnGraphTabClosed(int index);

   private:

      GraphTabs*           mGraphTabs;

      dtCore::RefPtr<Director> mDirector;

      QAction*             mParentAction;
   };

} // namespace dtDirector

#endif // DIRECTOR_EDITOR
