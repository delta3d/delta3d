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

#include <dtEditQt/export.h>
#include <QtCore/QObject>
#include <vector>
#include <osg/Referenced>
#include <dtCore/actorproxy.h>
#include <dtQt/typedefs.h>
#include <dtCore/refptr.h>
#include <dtActors/volumeeditactor.h>

class QAction;
class QActionGroup;
class QTimer;


namespace dtCore
{
   class Map;
}

namespace dtQt
{
   class DocBrowser;
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
   class DT_EDITQT_EXPORT EditorActions : public QObject, public osg::Referenced
   {
      Q_OBJECT

   public:
      int  mSaveMilliSeconds;
      bool mWasCancelled;

      static const std::string PREFAB_DIRECTORY;

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

      /// Accessor to the timer
      inline QTimer* getTimer() { return mTimer; }

      /**
       * This method is used by several processes to delete a proxy.  It removes
       * the proxy from the scene and map and clears up the billboard.  Finally, it emits
       * the appropriate about to and delete proxy events.
       */
      bool deleteProxy(dtCore::BaseActorObject* proxy, dtCore::RefPtr<dtCore::Map> currMap);

      ///Get the container of ExternalTools, const-like
      const QList<ExternalTool*>& GetExternalTools() const;

      ///Get the container of ExternalTools in a modifiable kind of way.
      QList<ExternalTool*>& GetExternalTools();

      const QList<QAction*> GetExternalToolActions() const;

      /**
       * Method to save a new prefab.  Returns true on success.
       */
      bool SaveNewPrefab(std::string category, std::string prefabName,
                         std::string iconFile, std::string prefabDescrip);

      /**
       * Ask for saving current changes (if any) and create(/load) a new EMPTY map.
       */
      void createNewEmptyMap(const std::string& mapToLoad);

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
       * @note If any errors occurred while saving the current map, this method
       *  will return QMessageBox::Abort.
       */
      int SaveCurrentMapChanges(bool askPermission);

      /** 
       * Set the volume editor's brush shape. Will adjust the shape and update the
       * toolbar icons.
       * @param shapeType The shape to use. 
       */
      void setBrushShape(const dtActors::VolumeEditActor::VolumeShapeType& shapeType);

      /**
       * Method to call for over seeing actor insertion into a map.
       * This method enables the editor to perform proper book keeping
       * so that all other parts of the application can function as intended.
       * @param actor Actor to be added to the map.
       * @param map The map object to which to add the specified actor.
       * @param renumber Optional parameter used by the map to determine if the actor needs a unique number suffix added to the actor name.
       */
      void AddActorToMap(dtCore::BaseActorObject& actor, dtCore::Map& map, bool renumber = false);
   
      /**
       * Method to call for over seeing actor removal from a map.
       * This method enables the editor to perform proper book keeping
       * so that all other parts of the application can function as intended.
       * @param actor Actor to be removed from the map.
       * @param map The map object from which to remove the specified actor.
       * @return TRUE if the actor was found and successfully removed from the map.
       */
      bool RemoveActorFromMap(dtCore::BaseActorObject& actor, dtCore::Map& map);


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

      // Action - File - Export Prefab
      QAction* mActionFileExportPrefab;

      // Action - File - Edit Preferences
      QAction* mActionFileEditPreferences;

      // Action - File - Change Project
      QAction* mActionFileChangeProject;

      ///List of Actions corresponding to the recently loaded Project Contexts
      std::vector<QAction*> mActionRecentProjects;

      // Action - File - Recent Map0
      QAction* mActionFileRecentMap0;

      // Action - File - Exit
      QAction* mActionFileExit;

      // Action - Edit - Local Space
      QAction* mActionLocalSpace;

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

      // Action - Group actors.
      QAction* mActionGroupActors;

      // Action - Ungroup actors.
      QAction* mActionUngroupActors;

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

      // Action - Edit - Resets the translation on the current selection
      QAction* mActionEditResetTranslation;

      // Action - Edit - Resets the rotation on the current selection
      QAction* mActionEditResetRotation;

      // Action - Edit - Resets the scale on the current selection
      QAction* mActionEditResetScale;

      // Action - Brush - Brush Shape
      QAction* mActionBrushShape;

      // Action - Brush - Reset Brush
      QAction* mActionBrushReset;

      // Action - Brush - Hide/Show Brush
      QAction* mActionHideShowBrush;

      // Action - Tools - Hide/Show Trigger Volumes
      QAction* mActionHideShowTriggers;

      // Action - File - Library Paths Dialog
      QAction* mActionFileEditLibraryPaths;

      // Action - Edit - Task Editor
      QAction* mActionEditTaskEditor;

      // Action - Project - Change project context.
      //QAction* actionProjectChangeContext;

      // Action - Property Editor - Hides/Brings back the property editor window
      QAction* mActionWindowsPropertyEditor;

