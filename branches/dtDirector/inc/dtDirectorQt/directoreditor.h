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
#include <dtDirectorQt/propertyeditor.h>

#include <dtDirector/director.h>

#include <dtCore/refptr.h>

#include <QtGui/QMainWindow>
#include <QtGui/QTabWidget>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>

class QAction;
class QMenuBar;
class QToolBar;
class QGraphicsSceneMouseEvent;

namespace dtDirector
{
   class EditorView;
   class DirectorEditor;
   class NodeItem;
   class MacroItem;

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
      * @param[in]  director    The Director.
      * @param[in]  propEditor  The Property Editor.
      * @param[in]  graphTabs   The Graph Tabs widget.
      * @param[in]  view        The scene viewer.
      * @param[in]  parent      The parent widget.
      */
      EditorScene(Director* director, PropertyEditor* propEditor, GraphTabs* graphTabs, QWidget* parent = 0);

      /**
       * Sets the current view.
       */
      void SetView(EditorView* view) {mView = view;}

      /**
       * Sets the currently viewed director graph.
       *
       * @param[in]  graph  The Graph to view.
       */
      void SetGraph(dtDirector::DirectorGraphData* graph);

      /**
       * Retrieves the current graph.
       *
       * @return  The current graph.
       */
      dtDirector::DirectorGraphData* GetGraph() {return mGraph;}

      /**
       * Retrieves the background item.
       */
      QGraphicsRectItem* GetTranslationItem() {return mTranslationItem;};

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

      /**
       * Retrieves a graph item for a graph.
       *
       * @param[in]  graph  The graph.
       *
       * @return     The graph item that contains this graph.
       */
      MacroItem* GetGraphItem(DirectorGraphData* graph);

      /**
       * Adds an item to the selected list.
       *
       * @param[in]  container  The item to add.
       */
      void AddSelected(dtDAL::PropertyContainer* container);

      /**
       * Removes an item from the selected list.
       *
       * @param[in]  container  The item to remove.
       */
      void RemoveSelected(dtDAL::PropertyContainer* container);

      /**
       * Refreshes the property editor.
       */
      void RefreshProperties();

   signals:

   public slots:

   protected:
      
      /**
       * Mouse button events.
       *
       * @param[in]  event  The mouse event.
       */
      void mousePressEvent(QGraphicsSceneMouseEvent* event);
      void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

      /**
       * Mouse movement event.
       *
       * @param[in]  event  The mouse event.
       */
      void mouseMoveEvent(QGraphicsSceneMouseEvent* event);  

   private:

      EditorView*              mView;

      dtCore::RefPtr<Director> mDirector;
      PropertyEditor*          mPropertyEditor;
      GraphTabs*               mGraphTabs;

      dtDirector::DirectorGraphData*   mGraph;

      std::vector<NodeItem*>           mNodes;

      bool     mDragging;
      QPointF  mDragOrigin;
      bool     mHasDragged;

      QGraphicsRectItem* mTranslationItem;
      
      PropertyEditor::PropertyContainerRefPtrVector mSelected;
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
      EditorView(EditorScene* scene, QWidget* parent = 0);

      /**
       * Retrieves the graphic scene.
       */
      EditorScene* GetScene() {return mScene;}

   protected:

      /**
       * Event handler when the mouse wheel is scrolled.
       *
       * @param[in]  event  The wheel event.
       */
      void wheelEvent(QWheelEvent* event);

   private:

      EditorScene* mScene;

      float mMinScale;
      float mMaxScale;
      float mCurrentScale;
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
       * @param[in]  parent    The parent widget.
       */
      DirectorEditor(QWidget* parent = 0);

      /**
       * Destructor
       */
      virtual ~DirectorEditor();

      /**
       * Sets the Director.
       */
      void SetDirector(Director* director);

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

      /**
       * Accessor for the Property Editor.
       */
      PropertyEditor* GetPropertyEditor() {return mPropertyEditor;}

   public slots:

      /**
       * Event handler when the visibility of the property editor is changed.
       *
       * @param[in]  visible  True if the editor is visible.
       */
      void OnPropertyEditorVisibilityChange(bool visible);

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

      /**
       * Event handler when the save button is pressed.
       */
      void OnSaveButton();

      /**
       * Event handler when the load button is pressed.
       */
      void OnLoadButton();

      /**
       * Event handler when the new button is pressed.
       */
      void OnNewButton();

      /**
       * Event handler when the parent button is pressed.
       */
      void OnParentButton();

      /**
       * Event handler when the show property editor button is pressed.
       */
      void OnShowPropertyEditor();

   protected:

      GraphTabs*           mGraphTabs;
      PropertyEditor*      mPropertyEditor;

      dtCore::RefPtr<Director> mDirector;

      QMenuBar* mMenuBar;
      QToolBar* mToolbar;

      QMenu*    mFileMenu;
      QMenu*    mEditMenu;
      QMenu*    mViewMenu;

      // File Actions.
      QAction*  mSaveAction;
      QAction*  mLoadAction;
      QAction*  mNewAction;

      // Edit Actions.
      QAction*  mParentAction;

      // View Actions.
      QAction*  mViewPropertiesAction;
   };

} // namespace dtDirector

#endif // DIRECTOR_EDITOR
