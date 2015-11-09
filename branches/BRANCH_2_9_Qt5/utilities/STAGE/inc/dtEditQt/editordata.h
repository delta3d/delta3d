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
 */
#ifndef DELTA_EDITOR_DATA
#define DELTA_EDITOR_DATA

#include <dtEditQt/export.h>
#include <QtGui/QColor>
#include <osg/Referenced>
#include <vector>
#include <string>
#include <list>
#include <dtEditQt/undomanager.h>
#include <dtCore/resourcedescriptor.h>

namespace dtCore
{
   class Map;
}

namespace dtEditQt
{

   class MainWindow;
   class UndoManager;
   class GroupUIRegistry;

   /**
    * This class holds all the main data for the editor.  In essence, it is the singleton global
    * repository.  Go here to get to the main window, the current map, selected actors, etc...
    * @note The EditorData class follows the Singleton pattern.
    */
   class DT_EDITQT_EXPORT EditorData : public osg::Referenced
   {
   public:
      /**
       * Gets the singleton instance of the this class
       *
       * @return Returns the singleton instance
       */
      static EditorData& GetInstance();

      /**
       * Sets the Main Window.
       *
       * @param Takes the current map. Should be gotten from opening a map or creating a new map
       */
      void setCurrentMap(dtCore::Map* map);

      /**
       * Gets the Current Map.
       *
       * @return Returns the current map that was set when opening or creating a new map
       */
      dtCore::Map* getCurrentMap();

      /**
       * Fills a vector with pointers to all the currently selected actor proxies.
       */
      void GetSelectedActors(dtCore::ActorPtrVector& toFill);

      /** 
       * Set the currently selected resource.
       * @param type The DataType of the resource that's selected
       * @param selectedResource The ResourceDescriptor of what's selected
       */
      void setCurrentResource(dtCore::DataType& type,
                              const dtCore::ResourceDescriptor& selectedResource);

      /** 
       * Get the currently selected resource, based on supplied DataType.
       * @param type The DataType describing the Resource you're looking for.
       */
      dtCore::ResourceDescriptor getCurrentResource(dtCore::DataType& type);
      
      /**
       * Gets the current grid size.
       * @return A power of two integer corresponding to the current grid size.
       * @note The default grid size is 16
       */
      int getGridSize() const { return mGridSize; }

      /**
       * Returns a pointer the the main window.
       * @return The main window.
       */
      MainWindow* getMainWindow() const { return mMainWindow; }

      /**
       * Returns the current library directory for the library editor
       * @return The current library directory
       */
      inline const std::string& getCurrentLibraryDirectory() const { return mCurrentLibraryDirectory; }

      /**
       * Sets the current library directory
       * @param newDir the new directory
       * @note If newDir is empty the directory will not be modified
       * @note The new library path has to be in the system path.
       */
      inline void setCurrentLibraryDirectory(const std::string& newDir)
      {
         if (!newDir.empty())
         {
            mCurrentLibraryDirectory = newDir;
         }
      }

      /**
       * Returns the current project context for the library editor
       * @return The current library directory
       */
      inline const std::string& getCurrentProjectContext() const
      {
         return mCurrentProjectContext;
      }

      /**
       * Sets the current project context
       * @param newDir the new directory
       * @note If newDir is empty the directory will not be modified
       */
      inline void setCurrentProjectContext(const std::string& newDir)
      {
         if (!newDir.empty())
         {
            mCurrentProjectContext = newDir;
         }
      }

      /**
       * Adds a new map name to the list of recent maps
       * @param name The name of the map
       * @note If the name is already in list, the name in the list is deleted
       */
      void addRecentMap(std::string name);

      /**
       * Adds a new project name to the list of recent projects
       * @param name The name of the project
       * @note If the name is already in list, the name in the list is deleted
       */
      void addRecentProject(std::string name);

      /**
       * UndoManager accessor
       */
      inline UndoManager& getUndoManager() { return mUndoManager; }

      /**
       * Recent project accessor
       */
      inline std::list<std::string>& getRecentProjects() { return mRecentProjects; }

      /**
       * Recent map accessor
       */
      inline std::list<std::string>& getRecentMaps() { return mRecentMaps; }

      // Accessor
      inline bool getLoadLastProject() const { return mLoadLastProject; }

      // Mutator
      inline void setLoadLastProject(bool enable) { mLoadLastProject = enable; }

      // Accessor
      inline bool getLoadLastMap() const { return mLoadLastMap; }

      // Mutator
      inline void setLoadLastMap(bool enable) { mLoadLastMap = enable; }

      // Accessor
      inline bool GetUseGlobalOrientationForViewportWidget() const { return mUseGlobalOrientationForViewportWidget; }

      // Mutator
      inline void SetUseGlobalOrientationForViewportWidget(bool enable) { mUseGlobalOrientationForViewportWidget = enable; }

      // Accessor
      inline QColor getSelectionColor() const { return mSelectionColor; }

      // Mutator
      inline void setSelectionColor(const QColor& color) { mSelectionColor = color; }

      // Accessor
      inline bool getRigidCamera() const { return mRigidCamera; }

      // Mutator
      inline void setRigidCamera(bool enable) { mRigidCamera = enable; }

      // Accessor
      inline float GetActorCreationOffset() const { return mActorCreationDistance; }

      // Mutator
      inline void SetActorCreationOffset(const float actorCreationDistance) { mActorCreationDistance = actorCreationDistance; }

      // Accessor
      inline float GetNumRecentProjects() const { return mNumRecentProjects; }

      // Mutator
      inline void SetNumRecentProjects(const float numRecentProjects) { mNumRecentProjects = numRecentProjects; }

      ///@return the ui registry for group actor properties
      GroupUIRegistry& GetGroupUIRegistry();

   protected:
      /**
       * Protected destructor, but does nothing.  Protected because it's a singleton
       */
      virtual ~EditorData();

      /**
       * Sets the Main Window.  This is protected so that only friends can set this.
       */
      void setMainWindow(MainWindow* window);

      friend class MainWindow;


   private:
      /**
       * Constructs the class.   This is private because it's a singleton
       */
      EditorData();
      EditorData(const EditorData& rhs);
      EditorData& operator=(EditorData& rhs);


      // Singleton instance of this class.
      static dtCore::RefPtr<EditorData> sInstance;

      // data
      // NOTE - This should be an dtCore::RefPtr, however QT is managing MainWindow, so we don't have to.
      MainWindow* mMainWindow;
      dtCore::RefPtr<GroupUIRegistry> mGroupUIRegistry;
      dtCore::RefPtr<dtCore::Map> mMap;
      std::string mCurrentLibraryDirectory;
      std::string mCurrentProjectContext;

      std::map<dtCore::DataType*, dtCore::ResourceDescriptor> mCurrentResources;

      std::list<std::string> mRecentMaps;
      std::list<std::string> mRecentProjects;

      UndoManager mUndoManager;

      int mGridSize;
      unsigned int mNumRecentProjects;

      bool mLoadLastProject, mLoadLastMap, mRigidCamera;
      bool mUseGlobalOrientationForViewportWidget; 
      float mActorCreationDistance;
      QColor mSelectionColor;
   };

} // namespace dtEditQt

#endif // DELTA_EDITOR_DATA
