/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Matthew W. Campbell
 */

#ifndef DELTA_MAIN_WINDOW
#define DELTA_MAIN_WINDOW

#include <dtEditQt/export.h>

DT_DISABLE_WARNING_ALL_START
#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtCore/QMap>
DT_DISABLE_WARNING_END

#include <dtCore/actorproxy.h>
#include <dtCore/objecttype.h>
#include <dtQt/typedefs.h>

class QSplitter;
class QActionGroup;

namespace dtActors
{
   class VolumeEditActor;
   class VolumeEditActorProxy;
}

namespace dtQt
{
   class ActorTreePanel;
}

namespace dtEditQt
{
   class ActorDockWidget;
   class ActorSearchDockWidget;
   class EditorSettings;
   class EditorViewportContainer;
   class OrthoViewport;
   class PerspectiveViewport;
   class PluginManager;
   class PropertyEditor;
   class ResourceBrowser; // OLD
   class ViewportContainer;



   /**
    * This class is the main window of the application.  It contains the menu bar,
    * toolbar, statusbar, and main UI interface.
    */
   class DT_EDITQT_EXPORT MainWindow : public QMainWindow
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      MainWindow(const std::string& stageConfigFile = "");

      /**
       * Destructor
       */
      virtual ~MainWindow();

      /**
       * Adds custom library paths as specified by the user settings
       * @return No return value
       */
      void loadLibraryPaths();

      /**
       * Loads the last Project Context if there is one defined and the "load
       * last project" user preference is set to true.
       * @return true if Project Context was set
       */
      bool LoadLastProject();

      /**
       * Finds the recently opened maps, if applicable
       * @return A vector containing the maps
       */
      std::vector<std::string> FindRecentMaps();

      /**
       * Searches for stored preferences
       */
      void findAndLoadPreferences();

      /**
       * Saves the preferences to file.
       */
      void WritePreferences();

      /**
      * Adds an exclusive tool into the tool mode bar.
      */
      void AddExclusiveToolMode(QAction* action);

      /**
      * Finds and returns the tool mode button of a given label.
      *
      * @param[in]  name  The name of the tool mode to find.
      *
      * @return     A pointer to the action, or NULL if none was found.
      */
      QAction* FindExclusiveToolMode(std::string name);

      /**
      * Removes an exclusive tool from the tool mode bar.
      */
      void RemoveExclusiveToolMode(QAction* action);

      /**
      * Returns to the normal tool mode.
      */
      void SetNormalToolMode();

      /**
       * @return the property editor.
       */
      PropertyEditor* GetPropertyEditor();


      /**
       * @return the full path to the config file for currently running STAGE
       **/
      const std::string& GetSTAGEConfigFile();

      /**
       * menu accessors
       */
      QMenu* GetFileMenu() const    { return mFileMenu; }
      QMenu* GetEditMenu() const    { return mEditMenu; }
      QMenu* GetProjectMenu() const { return mProjectMenu; }
      QMenu* GetWindowMenu() const  { return mWindowMenu; }
      QMenu* GetHelpMenu() const    { return mHelpMenu; }
      QMenu* GetToolsMenu() const   { return mToolsMenu; }

      /**
      * toolbar accessors.
      */
      QToolBar* GetFileToolbar() const          { return mFileToolBar; }
      QToolBar* GetEditToolbar() const          { return mEditToolBar; }
      QToolBar* GetUndoToolbar() const          { return mUndoToolBar; }
      QToolBar* GetToolsToolbar() const         { return mToolsToolBar; }
      QToolBar* GetExternalToolsToolbar() const { return mExternalToolsToolBar; }

      /**
      * viewport accessors.
      */
      PerspectiveViewport* GetPerspView() const { return mPerspView; }
      OrthoViewport*       GetTopView() const   { return mTopView; }
      OrthoViewport*       GetSideView() const  { return mSideView; }
      OrthoViewport*       GetFrontView() const { return mFrontView; }
      EditorViewportContainer* GetViewContainer() const { return mEditorContainer; }

      /**
       * @return the plugin manager
       */
      PluginManager* GetPluginManager() { return mPluginManager; }

      /**
      * @return the volume edit "actor"
      */
      dtActors::VolumeEditActor* GetVolumeEditActor();

      /**
       * @return the volume edit actor proxy
       */
      dtActors::VolumeEditActorProxy* GetVolumeEditActorProxy();

      /**
       * Method used to filter only actor component types.
       */
      bool IsActorComponentType(const dtCore::ObjectType& objType) const;

      /**
       * Method used to filter only actor types.
       */
      bool IsActorType(const dtCore::ObjectType& objType) const;

   public slots:
      /**
       * This slot is triggered when the application is started and initialized.
       */
      void onEditorInitiated();

      /**
       *    Auto loads the last loaded map.
       */
      void onAutoLoadMap();

      /**
       * This slot is triggered just before the editor is about to shut down.  This is
       * where the main window settings (size, position, docked windows, etc.) are
       * saved.
       */
      void onEditorShutDown();

      /**
       * Enables or disables all the actions on the main window depending on the state of data.
       * There should be a project and/or a map for most actions.  Call this when something
       * major changes in the app to disable the UI appropriately.  Note that this is tied to
       * the EditorEvents::editorInitiationEvent.
       */
      void enableActions();

      /**
       * This slot will either hide or retrieve the property editor window when the tab is
       * selected from the main menu
       */
      void onPropertyEditorSelection();

      /**
       * Slot to handle the AddActorComponent action.
       */
      void onAddActorComponents();

      /**
       * Slot to handle the AddActorComponent action.
       */
      void onRemoveActorComponents();

      /**
       * Slot to handle the ChangeActorType action.
       */
      void onChangeActorType();

