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
 * Curtiss Murphy
 * William E. Johnson II
 */

#ifndef DELTA_EDTIOR_ACTIONS
#define DELTA_EDTIOR_ACTIONS

#include <QtCore/QObject>
#include <vector>
#include <osg/Referenced>
#include <dtDAL/actorproxy.h>
#include <dtEditQt/typedefs.h>
#include <dtCore/refptr.h>

class QAction;
class QActionGroup;
class QListWidgetItem;
class QListWidget;
class QDialog;
class QTimer;

namespace dtCore
{
   class Isector;
}

namespace dtDAL
{
   class Map;
}

namespace dtEditQt
{
   class ExternalTool;
   class ExternalToolArgParser;

   /**
    * This class holds all the UI QActions.  It has a list of the actions that are
    * used for menus, dialogs, popups, and shortcuts.  Each action uses the signal/slot
    * behavior from QT.
    * @note The EditorActions class follows the Singleton pattern.
    */
   class EditorActions : public QObject, public osg::Referenced
   {
      Q_OBJECT

   public:
      int  mSaveMilliSeconds;
      bool mWasCancelled;

      /**
       * Gets the singleton instance of the EditorActions.
       * @return Returns the singleton instance.
       */
      static EditorActions& GetInstance();

      /**
       * Returns the window name with a *appended, if modified
       * @return The new name
       */
      const std::string getWindowName() const;//bool modified = false, bool includesMap = true);

      // Initialized the recent items actions
      void setupRecentItems();

      /// Accessor to the timer
      inline QTimer* getTimer() { return mTimer; }

      /**
       * This method is used by several processes to delete a proxy.  It removes
       * the proxy from the scene and map and clears up the billboard.  Finally, it emits
       * the appropriate about to and delete proxy events.
       */
      bool deleteProxy(dtDAL::ActorProxy* proxy, dtCore::RefPtr<dtDAL::Map> currMap);

      ///Get the container of ExternalTools, const-like
      const QList<ExternalTool*>& GetExternalTools() const;

      ///Get the container of ExternalTools in a modifiable kind of way.
      QList<ExternalTool*>& GetExternalTools();

      const QList<QAction*> GetExternalToolActions() const;

      /**
       * The actions for this class are public.  Essentially, this whole class is here
       * to create these, trap their events, and expose them.
       */

      // Action - File - New Map
      QAction* mActionFileNewMap;

      // Action - File - Open Map
      QAction* mActionFileOpenMap;

      // Action - File - Close Map
      QAction* mActionFileCloseMap;

      // Action - File - Save Map
      QAction* mActionFileSaveMap;

      // Action - File - Save Map As
      QAction* mActionFileSaveMapAs;

      // Action - File - Edit Preferences
      QAction* mActionFileEditPreferences;

      // Action - File - Change Project
      QAction* mActionFileChangeProject;

      // Action - File - Recent Project0
      QAction* mActionFileRecentProject0;

      // Action - File - Recent Project1
      QAction* mActionFileRecentProject1;

      // Action - File - Recent Project2
      QAction* mActionFileRecentProject2;

      // Action - File - Recent Project3
      QAction* mActionFileRecentProject3;

      // Action - File - Recent Map0
      QAction* mActionFileRecentMap0;

      // Action - File - Recent Map1
      QAction* mActionFileRecentMap1;

      // Action - File - Recent Map2
      QAction* mActionFileRecentMap2;

      // Action - File - Recent Map3
      QAction* mActionFileRecentMap3;

      // Action - File - Exit
      QAction* mActionFileExit;

      // Action - Edit - Local Space
      QAction* actionLocalSpace;

      // Action - Edit - Duplicate Actor
      QAction* mActionEditDuplicateActor;

      // Action - Edit - Delete Actor
      QAction* mActionEditDeleteActor;

      // Action - Edit Clamp actors to ground.
      QAction* mActionEditGroundClampActors;

      // Action - Edit - Goto Actor
      QAction* mActionEditGotoActor;

      // Action - Goto position
      QAction* mActionGetGotoPosition;

      // Action - Edit - Undo
      QAction* mActionEditUndo;

      // Action - Edit - Redo
      QAction* mActionEditRedo;

      // Action - Edit - Map Libraries Editor
      QAction* mActionEditMapLibraries;

      // Action - Edit - Map Properties Editor
      QAction* mActionEditMapProperties;

      // Action - Edit - Map Events Editor
      QAction* mActionEditMapEvents;

      // Action - File - Library Paths Dialog
      QAction* mActionFileEditLibraryPaths;

      // Action - Edit - Task Editor
      QAction* mActionEditTaskEditor;

      // Action - Project - Change project context.
      //QAction* actionProjectChangeContext;

      // Action - Property Editor - Hides/Brings back the property editor window
      QAction* mActionWindowsPropertyEditor;

      // Action - Actor Search - Hides/Brings back the actor tab window
      QAction* mActionWindowsActorSearch;

      // Action - Resource Browser - Hides/Brings back the resource browser window
      QAction* mActionWindowsResourceBrowser;

      // Action - Reset Windows - Resets the docking windows to a default state
      QAction* mActionWindowsResetWindows;

      // Add a new external tool
      QAction* mActionAddTool;

      // Action - Help - About Delta Level Editor
      QAction* mActionHelpAboutEditor;

      // Action - Help - About QT
      QAction* mActionHelpAboutQT;

      //Group of external tool QActions
      QActionGroup* mExternalToolActionGroup;

   public slots:

      /**
       * Slot - File New event.  Creates a new map.
       */
      void slotFileNewMap();

      /**
       * Slot - File Open event.  Opens an existing map.
       */
      void slotFileOpenMap();

