/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author David Guthrie
 */

#ifndef DELTA_PROJECT
#define DELTA_PROJECT

#include <string>
#include <vector>
#include <map>
#include <tree.h>

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <dtCore/export.h>
#include <dtABC/application.h>
#include "dtDAL/resourcedescriptor.h"
#include "dtDAL/resourcetreenode.h"
#include "dtDAL/resourcehelper.h"
#include "dtDAL/fileutils.h"

namespace dtDAL
{

    class Map;
    class MapParser;
    class MapWriter;
    class Log;
    class DataType;
    class LibraryManager;

   /**
    * @class Project
    * @brief Represents the project directory structure.
    * This represents the project directory structure and implements the methods to access and
    * modify a project including the maps and resource content.  This class is a singleton.  Call
    * the getInstance method for the single instance.
    */
  class DT_EXPORT Project : public osg::Referenced
  {
  private:
    static osg::ref_ptr<Project> mInstance; //< the instance of the project.

    static const std::string LOG_NAME;
    static const std::string MAP_DIRECTORY;
    static const std::string MAP_BACKUP_SUB_DIRECTORY;

    Project();

    bool mValidContext;
    std::string mContext;

    bool mContextReadOnly;
    mutable bool mResourcesIndexed;

    std::map<std::string,std::string> mMapList; //< The list of maps by name mapped to the file names.
    mutable std::set<std::string> mMapNames; //< The list of map names.

    std::map<std::string, osg::ref_ptr<Map> > mOpenMaps; //< A vector of the maps currently loaded.
    mutable core::tree<ResourceTreeNode> mResources; //< a tree of all the resources.  This is more of a cache.

    osg::ref_ptr<MapParser> mParser;
    osg::ref_ptr<MapWriter> mWriter;
    //This is here to make sure the library manager is deleted AFTER the maps are closed.
    //so that libraries won't be closed and the proxies deleted out from under the map.
    osg::ref_ptr<LibraryManager> libraryManager;
    ResourceHelper mResourceHelper;

    Log* mLogger;

    //verifies that a directory exists by creating it if it doesn't and updating the tree.
    core::tree<ResourceTreeNode>* VerifyDirectoryExists(const std::string& path, const std::string& category = "",
        core::tree<ResourceTreeNode>* parentTree = NULL);

    //internal handling for saving a map.
    void InternalSaveMap(Map& map);
    //internal handling for deleting a map.
    void InternalDeleteMap(const std::string& mapFileName);

    //internal handling for loading a map.
    Map& InternalLoadMap(const std::string& name,const std::string& fullPath, bool clearModified);

    void UnloadUnusedLibraries(Map& mapToClose);

    //internal method to get the pointer to the subtree for a given datatype.
    core::tree<ResourceTreeNode>& GetResourcesOfType(const DataType& datatype) const;
    //Checks to see if a map is actually one the current project knows about.
    void CheckMapValidity(const Map& map, bool allowReadonly = false) const;
    //re-reads the map names from project.
    void ReloadMapNames() const;
    //indexes all the resources in the project.
    void IndexResources() const;
    //recursive helper method for the other indexResources
    //The category AND the categoryPath are passed so that
    //they won't have to be converted on every recursive call.
    void IndexResources(FileUtils& fileUtils, core::tree<ResourceTreeNode>::iterator& i,
        const DataType& dt, const std::string& categoryPath,const std::string& category) const;

    //Gets the list of backup map files.
    void GetBackupMapFilesList(DirectoryContents& toFill) const;

    //searches the resource tree for a category node and returns
    //an iterator to it or resources.end() if not found.
    core::tree<ResourceTreeNode>::iterator FindTreeNodeFromCategory(
        const DataType* dt, const std::string& category) const;


