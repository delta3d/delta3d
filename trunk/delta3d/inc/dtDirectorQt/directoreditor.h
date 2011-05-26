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

#include "ui_directoreditor.h"
#include <phonon/phonon>

class QAction;
class QMenuBar;
class QToolBar;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Phonon
{
   class MediaObject;
   enum State;
}
/// @endcond

namespace dtQt
{
   class DocBrowser;
}

namespace dtDirector
{
   class CustomEditorTool;
   class GraphBrowser;
   class GraphTabs;
   class NodeTabs;
   class PropertyEditor;
   class ReplayBrowser;
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
       * Loads an existing script.
       *
       * @param[in]  The file path to the script.
       *
       * @return     True if a script was loaded.
       */
      bool LoadScript(const std::string& fileName);

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
      Director* GetDirector() {return mDirector.get();}

      /**
       * Accessor for the graph tab widget.
       */
      GraphTabs* GetGraphTabs() { return mUI.graphTab; }

      /**
       * Accessor for the Property Editor.
       */
      PropertyEditor* GetPropertyEditor() {return mUI.propertyEditor;}

      /**
       * Accessor for the undo manager.
       */
      UndoManager* GetUndoManager() {return mUndoManager;}

      /**
       * Accessor for the replay browser.
       */
      ReplayBrowser* GetReplayBrowser() {return mUI.replayBrowser;}

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
      QAction* GetParentAction()   { return mUI.action_Step_Out_Of_Graph; }
      QAction* GetUndoAction()     { return mUI.action_Undo; }
      QAction* GetRedoAction()     { return mUI.action_Redo; }
      QAction* GetCutAction()      { return mUI.action_Cut; }
      QAction* GetCopyAction()     { return mUI.action_Copy; }
      QAction* GetPasteAction()    { return mUI.action_Paste; }
      QAction* GetDeleteAction()   { return mUI.action_Delete; }
      QAction* GetSnapGridAction() { return mUI.action_Smart_Grid_snap; }
      QAction* GetShowLinkAction() { return mUI.action_Show_Links; }
      QAction* GetHideLinkAction() { return mUI.action_Hide_Links; }
      QAction* GetRefreshAction()  { return mUI.action_Refresh; }

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
       * Refreshes a single node.
       *
       * @param[in]  node  The node to refresh.
       */
      void RefreshNode(Node* node);

      /**
       * Refreshes the glow for a single node.
       *
       * @param[in]  node  The node to refresh.
       */
      void RefreshGlow(Node* node);

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
      static bool RegisterCustomEditorTool(CustomEditorTool* tool);

      /**
       * UnRegisters a custom editor tool.
       *
       * @param[in]  tool      The tool to un-register.
       * @param[in]  toolName  The name of the tool ot un-register.
       *
       * @return     True if the tool was found and un-registered successfully.
       */
      static bool UnRegisterCustomEditorTool(CustomEditorTool* tool);
      static bool UnRegisterCustomEditorTool(const std::string& toolName);

      /**
       * Retrieves the tool of a given name.
       *
       * @param[in]  name  The name of the tool.
       *
       * @return     The tool, if found.
       */
      static CustomEditorTool* GetRegisteredTool(const std::string& name);

      /**
       * Retrieves a list of tools that have been registered.
       *
       * @param[in]  director  The director.
       *
       * @return  A list of registered tool names.
       */
      static std::vector<std::string> GetRegisteredToolList(Director* director);

      /**
       * Adds a custom editor to the node scene.
       *
       * @param[in]  name  The custom editor name.
       */
      void AddCustomEditor(const std::string& name);

      /**
       * Focuses the current view on the given node.
       *
       * @param[in]  node  The node to focus on.
       */
      void FocusNode(Node* node);

      /**
       * Focuses the current view on the given graph.
       *
       * @param[in]  graph  The graph to focus on.
       */
      void FocusGraph(DirectorGraph* graph);

      /**
       * Event handlers for debugging.
       */
      void OnBeginDebug();
      void OnEndDebug();

      void OnStepDebug();

   public slots:

      /**
       * Event handler when the visibility of the property editor is changed.
       *
       * @param[in]  visible  True if the editor is visible.
       */
      void on_propertyEditor_visibilityChanged(bool visible);

      /**
       * Event handler when the visibility of the graph browser is changed.
       *
       * @param[in]  visible  True if the browser is visible.
       */
      void on_graphBrowser_visibilityChanged(bool visible);

      /**
       * Event handler when the visibility of the search browser is changed.
       *
       * @param[in]  visible  True if the browser is visible.
       */
      void on_searchBrowser_visibilityChanged(bool visible);

      /**
       * Event handler when the visibility of the replay browser is changed.
       *
       * @param[in]  visible  True if the browser is visible.
       */
      void on_replayBrowser_visibilityChanged(bool visible);

      /**
       * Event handler when the visibility of the thread browser is changed.
       *
       * @param[in]  visible  True if the browser is visible.
       */
      void on_threadBrowser_visibilityChanged(bool visible);

      /**
       * Event handler when the visibility of the node palette is changed.
       *
       * @param[in]  visible  True if the browser is visible.
       */
      void on_nodePalette_visibilityChanged(bool visible);

