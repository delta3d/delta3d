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

#include <QtGui/QColor>
#include <osg/Referenced>
#include <vector>
#include <string>
#include <list>
#include <dtEditQt/undomanager.h>
#include <dtDAL/resourcedescriptor.h>

namespace dtDAL
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
   class EditorData : public osg::Referenced
   {
   public:
      /**
       * Gets the singleton instance of the this class
       *
       * @return Returns the singleton instance
       */
      static EditorData& GetInstance();

      /**
       * Gets the environment string associated with OSG_LIBRARY_PATH.
       *
       * @return Returns the user-defined custom path that OSG uses in addition to its main search path for libraries.
       */
      std::string getOriginalOsgLibraryPath() { return osgLibraryPath; }

      /**
       * Gets the environment string associated with OSG_LIBRARY_PATH.
       *
       * @param Sets the user-defined custom path that OSG uses in addition to its main search path for libraries.
       */
      void setOriginalOsgLibraryPath(const std::string& path);

      /**
       * Sets the Main Window.
       *
       * @param Takes the current map. Should be gotten from opening a map or creating a new map
       */
      void setCurrentMap(dtDAL::Map* map);

      /**
       * Gets the Current Map.
       *
       * @return Returns the current map that was set when opening or creating a new map
       */
      dtDAL::Map* getCurrentMap();

      /**
       * Fills a vector with pointers to all the currently selected actor proxies.
       */
      void GetSelectedActors(std::vector<dtDAL::ActorProxy*>& toFill);

      /**
       * Sets the currently selected Sound resource.  This is called when the user is
       * using the resource browser and is selecting resources.  It is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
       * with empty strings.
       * @see setMeshResource, setTextureResource, setParticleResource
       */
      void setCurrentSoundResource(const dtDAL::ResourceDescriptor selectedResource);

      /**
       * Sets the currently selected Mesh resource.  This is called when the user is
       * using the resource browser and is selecting resources.  It is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
       * with empty strings.
       * @see setSoundResource, setTextureResource, setParticleResource
       */
      void setCurrentMeshResource(const dtDAL::ResourceDescriptor selectedResource);

      /**
       * Sets the currently selected Texture resource.  This is called when the user is
       * using the resource browser and is selecting resources.  It is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
       * with empty strings.
       * @see setSoundResource, setMeshResource, setParticleResource, setTerrainResource
       */
      void setCurrentTextureResource(const dtDAL::ResourceDescriptor selectedResource);

      /**
       * Sets the currently selected Particle resource.  This is called when the user is
       * using the resource browser and is selecting resources.  It is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
       * with empty strings.
       * @see setSoundResource, setMeshResource, setTextureResource, setTerrainResource
       */
      void setCurrentParticleResource(const dtDAL::ResourceDescriptor selectedResource);

      /**
       * Sets the currently selected Skeletal Model resource.  This is called when the user is
       * using the resource browser and is selecting resources.  It is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
       * with empty strings.
       * @see setSoundResource, setMeshResource, setTextureResource, setTerrainResource
       */
      void setCurrentSkeletalModelResource(const dtDAL::ResourceDescriptor selectedResource);

      /**
       * Sets the currently selected terrain resource.  This is called when the user is
       * using the resource browser and is selecting resources.  It is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
       * with empty strings.
       * @see setSoundResource, setMeshResource, setTextureResource
       */
      void setCurrentTerrainResource(const dtDAL::ResourceDescriptor selectedResource);

      /**
       * Gets the current grid size.
       * @return A power of two integer corresponding to the current grid size.
       * @note The default grid size is 16
       */
      int getGridSize() const { return gridSize; }

      /**
       * Gets the currently selected Sound resource.  This is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @return The currently selected resource descriptor.  Check
       * resource.getResourceIdentifier().empty() to see if it's actually none.
       * @see setSoundResource, setMeshResource, setTextureResource
       */
      dtDAL::ResourceDescriptor getCurrentSoundResource() { return soundResource; }

      /**
       * Gets the currently selected Mesh resource.  This is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @return The currently selected resource descriptor.  Check
       * resource.getResourceIdentifier().empty() to see if it's actually none.
       * @see setSoundResource, setMeshResource, setTextureResource
       */
      dtDAL::ResourceDescriptor getCurrentMeshResource() { return this->meshResource; }

      /**
       * Gets the currently selected Texture resource.  This is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @return The currently selected resource descriptor.  Check
       * resource.getResourceIdentifier().empty() to see if it's actually none.
       * @see setSoundResource, setMeshResource, setTextureResource
       */
      dtDAL::ResourceDescriptor getCurrentTextureResource() { return this->textureResource; }

      /**
       * Gets the currently selected Particle resource.  This is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @return The currently selected resource descriptor.  Check
       * resource.getResourceIdentifier().empty() to see if it's actually none.
       * @see setSoundResource, setMeshResource, setTextureResource
       */
      dtDAL::ResourceDescriptor getCurrentParticleResource() { return this->particleResource; }


      /**
       * Gets the currently selected skeletal model resource.  This is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @return The currently selected resource descriptor.  Check
       * resource.getResourceIdentifier().empty() to see if it's actually none.
       * @see setSoundResource, setMeshResource, setTextureResource
       */
      dtDAL::ResourceDescriptor getCurrentSkeletalModelResource() { return this->skeletalModelResource; }

      /**
       * Gets the currently selected terrain resource.  This is used by the
       * property editor when the user selects 'Use Current'.
       *
       * @return The currently selected resource descriptor.  Check
       * resource.getResourceIdentifier().empty() to see if it's actually none.
       * @see getSoundResource, getMeshResource, getTextureResource, getTerrainResource
       */
      dtDAL::ResourceDescriptor getCurrentTerrainResource() { return this->terrainResource; }

      /**
       * Returns a pointer the the main window.
       * @return The main window.
       */
      MainWindow* getMainWindow() const { return mainWindow; }

      /**
       * Returns the current library directory for the library editor
       * @return The current library directory
       */
      inline const std::string& getCurrentLibraryDirectory() const { return currentLibraryDirectory; }

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
            currentLibraryDirectory = newDir;
         }
      }

      /**
       * Returns the current project context for the library editor
       * @return The current library directory
       */
      inline const std::string& getCurrentProjectContext() const
      {
         return currentProjectContext;
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
            currentProjectContext = newDir;
         }
      }

      /**
       * Adds a new map name to the list of recent maps
       * @param name The name of the map
       * @note If the name is already in list, the name in the list is deleted
       */
      void addRecentMap(const std::string& name);

      /**
       * Adds a new project name to the list of recent projects
       * @param name The name of the project
       * @note If the name is already in list, the name in the list is deleted
       */
      void addRecentProject(const std::string& name);

      /**
       * UndoManager accessor
       */
      inline UndoManager& getUndoManager()
      {
         return undoManager;
      }

      /**
       * Recent project accessor
       */
      inline std::list<std::string>& getRecentProjects() { return recentProjects; }

      /**
       * Recent map accessor
       */
      inline std::list<std::string>& getRecentMaps() { return recentMaps; }

      // Accessor
      inline bool getLoadLastProject() const { return loadLastProject; }

      // Mutator
      inline void setLoadLastProject(bool enable) { loadLastProject = enable; }

      // Accessor
      inline bool getLoadLastMap() const { return loadLastMap; }

      // Mutator
      inline void setLoadLastMap(bool enable) { loadLastMap = enable; }

      // Accessor
      inline bool GetUseGlobalOrientationForViewportWidget() const { return mUseGlobalOrientationForViewportWidget; }

      // Mutator
      inline void SetUseGlobalOrientationForViewportWidget(bool enable) { mUseGlobalOrientationForViewportWidget = enable; }

      // Accessor
      inline QColor getSelectionColor() const { return selectionColor; }

      // Mutator
      inline void setSelectionColor(const QColor& color) { selectionColor = color; }

      // Accessor
      inline bool getRigidCamera() const { return rigidCamera; }

      // Mutator
      inline void setRigidCamera(bool enable) { rigidCamera = enable; }

      // Accessor
      inline float GetActorCreationOffset() const { return mActorCreationDistance; }

      // Mutator
      inline void SetActorCreationOffset(const float actorCreationDistance) { mActorCreationDistance = actorCreationDistance; }

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
      static dtCore::RefPtr<EditorData> instance;

      // data
      // NOTE - This should be an dtCore::RefPtr, however QT is managing MainWindow, so we don't have to.
      MainWindow* mainWindow;
      dtCore::RefPtr<GroupUIRegistry> mGroupUIRegistry;
      dtCore::RefPtr<dtDAL::Map> myMap;
      std::string currentLibraryDirectory;
      std::string currentProjectContext;
      std::string osgLibraryPath;

      dtDAL::ResourceDescriptor soundResource;
      dtDAL::ResourceDescriptor meshResource;
      dtDAL::ResourceDescriptor textureResource;
      dtDAL::ResourceDescriptor particleResource;
      dtDAL::ResourceDescriptor terrainResource;
      dtDAL::ResourceDescriptor skeletalModelResource;

      std::list<std::string> recentMaps;
      std::list<std::string> recentProjects;

      UndoManager undoManager;

      int gridSize;
      unsigned int numRecentProjects;

      bool loadLastProject, loadLastMap, rigidCamera;
      bool mUseGlobalOrientationForViewportWidget; 
      float mActorCreationDistance;
      QColor selectionColor;
   };

} // namespace dtEditQt

#endif // DELTA_EDITOR_DATA