    const std::string GetBackupDir() const {
        return Project::MAP_DIRECTORY + FileUtils::PATH_SEPARATOR + Project::MAP_BACKUP_SUB_DIRECTORY;
    }
    //Later
/*    core::tree<ResourceTreeNode>* getMatchingBranch(
        core::tree<ResourceTreeNode>::iterator level,
        const DataType& type,
        const std::string& partialName,
        const std::string& extension) const;*/
    protected:
        virtual ~Project();

  public:
    /**
      * @return the single instance of this class.
      */
    static Project& GetInstance()
    {
        if (mInstance == NULL)
        {
            mInstance = new Project();
        }
        return *mInstance;
    }

    /**
      * @return true if the context is valid.
      */
    bool IsContextValid() const { return mValidContext; };

    /**
      * Assigns the context for the project.  If it is not opened read only, the code will attempt to make the path
      * into a project.  If the path does not exist, it will attempt to create and initialize it as well.  The method
      * will throw exceptions if the directory cannot be a valid project for some reason, or if it is not initialized and
      * the programmer passed a trie for opening it read only.
      * @param path the file path of to the project
      * @param openReadOnly optional parameter that defaults to false.  If true, the class will not attempt to modify
      * the project directory.
      * @throws ExceptionEnum::ProjectInvalidContext if the path specified is invalid.
      */
    void SetContext(const std::string& path, bool openReadOnly=false);

    /**
      * @return the current context.  This will be empty if no valid context is set.
      */
    const std::string& GetContext() const { return mContext; };

    /**
      * In the process of opening and using a project, lists of resources, maps, etc, are created
      * This will clear or otherwise reload these cached lists so that any changes made to the project
      * by hand will be picked up.
      */
    void Refresh();

    /**
     * @return a vector with the names of the maps currently in the project.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     */
    const std::set<std::string>& GetMapNames();

    /**
     * returns the map with the given name.
     * @param name the name of the map as specified by the getMapNames() vector.
     * @return the opened map
     * @throws ExceptionEnum::MapLoadParsingError if an error occurs reading the map file.
     * @throws ExceptionEnum::ProjectFileNotFound if the map does not exist.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     */
    Map& GetMap(const std::string& name);

    /**
     * returns the last backup save of the map with the given name.
     * @note if no backup is found, this call will NOT open the saved map, it will throw a file not
     *       found exception.
     * @param name the name of the map as specified by the getMapNames() vector.
     * @return the opened map
     * @throws ExceptionEnum::MapLoadParsingError if an error occurs reading the map file.
     * @throws ExceptionEnum::ProjectFileNotFound if a backup does not exist.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     */
    Map& OpenMapBackup(const std::string& name);

    /**
     * Loads a map by name into an application.  If the map is already opened, the currently
     * loaded map will be reused.
     * @param name The name of the map to load.
     * @param scene The application to add the proxy objects to.
     * @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
     * @return the map that was loaded into the scene.
     * @throws ExceptionEnum::MapLoadParsingError if an error occurs reading the map file.
     * @throws ExceptionEnum::ProjectFileNotFound if the map does not exist.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     */
    Map& LoadMapIntoScene(const std::string& name, dtCore::Scene& scene, bool addBillBoards = false)
    {
        Map& m = GetMap(name);
        LoadMapIntoScene(m, scene, addBillBoards);
        return m;
    }

    /**
     * Loads a map into a scene.
     * @param map The map to load into the scene
     * @param scene the scene to load the map into
     * @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     */
    void LoadMapIntoScene(Map& map, dtCore::Scene& scene, bool addBillBoards = false);

    /**
     * Creates a new map and saves it.
     * @param name The name of the map.
     * @param fileName the name of the map file.
     * @return The new map.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
     * @throws ExceptionEnum::ProjectException if a map by that name or fileName already exists.
     * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
     */
    Map& CreateMap(const std::string& name, const std::string& fileName);


    /**
     * Closes the map passed in.  The map will likely be freeded when this method exits.
     * @note if you tell this call to unloadLibraries, be certain you have cleared all of the proxies in this map
     *       from your scene and deleted any memory that may may have been allocated by the map or you can
     *       end up in a bad state.
     * @param map the map the close.
     * @param unloadLibraries unload all libraries not used by other open maps. This is dangerous.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     */
    void CloseMap(Map& map, bool unloadLibraries = false);

