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
   class GraphBrowser;
   class UndoManager;
   class ReplayBrowser;
   class CustomEditorTool;

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
       * Accessor for the graph tab widget.
       */
      GraphTabs* GetGraphTabs() {return mGraphTabs;}

      /**
       * Accessor for the Property Editor.
       */
      PropertyEditor* GetPropertyEditor() {return mPropertyEditor;}

      /**
       * Accessor for the undo manager.
       */
      UndoManager* GetUndoManager() {return mUndoManager;}

      /**
       * Accessor for the replay browser.
       */
      ReplayBrowser* GetReplayBrowser() {return mReplayBrowser;}

      /**
       * Retrieves whether we are in replay mode.
       */
      bool GetReplayMode() {return mReplayMode;}

      /**
       * Retrieves the input that was triggered on the replay node.
       */
      const InputLink* GetReplayInput() {return mReplayInput;}

      /**
       * Retrieves the output that triggered the replay node.
       */
      const OutputLink* GetReplayOutput() {return mReplayOutput;}

      /**
       * Retrieves the current replay item.
       */
      const Director::RecordNodeData& GetReplayNode() {return mReplayNode;}

      /**
       * Sets the current replay node data.
       *
       * @param[in]  replayNode  The replay node.
       * @param[in]  input       The name of the input that is being fired.
       * @param[in]  output      The name of the output that is firing this node.
       */
      void SetReplayNode(Director::RecordNodeData* replayNode = NULL, InputLink* input = NULL, OutputLink* output = NULL);

      /**
       * Retrieves actions.
       */
      QAction* GetParentAction()   {return mParentAction;}
      QAction* GetUndoAction()     {return mUndoAction;}
      QAction* GetRedoAction()     {return mRedoAction;}
      QAction* GetCutAction()      {return mCutAction;}
      QAction* GetCopyAction()     {return mCopyAction;}
      QAction* GetPasteAction()    {return mPasteAction;}
      QAction* GetDeleteAction()   {return mDeleteAction;}
      QAction* GetSnapGridAction() {return mSnapGridAction;}
      QAction* GetShowLinkAction() {return mShowLinksAction;}
      QAction* GetHideLinkAction() {return mHideLinksAction;}
      QAction* GetRefreshAction()  {return mRefreshAction;}

      /**
       * Calculates a snapped position.
       *
       * @param[in]  position  The original position.
       *
       * Return      A snapped position, only if snap is enabled.
       */
      osg::Vec2 GetSnapPosition(osg::Vec2 position);

      /**
       * Refreshes the entire view.
       */
      void Refresh();

      /**
       * Refreshes all scenes that view a given graph.
       *
       * @param[in]  graph  The graph to refresh.
       */
      void RefreshGraph(DirectorGraph* graph);

      /**
       * Refreshes the state of the UI buttons.
       */
      void RefreshButtonStates();

      /**
       * Event handler when a director node is created.
       * @note: This should be called after an external editor
       *        has created a node and set all its properties
       *        so it can be registered in the undo manager.
       *
       * @param[in]  node  The created node.
       */
      void OnNodeCreated(Node* node);

      /**
       * Deletes a director node.
       *
       * @param[in]  id  The ID of the node to delete.
       */
      void DeleteNode(dtCore::UniqueId id);

      /**
       * Event handler when viewing the context menu of a value node.
       *
       * @param[in]  node  The value node.
       * @param[in]  menu  The menu to add new context items to.
       *
       * @return     True if a default action was set.
       */
      virtual bool OnContextValueNode(dtCore::RefPtr<dtDirector::Node> node, QMenu& menu)
      {return false;}

      /**
       * Event handler when a value node is double clicked.
       *
       * @param[in]  node  The value node that was double clicked.
       *
       * @return     True if an action was performed.
       */
      virtual bool OnDoubleClickValueNode(dtCore::RefPtr<dtDirector::Node> node)
      {return false;}

      /**
       * Retrieves the current actor selection from STAGE.
       *
       * @return  The current actor selection.
       */
      virtual std::vector<dtDAL::BaseActorObject*> GetActorSelection()
      {return std::vector<dtDAL::BaseActorObject*>();}

      /**
       * Registers a custom editor tool.
       *
       * @param[in]  tool  The tool to register.
       *
       * @return     True if the tool was registered successfully.
       */
      bool RegisterCustomEditorTool(CustomEditorTool* tool);

      /**
       * UnRegisters a custom editor tool.
       *
       * @param[in]  tool      The tool to un-register.
       * @param[in]  toolName  The name of the tool ot un-register.
       *
       * @return     True if the tool was found and un-registered successfully.
       */
      bool UnRegisterCustomEditorTool(CustomEditorTool* tool);
      bool UnRegisterCustomEditorTool(const std::string& toolName);

      /**
       * Retrieves a list of tools that have been registered.
       *
       * @return  A list of registered tool names.
       */
      std::vector<std::string> GetRegisteredToolList();

      /**
       * Retrieves the tool of a given name.
       *
       * @param[in]  name  The name of the tool.
       *
       * @return     The tool, if found.
       */
      CustomEditorTool* GetRegisteredTool(const std::string& name);

   public slots:

      /**
       * Event handler when the visibility of the property editor is changed.
       *
       * @param[in]  visible  True if the editor is visible.
       */
      void OnPropertyEditorVisibilityChange(bool visible);

      /**
       * Event handler when the visibility of the graph browser is changed.
       *
       * @param[in]  visible  True if the browser is visible.
       */
      void OnGraphBrowserVisibilityChange(bool visible);

      /**
       * Event handler when the visibility of the replay browser is changed.
       *
       * @param[in]  visible  True if the browser is visible.
       */
      void OnReplayBrowserVisibilityChange(bool visible);

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
      * Event handler when the save as button is pressed.
      */
      void OnSaveAsButton();

      /**
       * Event handler when the load button is pressed.
       */
      void OnLoadButton();

      /**
       * Event handler when the new button is pressed.
       */
      void OnNewButton();

      /**
       * Event handler when the load button is pressed.
       */
      void OnLoadRecordingButton();

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
       * Event handler when the cut button is pressed.
       */
      void OnCut();

      /**
       * Event handler when the copy button is pressed.
       */
      void OnCopy();

      /**
       * Event handler when the paste button is pressed.
       */
      void OnPaste();

      /**
       * Event handler when the delete button is pressed.
       */
      void OnDelete();

      /**
       * Event handler when the manager libraries button is pressed.
       */
      void OnManageLibraries();

      /**
       * Event handler when the show property editor button is pressed.
       */
      void OnShowPropertyEditor();

      /**
       * Event handler when the show graph browser button is pressed.
       */
      void OnShowGraphBrowser();

      /**
       * Event handler when the show replay browser button is pressed.
       */
      void OnShowReplayBrowser();

      /**
       * Event handler when the show links button is pressed.
       */
      void OnShowLinks();

      /**
       * Event handler when the hide links button is pressed.
       */
      void OnHideLinks();

      /**
       * Event handler when the refresh button is pressed.
       */
      void OnRefresh();

   protected:

      /**
       * Event handler when a key is pressed.
       *
       * @param[in]  e  The key event.
       */
      void keyPressEvent(QKeyEvent *e);

      /**
       * Event handler when the view is shown or hidden.
       */
      void showEvent(QShowEvent* event);

      /**
       * Clears the contents of the current script.
       */
      void ClearScript();

      /**
       * Saves the current script.
       *
       * @param[in]  saveAs  True if we want to show the file dialog.
       * 
       * @return  True if the script was saved.
       */
      bool SaveScript(bool saveAs = false);

      /**
       * Loads an existing script.
       *
       * @return  True if a script was loaded.
       */
      bool LoadScript();

   public:

      /**
       * Pastes the contents of the clipboard to the current scene.
       */
      void PasteNodes(bool createLinks = false);


      GraphTabs*           mGraphTabs;
      PropertyEditor*      mPropertyEditor;
      GraphBrowser*        mGraphBrowser;
      ReplayBrowser*       mReplayBrowser;
      UndoManager*         mUndoManager;

      dtCore::RefPtr<Director> mDirector;
      std::string              mFileName;

      bool                     mReplayMode;
      Director::RecordNodeData mReplayNode;
      InputLink*               mReplayInput;
      OutputLink*              mReplayOutput;

      std::map<std::string, CustomEditorTool*> mCustomTools;

      QMenuBar* mMenuBar;
      QToolBar* mFileToolbar;
      QToolBar* mEditToolbar;

      QMenu*    mFileMenu;
      QMenu*    mEditMenu;
      QMenu*    mViewMenu;

      // File Actions.
      QAction*  mNewAction;
      QAction*  mLoadAction;
      QAction*  mSaveAction;
      QAction*  mSaveAsAction;

      QAction*  mLoadRecordingAction;

      // Edit Actions.
      QAction*  mParentAction;
      QAction*  mSnapGridAction;
      QAction*  mUndoAction;
      QAction*  mRedoAction;
      QAction*  mDeleteAction;

      QAction*  mCutAction;
      QAction*  mCopyAction;
      QAction*  mPasteAction;

      QAction*  mLibrariesAction;

      // View Actions.
      QAction*  mViewPropertiesAction;
      QAction*  mViewGraphBrowserAction;
      QAction*  mViewReplayBrowserAction;
      QAction*  mShowLinksAction;
      QAction*  mHideLinksAction;
      QAction*  mRefreshAction;
   };

} // namespace dtDirector

#endif // DIRECTORQT_EDITOR