      /**
       * Slot - File close event. Closes the current map.
       */
      void slotFileCloseMap();

      /**
       * Slot - File Save event.  Saves the current map.
       */
      void slotFileSaveMap();

      /**
       * Slot - File Save As event.  Saves the current map as a different name,
       */
      void slotFileSaveMapAs();

      /**
       * Slot - File Edit Preferences event. Spawns the preference editor
       */
      void slotFileEditPreferences();

      /**
       * Slot - File Exit event.  Exits the app.
       */
      void slotFileExit();

      /**
      * Slot - Edit - Local space event.  Sets the edit gizmos to local space.
      */
      void slotEditLocalSpace();

      /**
       * Slot - Edit - Duplicate event.  Copies the current actor selection and added the
       * copies to the scene.
       */
      void slotEditDuplicateActors();

      /**
       * Slot - Edit - Delete event.  Removes the current actor selection from the scene.
       */
      void slotEditDeleteActors();

      /**
       * Slot - Edit - Ground clamp event. Clamps the current actor selection to be in line with what is
       * below them.
       */
      void slotEditGroundClampActors();


      /**
       * Slot - Edit - Task Editor
       */
      void slotTaskEditor();

      /**
       * Slot - Edit - Goto Actor. Places the camera at the selected actor.
       */
      void slotEditGotoActor();

      /**
       * Slot - Edit - Undo.  Undoes the last user edit property, delete proxy, or create proxy command.
       */
      void slotEditUndo();

      /**
       * Slot - Edit - Redo.  Reapplies the previously undone command
       */
      void slotEditRedo();

      /**
       * Slot - Edit Lib Edit event.  Spawns the library editor.
       */
      void slotEditMapLibraries();

      /**
       * Slot - Edit Game Events Edit event.  Spawns the game event editor.
       */
      void slotEditMapEvents();

      /**
       * Slot - Edit Map Properties event.  Spawns the map properties editor.
       */
      void slotEditMapProperties();

      /**
       * Slot - File Library Paths event.  Spawns the edit library paths dialog.
       */
      void slotFileEditLibraryPaths();

      /**
       * Slot - Project - Change context.  Spawns a dialog to change the current project context.
       */
      void slotProjectChangeContext();

      /**
       * Slot - File - Recent Projects - 0 Opens a recent project
       */
      void slotFileRecentProject0();

      /**
       * Slot - File - Recent Maps - 0 Opens a recent map
       */
      void slotFileRecentMap0();

      ///Open the New External Tool editor window
      void SlotNewExternalToolEditor();

      /**
       * Slot - Help - About Editor. Spawns the editor about box.
       */
      void slotHelpAboutEditor();

      /**
       * Slot - Help - About QT. Spawns the QT about box.
       */
      void slotHelpAboutQT();

      /// Refreshes the list of recent projects.
      void refreshRecentProjects();

      /// Refreshes the list of recent maps
      void refreshRecentMaps();

      // used by QTimer to autosave maps
      void slotAutosave();

      // used to determine when to pause the autosave
      void slotPauseAutosave();

      // restarts the autosave
      void slotRestartAutosave();

      // helper method to keep track of the selected actors. this is needed in order to
      // be able to call the goto actor functionality from the menu. The signal requires
      // an argument.
      void slotSelectedActors(ActorProxyRefPtrVector& actors);

      void slotOnActorCreated(ActorProxyRefPtr proxy, bool forceNoAdjustments);

      void slotGetGotoPosition();


   signals:
      void ExternalToolsModified(const QList<QAction*>&);

   protected:
      /**
       * Protected destructor, but does nothing.  Protected because it's a singleton
       */
      virtual ~EditorActions();

   private:
      /**
       * Constructs the EditorActions class.   This is private because it's a singleton
       */
      EditorActions();
      EditorActions& operator=(const EditorActions& rhs);
      EditorActions(const EditorActions& rhs);

      friend class MainWindow;

      /**
       * Helper method called when an action requires the current map to be changed.
       * @param oldMap The map that is currently loaded and needs to be unloaded.
       * @param newMap The new map to load.
       */
      void changeMaps(dtDAL::Map* oldMap, dtDAL::Map* newMap);

      void HandleMissingLibraries(const dtDAL::Map& newMap) const;

      void HandleMissingActorsTypes(const dtDAL::Map& newMap) const;

      /**
       * If the current map is valid and has been modified, this method will
       * prompt the user for asking to save the changes.
       * @param askPermission If true a message box will be displayed asking
       *  the user whether or not the changes should be saved.
       * @return
       *  If askPermission is true, the return value will be the response from
       *  the user. This will be equal to either QMessageBox::Yes, QMessageBox::No,
       *  or QMessageBox::Cancel.  If ask permission is false, the return value
       *  is QMessageBox::Ignore.
       * @note If the current map is invalid, this method will return
       *  QMessageBox::Ignore.
       * @note If any errors occured while saving the current map, this method
       *  will return QMessageBox::Abort.
       */
      int saveCurrentMapChanges(bool askPermission);

      void setupFileActions();
      void setupEditActions();
      void SetupToolsActions();
      void setupHelpActions();
      void setupWindowActions();

      QTimer* mTimer;
      std::vector< dtCore::RefPtr<dtDAL::ActorProxy> > mActors;

      ///Singleton instance of this class.
      static dtCore::RefPtr<EditorActions> sInstance;

      dtCore::RefPtr<dtCore::Isector> mIsector;

      QList<ExternalTool*> mTools;
      QList<const ExternalToolArgParser*> mExternalToolArgParsers;
   };

} // namespace dtEditQt

#endif // DELTA_EDTIOR_ACTIONS
