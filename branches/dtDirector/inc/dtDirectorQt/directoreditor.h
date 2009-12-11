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

#ifndef DIRECTORQT_EDITOR
#define DIRECTORQT_EDITOR

#include <dtDirectorQt/export.h>

#include <dtDirector/director.h>

#include <QtGui/QMainWindow>


class QAction;
class QMenuBar;
class QToolBar;

namespace dtDirector
{
   class GraphTabs;
   class PropertyEditor;
   class UndoManager;

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
      void OpenGraph(dtDirector::DirectorGraph* graph, bool newTab = false);

      /**
       * Accessor for the Director.
       */
      Director* GetDirector() {return mDirector;}

      /**
       * Accessor for the Property Editor.
       */
      PropertyEditor* GetPropertyEditor() {return mPropertyEditor;}

      /**
       * Accessor for the undo manager.
       */
      UndoManager* GetUndoManager() {return mUndoManager;}

      /**
       * Refreshes the entire view.
       */
      void Refresh();

      /**
       * Refreshes the state of the UI buttons.
       */
      void RefreshButtonStates();

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
       * Event handler when the undo button is pressed.
       */
      void OnUndo();

      /**
       * Event handler when the redo button is pressed.
       */
      void OnRedo();

      /**
       * Event handler when the show property editor button is pressed.
       */
      void OnShowPropertyEditor();

   protected:

      /**
       * Event handler when a key is pressed.
       *
       * @param[in]  e  The key event.
       */
      void keyPressEvent(QKeyEvent *e);


      GraphTabs*           mGraphTabs;
      PropertyEditor*      mPropertyEditor;
      UndoManager*         mUndoManager;

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
      QAction*  mUndoAction;
      QAction*  mRedoAction;

      // View Actions.
      QAction*  mViewPropertiesAction;
   };

} // namespace dtDirector

#endif // DIRECTORQT_EDITOR
