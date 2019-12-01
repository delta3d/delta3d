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

#include <dtDirector/node.h>
#include <dtDirector/director.h>

#include <QtGui/QMainWindow>

#include <phonon/phonon>

class QAction;
class QMenuBar;
class QToolBar;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class DirectorEditor;
}

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
   class PluginManager;
   class UndoManager;
   class EditorNotifier;

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
       * Setup for the plugin manager.
       */
      void SetupPlugins();

      /**
       * Sets the Director.
       */
      void SetDirector(Director* director);

      /**
       * Retrieves whether we are currently in debugging mode.
       */
      bool IsDebugging() const;

      /**
       * Retrieves the editor notifier.
       */
      EditorNotifier* GetNotifier() const;

      /**
       * Loads an existing script.
       *
       * @param[in]  The file path to the script.
       *
       * @return     True if a script was loaded.
       */
      bool LoadScript(const std::string& fileName);

      /**
       * Saves the current state of the graph tabs.
       */
      void SaveTabStates();

      /**
       * Restores the state of the graph tabs.
       */
      void RestoreTabStates();

      /**
       * Refeshes the "New" script button menu based on what script
       * types are available.
       */
      void RefreshNewMenu();

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
      const Director* GetDirector() const {return mDirector.get();}

      /**
       * Accessor for the graph tab widget.
       */
      GraphTabs* GetGraphTabs();

      /**
       * Accessor for the Property Editor.
       */
      PropertyEditor* GetPropertyEditor();

      /**
       * Accessor for the Plugin Manager.
       */
      PluginManager* GetPluginManager() {return mPluginManager;}

      /**
       * Accessor for the undo manager.
       */
      UndoManager* GetUndoManager() {return mUndoManager;}

      /**
       * Retrieves actions.
       */
      QAction* GetParentAction();
      QAction* GetUndoAction();
      QAction* GetRedoAction();
      QAction* GetCutAction();
      QAction* GetCopyAction();
      QAction* GetPasteAction();
      QAction* GetDeleteAction();
      QAction* GetSnapGridAction();
      QAction* GetShowLinkAction();
      QAction* GetHideLinkAction();
      QAction* GetRefreshAction();

      /**
       * Refreshes the recent file listing.
       */
      void RefreshRecentFiles();

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
       *	Refreshes all graphs visible in tab views.
       */
      void RefreshGraphs();

      /**
       * Refreshes a single node.
       *
       * @param[in]  node  The node to refresh.
       */
      void RefreshNodeItem(Node* node);

      /**
       *	Refreshes a single graph node item.
       *
       * @param[in]  graph  The graph to refresh.
       */
      void RefreshGraphItem(DirectorGraph* graph);

      /**
       * Refreshes the glow for a single node.
       *
       * @param[in]  node  The node to refresh.
       */
      void RefreshGlow(Node* node);

      /**
       * Refreshes the graph browser.
       */
      void RefreshGraphBrowser();

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
      void DeleteNode(dtDirector::ID id);

      /**
       *	Deletes a director graph.
       *
       * @param[in]  id  The Id of the graph to delete.
       */
      void DeleteGraph(dtDirector::ID id);

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
      virtual dtCore::ActorPtrVector GetActorSelection()
      {return dtCore::ActorPtrVector();}

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
      void OnNewScriptTypeTriggered(QAction* action);

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
       * Event handler when the manage plugins button is pressed.
       */
      void on_action_Manage_Plugins_triggered();

      /**
       * Event handler when the manage libraries button is pressed.
       */
      void on_action_Manage_Libraries_triggered();

      /**
       *	Event handler when the manager imported scripts button is pressed.
       */
      void on_action_Manage_Imported_Scripts_triggered();

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
       * Event handler for when the toggle break point button is toggled.
       */
      void on_actionToggle_Break_Point_triggered();

      /**
       * Event handler for when the node search text has been edited.
       */
      void on_nodeSearchEdit_editingFinished();

      /**
       * Event handler for when a recent file has been selected.
       */
      void OnRecentFile(QAction* action);

      /**
       * Event handler when a node creation menu item is triggered.
       */
      void OnCreateNodeEvent(const QString& name, const QString& category, const QString& refName);

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
      void ClearScript(const std::string& scriptType = "");

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

      /**
       * Refreshes the node scenes with newly loaded node libraries
       */
      void RefreshNodeScenes();

   private slots:

      ///Used for managing the sound states
      void OnStateChanged(Phonon::State newState, Phonon::State oldState);

   private:
      /**
       * Creates the node scenes
       */
      void CreateNodeScene(NodeTabs* nodeTabs);

      /**
       * Refreshes one given node scene to display newly loaded nodes
       *
       * @param widget     The widget containing the scene to update
       * @param nodeType   The type of nodes to display in the scene
       */
      void RefreshNodeScene(NodeTabs* nodeTabs, NodeType::NodeTypeEnum nodeType);

      /**
       * Refreshes the reference node scene.
       */
      void RefreshReferenceScene();

      /**
       *	Restores the geometry of the window.
       */
      void RestoreWindow();

      /**
       * Sets the file name related variables.
       */
      void AssignFileNameVars(const std::string& fileName);

      void UpdateRecentFiles();
      void RemoveRecentFile();

      dtCore::RefPtr<Director> mNodeSceneDirector;

      Ui::DirectorEditor&       mUI;

      dtQt::DocBrowser*        mDocBrowser;
      PluginManager*           mPluginManager;

      UndoManager*             mUndoManager;

      dtCore::RefPtr<Director> mDirector;
      std::string              mFileName;
      std::string              mFullFileName;

      struct TabStateData
      {
         dtDirector::ID id;
         QPointF        pos;
      };
      std::vector<TabStateData>mTabStates;
      int                      mSavedTabIndex;

      Phonon::MediaObject*     mClickSound;

      static std::map<std::string, CustomEditorTool*> mCustomTools;
      static std::vector<DirectorEditor*> mEditorsOpen;
   };

} // namespace dtDirector

#endif // DIRECTORQT_EDITOR