    /**
     * Deletes the given map.
     * @note if you tell this call to unloadLibraries, be certain you have cleared all of the proxies in this map
     *       from your scene and deleted any memory that may may have been allocated by the map or you can
     *       end up in a bad state.
     * @param map the map the delete.
     * @param unloadLibraries unload all libraries not used by other open maps. This is dangerous.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
     */
    void DeleteMap(Map& map, bool unloadLibraries = false);

    /**
     * Deletes the given map.
     * @note if you tell this call to unloadLibraries, be certain you have cleared all of the proxies in this map
     *       from your scene and deleted any memory that may may have been allocated by the map or you can
     *       end up in a bad state.
     * @param mapName the name of the map to delete.
     * @param unloadLibraries unload all libraries not used by other open maps. This is dangerous.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
     */
    void DeleteMap(const std::string& mapName, bool unloadLibraries = false);

    /**
      * Saves the given map whether it has changed or not.
      * @param map the map to save.
      * @throws ExceptionEnum::ProjectException if you change the name to match another map.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set or the Map is not part of the project.
      * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
      * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
      */
    void SaveMap(Map& map);

    /**
      * Saves the given map to a new file name and alters the open map object to point to the new file.
      * There will then be no open copy of the old map file name.
      * @param map the map to save.
      * @param newName the new name of the map to save.
      * @param newFileName the new file name of the map to save.
      * @throws ExceptionEnum::ProjectException if a map by that name or fileName already exists.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set or the Map is not part of the project.
      * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
      * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
      */
    void SaveMapAs(Map& map, const std::string& newName, const std::string& newFileName);

    /**
      * Saves the given map to a new file name and alters the open map object to point to the new file.
      * There will then be no open copy of the old map file name.
      * Keep in mind that if the map name given is not loaded, this call will actually open the map before saving it.
      * @param map the current map name.
      * @param newName the new name of the map to save.
      * @param newFileName the new file name of the map to save.
      * @throws ExceptionEnum::ProjectException if a map by that name or fileName already exists.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set or the Map is not part of the project.
      * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
      * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
      */
    void SaveMapAs(const std::string& mapName, const std::string& newName, const std::string& newFileName);

    /**
     * Saves the given map whether it has changed or not.  If the map has not been loaded, but exists, this
     * call is a NOOP.
     * @param map the map to save.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
     * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
     */
    void SaveMap(const std::string& mapName);

    /**
     * Saves a new backup of the map.
     * @param map the map to save a backup of.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
     * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
     */
    void SaveMapBackup(Map& map);

    /**
     * @param map the map to get the backups count for.
     * @return the number of backup files this map has currently
     */
    bool HasBackup(Map& map) const;

    /**
     * @param mapName the name map to get the backups count for.
     * @return the number of backup files this map has currently
     */
    bool HasBackup(const std::string& mapName) const;

    /**
     * Clears the backup save for a give map.
     * @param map the map to clear the backups for.
     */
    void ClearBackup(Map& map);

    /**
     * Clears the backup save for a given map.
     * @param mapName the name of the map to clear the backups for.
     */
    void ClearBackup(const std::string& mapName);

    /**
     * @param resourceType the type of resource.  The datatype passed in MUST be a resource type.
     * @param toFill the vector to fill with the handlers.
     * @return The resource types that are available for the given handler.
     * @throws ExceptionEnum::ProjectResourceError if the datatype is a primitive type, not a resource type.
     */
    void GetHandlersForDataType(const DataType& resourceType, std::vector<osg::ref_ptr<const ResourceTypeHandler> >& toFill) const;

    /**
     * Registers a new type handler.  The method will get the datatype and applicable filters from
     * the handler itself.
     * @param handler The resource type handler to register.
     * @throws ExceptionEnum::ProjectResourceError if the datatype of the handler is a primitive type, not a resource type.
     */
    void RegisterResourceTypeHander(ResourceTypeHandler& handler);