      /**
      * Event handler when the current document tab has changed.
      *
      * @param[in]  index  The index of the new tab.
      */
      void on_graphTab_currentChanged(int index);

      /**
      * Event handler when the current document tab has closed.
      *
      * @param[in]  index  The index of the closed tab.
      */
      void on_graphTab_tabCloseRequested(int index);

      /**
       * Event handler when the save button is pressed.
       */
      void on_action_Save_triggered();

      /**
      * Event handler when the save as button is pressed.
      */
      void on_action_Save_as_triggered();

      /**
       * Event handler when the load button is pressed.
       */
      void on_action_Load_triggered();

      /**
       * Event handler when the new button is pressed.
       */
      void on_action_New_triggered();

      /**
       * Event handler when the load button is pressed.
       */
      void on_action_Load_Recording_triggered();

      /**
       * Event handler when the load button is pressed.
       */
      void on_action_Exit_triggered();

      /**
       * Event handler when the parent button is pressed.
       */
      void on_action_Step_Out_Of_Graph_triggered();

      /**
       * Event handler when the undo button is pressed.
       */
      void on_action_Undo_triggered();

      /**
       * Event handler when the redo button is pressed.
       */
      void on_action_Redo_triggered();

      /**
       * Event handler when the cut button is pressed.
       */
      void on_action_Cut_triggered();

      /**
       * Event handler when the copy button is pressed.
       */
      void on_action_Copy_triggered();

      /**
       * Event handler when the paste button is pressed.
       */
      void on_action_Paste_triggered();
      void on_actionPaste_with_Links_triggered();

      /**
       * Event handler when the delete button is pressed.
       */
      void on_action_Delete_triggered();

      /**
       * Event handler when the manager libraries button is pressed.
       */
      void on_action_Manage_Libraries_triggered();

      /**
       * Event handler when the show node palette button is pressed.
       */
      void on_action_Node_Palette_triggered();

      /**
       * Event handler when the show property editor button is pressed.
       */
      void on_action_Property_Editor_triggered();

      /**
       * Event handler when the show graph browser button is pressed.
       */
      void on_action_Graph_Browser_triggered();

      /**
       * Event handler when the show search browser button is pressed.
       */
      void on_action_Search_Browser_triggered();

      /**
       * Event handler when the show replay browser button is pressed.
       */
      void on_action_Replay_Browser_triggered();

      /**
       * Event handler when the show thread browser button is pressed.
       */
      void on_action_Thread_Browser_triggered();

      /**
       * Event handler when the show links button is pressed.
       */
      void on_action_Show_Links_triggered();

      /**
       * Event handler when the hide links button is pressed.
       */
      void on_action_Hide_Links_triggered();

      /**
       * Event handler when the refresh button is pressed.
       */
      void on_action_Refresh_triggered();

      /**
       * Event handler when the help button is pressed.
       */
      void on_actionDirector_Help_triggered();

      /**
       * Event handler for when the debug pause button is pressed.
       */
      void on_actionPause_triggered();

      /**
       * Event handler for when the debug continue button is pressed.
       */
      void on_actionContinue_triggered();

      /**
       * Event handler for when the debug step button is pressed.
       */
      void on_actionStep_Next_triggered();

      /**
       * Event handler when a node creation menu item is triggered.
       */
      void OnCreateNodeEvent(const QString& name, const QString& category);

      /**
       *  Play a "click" sound.  Used when connecting Links together.
       */
      void OnPlayClickSound();

   protected:

      /**
       * Event handler when a key is pressed.
       *
       * @param[in]  e  The key event.
       */
      void keyPressEvent(QKeyEvent *e);

      /**
       * Event handler when the view is shown.
       */
      void showEvent(QShowEvent* event);

      /**
       * Event handler when the view is hidden.
       */
      void hideEvent(QHideEvent* event);

      /**
       * Event handler when the window is closed.
       */
      void closeEvent(QCloseEvent* event);

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
      void PasteNodes(bool createLinks = false, bool externalLinks = false);

   private slots:

      ///Used for managing the sound states
      void OnStateChanged(Phonon::State newState, Phonon::State oldState);

   private:
      /**
       * Creates the node scenes
       */
      void CreateNodeScene(NodeTabs* nodeTabs);

      /**
       * Refreshes the node scenes with newly loaded node libraries
       */
      void RefreshNodeScenes();

      /**
       * Refreshes one given node scene to display newly loaded nodes
       *
       * @param widget   The widget containing the scene to update
       * @param nodeType The type of nodes to display in the scene
       */
      void RefreshNodeScene(NodeTabs* nodeTabs, NodeType::NodeTypeEnum nodeType);

      Ui::DirectorEditor       mUI;

      dtQt::DocBrowser*        mDocBrowser;

      UndoManager*             mUndoManager;

      dtCore::RefPtr<Director> mDirector;
      std::string              mFileName;

      bool                     mReplayMode;
      Director::RecordNodeData mReplayNode;
      InputLink*               mReplayInput;
      OutputLink*              mReplayOutput;

      Phonon::MediaObject *mClickSound;

      static std::map<std::string, CustomEditorTool*> mCustomTools;
      static std::vector<DirectorEditor*> mEditorsOpen;
   };

} // namespace dtDirector

#endif // DIRECTORQT_EDITOR