      // Action - Property Editor - Hides/Brings back the property editor window
      QAction* mActionWindowsActor;

      // Action - Actor Search - Hides/Brings back the actor tab window
      QAction* mActionWindowsActorSearch;

      // Action - Actor Tree Panel - Hides/Brings back the actor tree panel window
      QAction* mActionWindowsActorTreePanel;

      // Action - Resource Browser - Hides/Brings back the resource browser window
      QAction* mActionWindowsResourceBrowser;

      // Action - Reset Windows - Resets the docking windows to a default state
      QAction* mActionWindowsResetWindows;

      // Add a new external tool
      QAction* mActionAddTool;

      // Action - Help - Editor Help.
      QAction* mActionHelpEditorHelp;

      // Action - Help - About Delta Level Editor
      QAction* mActionHelpAboutEditor;

      // Action - Help - About QT
      QAction* mActionHelpAboutQT;

      //Group of external tool QActions
      QActionGroup* mExternalToolActionGroup;

      // STAGE Document Browser.
      dtQt::DocBrowser* mDocBrowser;

      ///Align the Camera to the selected actor action
      QAction* mAlignCameraToActorAction;

      ///Align the selected actor(s) to the camera action
      QAction* mAlignActorToCameraAction;

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
      * Slot - File Export prefab.
      */
      void slotFileExportPrefab();

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
      void slotEditDuplicateActors(bool jitterClone = true);

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
       * Slot - Edit - Group Actors.
       */
      void slotEditGroupActors();

      /**
      * Slot - Edit - Ungroup Actors.
      */
      void slotEditUngroupActors();

      /**
       * Slot - Edit - Undo.  Undoes the last user edit property, delete proxy, or create proxy command.
       */
      void slotEditUndo();

      /**
       * Slot - Edit - Redo.  Reapplies the previously undone command
       */
      void slotEditRedo();

      /**
      * Slot - Edit - Reset Translation.
      */
      void slotEditResetTranslation();

      /**
      * Slot - Edit - Reset Rotation.
      */
      void slotEditResetRotation();

      /**
      * Slot - Edit - Reset Scale.
      */
      void slotEditResetScale();

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
       * Slot - Cycle through the STAGE Brush shapes (box, sphere, cone, etc)
       */
      void slotCycleBrushShape();


      /**
       * Slot - Reset the STAGE Brush (reset to cube and put in front of camera)
       */
      void slotResetBrush();

      /**
      * Slot - Show/Hide the STAGE Brush
      */
      void slotShowHideBrush();

      /**
       * Slot - Show/Hide Trigger Volumes
       */
      void slotShowHideTriggers();

      /**
       * Slot - File Library Paths event.  Spawns the edit library paths dialog.
       */
      void slotFileEditLibraryPaths();

      /**
       * Slot - Project - Change context.  Spawns a dialog to change the current project context.
       */
      void slotProjectChangeContext();

      /**
       * Opens a recent project context, defined by the triggered Action
       */
      void slotOpenRecentProject();
      
      /** 
       * Opens the Project Context defined by the supplied path
       * @param path The full path to the Project Context to open
       */
      void SlotChangeProjectContext(const std::string& path);

      /**
       * Slot - File - Recent Maps - 0 Opens a recent map
       */
      void slotFileRecentMap0();

      ///Open the New External Tool editor window
      void SlotNewExternalToolEditor();

      /**
      * Slot - Help - How.  Spawns the editor help box.
      */
      void slotHelpEditorHelp();

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
      void slotSelectedActors(ActorRefPtrVector& actors);

      void slotOnActorCreated(dtCore::ActorPtr proxy, bool forceNoAdjustments);

      void slotGetGotoPosition();

      void slotChangeActorParent(dtCore::ActorPtr actor, dtCore::ActorPtr oldParent, dtCore::ActorPtr newParent);
      void slotDetachActorParent(dtCore::ActorPtr actor, dtCore::ActorPtr oldParent);

   signals:
      void ExternalToolsModified(const QList<QAction*>&);

      void PrefabExported();

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
      void changeMaps(dtCore::Map* oldMap, dtCore::Map* newMap);

      void HandleMissingLibraries(const dtCore::Map& newMap) const;

      void HandleMissingActorsTypes(const dtCore::Map& newMap) const;


      void setupFileActions();
      void setupEditActions();
      void SetupToolsActions();
      void setupHelpActions();
      void setupWindowActions();

      QTimer* mTimer;
      std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > mActors;

      ///Singleton instance of this class.
      static dtCore::RefPtr<EditorActions> sInstance;

      QList<ExternalTool*> mTools;
      QList<const ExternalToolArgParser*> mExternalToolArgParsers;

      bool mShowingTriggerVolumes;
   };

} // namespace dtEditQt

#endif // DELTA_EDTIOR_ACTIONS