    /**
      * This will take the resource descriptor object and return a path to the resource within the project.
      * The first time this is called, an index of the resources may need to be created internally to validate
      * the string, so it may take some time to return.
      * @param resource The  representation of a resource.
      * @return The path to load a resource.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
      * @throws ExceptionEnum::ProjectResourceError if the string representation is invalid.
      * @throws ExceptionEnum::ProjectFileNotFound if the file was not found.
      */
    const std::string GetResourcePath(const ResourceDescriptor& resource) const;

    /**
      * Adds a resource to the project by copying it into the project.
      * @param newName the new name of the resource.
      * @param pathToFile The path to the file to copy into the project.
      * @param category The category of the resource file as a dot-notation string.
      * @param type the resounce datatype of the resource.  This must be one of the enums that define a resource.
      * @return the resource's unique identifier string.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
      * @throws ExceptionEnum::ProjectIOException if the could not complete because of some sort of IO exception.
      * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
      */
    const ResourceDescriptor AddResource(const std::string& newName, const std::string& pathToFile, const std::string& category, const DataType& type);

    /**
      * Removes a resource from a project.  This will delete the file if it exists.  If no such resource exists, the
      * call will be a noop.  This call is not guaranteed to check all actors to make sure the resource is not in use.
      * @param resource The resource descriptor object.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
      * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
      * @throws ExceptionEnum::ProjectIOException if the could not complete because of some sort of IO exception.
      * @throws ExceptionEnum::ProjectResourceError if the resource could not be removed for reasons other than file io.
      */
    void RemoveResource(const ResourceDescriptor& resource);

    /**
     * Creates a new resource category.
     * @note if the category already exists, this will be a no-op.
     * @param category the category to create.
     * @param type the data type to add the category into.
     */
    void CreateResourceCategory(const std::string& category, const DataType& type);

    /**
     * Removes a category.
     * @param category The category to remove.
     * @param type The datatype the category is in.
     * @param recursive true to recursively delete the category and all subcategories and resources.
     * @return true if the category was removed, or didn't exist.  false if the category could not be removed because
     *         recursive was false and the category was not empty.
     * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
     * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
     * @throws ExceptionEnum::ProjectIOException if the could not complete because of some sort of IO exception.
     * @throws ExceptionEnum::ProjectResourceError if the resource could not be removed for reasons other than file io.
     * @throws ExceptionEnum::ProjectFileNotFound on rare occasion, this could possibly be thrown if the file contents
     *              are changed while the recusive delete is occuring.
     */
    bool RemoveResourceCategory(const std::string& category, const DataType& type, bool recursive);

    /**
     * @note This tree will likely be cached.  See the Project::refresh() method.
     * @see Project#refresh
     * @return all of the resources known to the project.
     */
    const core::tree<ResourceTreeNode>& getAllResources() const
    {
        if (!mResourcesIndexed)
            IndexResources();
        return mResources;
    }

    /**
     * @note The data will likely be cached.  See the Project::refresh() method.
     * @see Project#refresh
     * @param toFill The tree to fill with the results.
     * @return all of the resources known to the project of the given type.
     */
    void GetResourcesOfType(const DataType& type, core::tree<ResourceTreeNode>& toFill) const;

    //Add later
//     void findResources(core::tree<ResourceTreeNode> toFill,
//         const DataType& type,
//         const std::string& partialName,
//         const std::string& extension) const;

    //void archiveProject(const std::string& targetPath);
    //void unarchiveProject(const std::string& targetPath);

    /**
      * @return true if the current project context is stored in an archive.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
      */
    bool IsArchive() const;

    /**
      * @return true if the current project is opened readonly.
      * @throws ExceptionEnum::ProjectInvalidContext if the context is not set.
      */
    bool IsReadOnly() const;


  };

}

#endif