      /**
      * This slot will either hide or retrieve the actor dockwidget when
      * selected from the main menu
      */
      void onActorSelection();

      /**
       * This slot will either hide or retrieve the actor search dockwidget when
       * selected from the main menu
       */
      void onActorSearchSelection();

      /**
       * This slot will either hide or retrieve the resource browser window when the tab is
       * selected from the main menu
       */
      void onResourceBrowserSelection();

      /**
       * Called when an actor property changes.  This method will then flag the
       * current map as being modified.
       * @param proxy The proxy that was changed.
       * @param property The property of the proxy that was modified.
       */
      void MarkEdited();

      /// Updates the window title
      void updateWindowTitle();

      void onActorProxyNameChanged(dtCore::BaseActorObject& proxy, std::string oldName);

      /**
       * Starts a new wait cursor.  You MUST call endWaitCursor() for each
       * startWaitCursor().
       *
       * @note - This behavior is extremely trivial, but is pulled to mainWindow for
       * future expansion
       */
      void startWaitCursor();

      /**
       * Ends a previously started wait cursor.  You must call this for each startWaitCursor().
       *
       * @note - This behavior is extremely trivial, but is pulled to mainWindow for
       * future expansion
       */
      void endWaitCursor();

      /**
       * Restores the docking windows to the default state
       */
      void onResetWindows();

      // This function sets the checkmarks in the Window menu. I used to do this explicitly
      // in onEditorInitiated, but it doesn't function correctly because the actual windows
      // aren't visible yet.
      void setWindowMenuTabsChecked();

      void RebuildToolsMenu(const QList<QAction*>& actions);

      /**
       * Shows a specific message on the status bar.
       */
      void showStatusBarMessage(const QString message, int timeout);

      /**
       * The recent projects menu
       */
      void RefreshRecentProjectsMenu();

   protected:
      /**
       * Called when the window receives the event to close itself.
       */
      void closeEvent(QCloseEvent* e);

      /**
       * Called when the user presses a key on the keyboard in the viewport.
       * Based on the combination of keys pressed, the viewport's current
       * mode will be set.
       * @param e
       */
      void keyPressEvent(QKeyEvent* e);

   private:
      PluginManager* mPluginManager;
      std::string mSTAGEConfigFullPath;

      dtCore::RefPtr<dtActors::VolumeEditActorProxy> mVolEditActorProxy;

      QMenu* mFileMenu;
      QMenu* mEditMenu;
      QMenu* mProjectMenu;
      QMenu* mWindowMenu;
      QMenu* mHelpMenu;
      QMenu* mRecentProjs;
      QMenu* mRecentMaps;
      QMenu* mToolsMenu;

      QToolBar* mFileToolBar;
      QToolBar* mEditToolBar;
      QToolBar* mUndoToolBar;
      QToolBar* mBrushToolBar;
      QToolBar* mToolsToolBar;
      QToolBar* mExternalToolsToolBar;

      QActionGroup* mToolModeActionGroup;
      QAction* mNormalToolMode;
      QAction* mAddActorComponent;
      QAction* mRemoveActorComponent;
      QAction* mChangeActorType;

      PerspectiveViewport* mPerspView;
      OrthoViewport* mTopView;
      OrthoViewport* mSideView;
      OrthoViewport* mFrontView;
      EditorViewportContainer* mEditorContainer;

      // main controls
      PropertyEditor*  mPropertyWindow;
      ActorDockWidget*        mActorDockWidg;
      ActorSearchDockWidget*  mActorSearchDockWidg;
      ResourceBrowser* mResourceBrowser;

      QDockWidget* mActorTreeDock;
      dtQt::ActorTreePanel* mActorTreePanel;
      

      QWidget*          mMainViewportParent;
      QList<QSplitter*> mSplitters;

      ///A map of Viewport names to their ViewportContainers
      QMap<QString, ViewportContainer*> mViewportContainers;

      /**
       * Connects the signals and slots the main window needs.
       */
      void connectSlots();

      /**
       * Create the main menus.
       */
      void setupMenus();

      /**
       * Create the toolbar.
       */
      void setupToolbar();

      /**
       * Creates the status bar and attaches it to the main window.
       */
      void setupStatusBar();

      /**
       * Creates the docking windows and central widget.
       * @note
       *      The Qt docking framework consists of docking areas in the borders
       *      of the main window as well as a central widget.  The central widget
       *      is not a dockable entity.  Therefore, the central widget is a
       *      nested splitter window which contains the 4 viewports.  All tools
       *    (browsers, viewers, etc.) are dockable around the viewports.
       */
      void setupDockWindows();

      /**
       * Creates the splitter windows and viewport widgets.
       * @return The root splitter pane for the viewports.
       */
      QWidget* setupViewports();

      /**
       * Creates the pseudo-actor (it's not in the map) that functions as a "cursor"
       * or "stamp" that allows us to perform operations inside it.  For example:
       * create a random number of Actors inside this volume.
       */
      void setupVolumeEditActor();

      void checkAndLoadBackup(const std::string& str);

      /**
       * Check on filesystem if a map does not exist.
       * @return TRUE if the specified map does not exist.
       */
      bool MapDoesNotExist( const std::string& mapToLoad );

      /**
       * Reset the splitters to equally divide the screen.
       */
      void ResetSplitters();

      /** Try to find and load any plugins */
      void SetupPlugins();

      /** Reads the recently loaded Project Contexts from the QSettings and tells
        * the EditorData about them.
        */
      void SetupRecentProjects() const;

      void ReadCameraSpeed(const EditorSettings &settings, const QString& viewName) const;
      void ReadClipDistance(const EditorSettings& settings, const QString& viewName) const;

      friend class EditorActions;
   };

} // namespace dtEditQt

#endif // DELTA_MAIN_WINDOW
