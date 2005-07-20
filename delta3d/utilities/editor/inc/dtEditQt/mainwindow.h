/*
* Delta3D Open Source Game and Simulation Engine Level Editor
* Copyright (C) 2005, BMH Associates, Inc.
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Matthew W. Campbell
*/
#ifndef __MainWindow__h
#define __MainWindow__h

#include <QMainWindow>
#include <osg/ref_ptr>
#include <dtCore/uniqueid.h>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/map.h"


namespace dtEditQt 
{

    class PerspectiveViewport;
    class OrthoViewport;
    class PropertyEditor;
    class ActorTab;
    class ResourceBrowser;

    /**
     * This class is the main window of the application.  It contains the menu bar,
     * toolbar, statusbar, and main UI interface.
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:

        /**
         * Constructor
         */
        MainWindow();

        /**
         * Overloaded so that the context menu is not displayed.  By default, the MainWindow
         * displays a right-click menu when the right mouse button is clicked.  Unfortunantly,
         * this causes problems for other right mouse button behavior.
         * @todo
         *  Allow application wide right click menu when mouse is only over certain
         *  widgets.
         */
        QMenu *createPopupMenu() { return NULL; }

        /**
         * Finds the recently opened projects, if applicable
         * @return The number of projects found
         */
        std::vector<std::string> findRecentProjects();

        /**
         * Finds the recently opened maps, if applicable
         * @return A vector containing the maps
         */
        std::vector<std::string> findRecentMaps();

        /**
         * Searches for stored preferences
         */
        void findAndLoadPreferences();

    public slots:

        /**
         * This slot is triggered when the application is started and initialized.
         */
        void onEditorInitiated();

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
         * This slot will either hide or retrieve the actor tab window when the tab is 
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
        void onActorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property);

        /// Updates the window title
        void updateWindowTitle();

        void onActorProxyCreated(osg::ref_ptr<dtDAL::ActorProxy> proxy);

        void onActorProxyDestroyed(osg::ref_ptr<dtDAL::ActorProxy> proxy);

        void onMapPropertyChanged();

        /**
         * Starts a new wait cursor.  You MUST call endWaitCursor() for each 
         * startWaitCursor().
         *
         * @Note - This behavior is extremely trivial, but is pulled to mainWindow for 
         * future expansion
         */
        void startWaitCursor();

        /** 
         * Ends a previously started wait cursor.  You must call this for each startWaitCursor().
         *
         * @Note - This behavior is extremely trivial, but is pulled to mainWindow for 
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

    protected:
        /**
         * Called when the window receives the event to close itself.
         */
        void closeEvent(QCloseEvent *e);

    private:

        QMenu *fileMenu;
        QMenu *editMenu;
        QMenu *projectMenu;
        QMenu *selectionMenu;
        QMenu *windowMenu;
        QMenu *helpMenu;
        QMenu *recentProjs;
        QMenu *recentMaps;

        QToolBar *fileToolBar;
        QToolBar *editToolBar;
        QToolBar *undoToolBar;
        QToolBar *selectionToolBar;

        PerspectiveViewport *perspView;
        OrthoViewport *topView;
        OrthoViewport *sideView;
        OrthoViewport *frontView;

        // main controls
        PropertyEditor *propertyWindow;
        ActorTab *actorTab;
        ResourceBrowser *resourceBrowser;

        QWidget *mainViewportParent;

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
        QWidget *setupViewports();

        void checkAndLoadBackup(const std::string &str);

        friend class EditorActions;
    };
}

#endif
