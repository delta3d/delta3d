/*
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
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
* @author Curtiss Murphy
*/
#ifndef DELTA_EDITOR_DATA
#define DELTA_EDITOR_DATA

#include <QtGui/QColor>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <vector>
#include <string>
#include <list>
#include "dtEditQt/undomanager.h"

#include <dtDAL/project.h>

namespace dtDAL
{
    class Map;
    class Project;
    class DataTypes;
}

namespace dtEditQt
{
    class MainWindow;
    class UndoManager;

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
        static EditorData &getInstance();

        /**
        * Sets the Main Window.
        *
        * @param Takes the current map. Should be gotten from opening a map or creating a new map
        */
        void setCurrentMap(dtDAL::Map *map);

        /**
         * Gets the Current Map.
         *
         * @return Returns the current map that was set when opening or creating a new map
         */
        osg::ref_ptr<dtDAL::Map> getCurrentMap();


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
         * @see setSoundResource, setMeshResource, setParticleResource, setTerrainResource, setCharacterResource
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
         * Sets the currently selected character resource.  This is called when the user is
         * using the resource browser and is selecting resources.  It is used by the
         * property editor when the user selects 'Use Current'.
         *
         * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
         * with empty strings.
         * @see setSoundResource, setMeshResource, setTextureResource, setTerrainResource
         */
        void setCurrentCharacterResource(const dtDAL::ResourceDescriptor selectedResource);

        /**
         * Sets the currently selected terrain resource.  This is called when the user is
         * using the resource browser and is selecting resources.  It is used by the
         * property editor when the user selects 'Use Current'.
         *
         * @param The currently selected resource.  If none, then pass in ResourceDescriptor()
         * with empty strings.
         * @see setSoundResource, setMeshResource, setTextureResource, setCharacterResource
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
        dtDAL::ResourceDescriptor getCurrentMeshResource(){return this->meshResource;}

        /**
         * Gets the currently selected Texture resource.  This is used by the
         * property editor when the user selects 'Use Current'.
         *
         * @return The currently selected resource descriptor.  Check
         * resource.getResourceIdentifier().empty() to see if it's actually none.
         * @see setSoundResource, setMeshResource, setTextureResource
         */
        dtDAL::ResourceDescriptor getCurrentTextureResource(){return this->textureResource;}

        /**
         * Gets the currently selected Particle resource.  This is used by the
         * property editor when the user selects 'Use Current'.
         *
         * @return The currently selected resource descriptor.  Check
         * resource.getResourceIdentifier().empty() to see if it's actually none.
         * @see setSoundResource, setMeshResource, setTextureResource
         */
        dtDAL::ResourceDescriptor getCurrentParticleResource(){return this->particleResource;}

        /**
         * Gets the currently selected Character resource.  This is used by the
         * property editor when the user selects 'Use Current'.
         *
         * @return The currently selected resource descriptor.  Check
         * resource.getResourceIdentifier().empty() to see if it's actually none.
         * @see setSoundResource, setMeshResource, setTextureResource
         */
        dtDAL::ResourceDescriptor getCurrentCharacterResource(){return this->characterResource;}

        /**
         * Gets the currently selected Character resource.  This is used by the
         * property editor when the user selects 'Use Current'.
         *
         * @return The currently selected resource descriptor.  Check
         * resource.getResourceIdentifier().empty() to see if it's actually none.
         * @see getSoundResource, getMeshResource, getTextureResource, getTerrainResource
         */
        dtDAL::ResourceDescriptor getCurrentTerrainResource(){return this->terrainResource;}

        /**
         * Returns a pointer the the main window.
         * @return The main window.
         */
        MainWindow *getMainWindow() const { return mainWindow; }

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
        inline void setCurrentLibraryDirectory(const std::string &newDir)
        {
            if(!newDir.empty())
                currentLibraryDirectory = newDir;
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
        inline void setCurrentProjectContext(const std::string &newDir)
        {
            if(!newDir.empty())
                currentProjectContext = newDir;
        }

        /**
         * Adds a new map name to the list of recent maps
         * @param name The name of the map
         * @note If the name is already in list, the name in the list is deleted
         */
        void addRecentMap(const std::string &name)
        {
            for(std::list<std::string>::iterator i = recentMaps.begin();
                i != recentMaps.end(); ++i)
            {
                if((*i) == name)
                {
                    recentMaps.erase(i);
                    break;
                }
            }

            if(recentMaps.size() < 4)
                recentMaps.push_front(name);
            else
            {
                recentMaps.pop_back();
                recentMaps.push_front(name);
            }
        }

        /**
         * Adds a new project name to the list of recent projects
         * @param name The name of the project
         * @note If the name is already in list, the name in the list is deleted
         */
        void addRecentProject(const std::string &name)
        {
            for(std::list<std::string>::iterator i = recentProjects.begin();
                i != recentProjects.end(); ++i)
            {
                if((*i) == name)
                {
                    recentProjects.erase(i);
                    break;
                }
            }

            if(recentProjects.size() < 4)
                recentProjects.push_front(name);
            else
            {
                recentProjects.pop_back();
                recentProjects.push_front(name);
            }
        }

        /**
         * UndoManager accessor
         */
        inline UndoManager &getUndoManager()
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
        inline QColor getSelectionColor() const { return selectionColor; }

        // Mutator
        inline void setSelectionColor(const QColor &color) { selectionColor = color; }

        // Accessor
        inline bool getRigidCamera() const { return rigidCamera; }

        // Mutator
        inline void setRigidCamera(bool enable) { rigidCamera = enable; }

    protected:
        /**
        * Protected destructor, but does nothing.  Protected because it's a singleton
        */
        virtual ~EditorData();

        /**
        * Sets the Main Window.  This is protected so that only friends can set this.
        */
        void setMainWindow(MainWindow *window);

        friend class MainWindow;


    private:
        /**
        * Constructs the class.   This is private because it's a singleton
        */
        EditorData();
        EditorData(const EditorData &rhs);
        EditorData &operator=(EditorData &rhs);

        //Singleton instance of this class.
        static osg::ref_ptr<EditorData> instance;

        // data
        // NOTE - This should be an osg::ref_ptr, however QT is managing MainWindow, so we don't have to.
        MainWindow *mainWindow;
        osg::ref_ptr<dtDAL::Map> myMap;
        std::string currentLibraryDirectory;
        std::string currentProjectContext;

        dtDAL::ResourceDescriptor soundResource;
        dtDAL::ResourceDescriptor meshResource;
        dtDAL::ResourceDescriptor textureResource;
        dtDAL::ResourceDescriptor particleResource;
        dtDAL::ResourceDescriptor characterResource;
        dtDAL::ResourceDescriptor terrainResource;

        std::list<std::string> recentMaps;
        std::list<std::string> recentProjects;

        UndoManager undoManager;

        int gridSize;
        unsigned int numRecentProjects;

        bool loadLastProject, loadLastMap, rigidCamera;
        QColor selectionColor;
    };
}

#endif
