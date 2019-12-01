/* -*-c++-*-
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
 * David Guthrie
 */

#ifndef DELTA_PROJECT
#define DELTA_PROJECT

#include <string>
#include <vector>
#include <map>
#include <set>

#include <osg/Referenced>

#include <dtCore/uniqueid.h>
#include <dtUtil/utiltree.h>
#include <dtCore/resourcetreenode.h>
#include <dtCore/prefabactortype.h>
#include <dtCore/resourcehelper.h>
#include <dtCore/export.h>

namespace dtUtil
{
   class Log;
   class FileUtils;
}

namespace dtCore
{
   class Scene;
}

namespace dtCore
{

   class Map;
   typedef dtCore::RefPtr<Map> MapPtr;
   class MapParser;
   class MapWriter;
   class DataType;
   class ActorFactory;
   class BaseActorObject;
   class ResourceDescriptor;
   class ProjectImpl;
   class GameEvent;
   class ProjectConfig;

   typedef std::vector<RefPtr<BaseActorObject> > ActorRefPtrVector;


   /**
    * @class Project
    * @brief Represents the project directory structure.
    * This represents the project directory structure and implements the methods to access and
    * modify a project including the maps and resource content.  This class is a singleton.  Call
    * the getInstance method for the single instance.
    */
   class DT_CORE_EXPORT Project : public osg::Referenced
   {
   public:
      typedef dtUtil::tree<ResourceTreeNode> ResourceTree;
      typedef unsigned ContextSlot;
      static const ContextSlot DEFAULT_SLOT_VALUE = ~0U;

      struct MapTreeData
      {
         void clear()
         {
            categoryName.clear();
            mapList.clear();
            subCategories.clear();
         }

         std::string                   categoryName;
         mutable std::set<std::string> mapList;
         std::vector<MapTreeData>      subCategories;
      };

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
       * Sets up the context paths and the project in general based on the given project config.
       * @note If adding any of the context paths in the config fail in AddContext, all of the context paths will be cleared
       * @throw dtUtil::Exception if setting up the project fails.
       * @throw dtCore::ProjectInvalidContextException if any of the paths specified are invalid.
       */
      void SetupFromProjectConfig(const ProjectConfig& config);

      /**
       * @see SetupFromProjectConfig
       * Works the same as SetupFromProjectConfig, but it loads it from file.  It also
       * returns the ProjecConfig object for convenience.
       */
      dtCore::RefPtr<ProjectConfig> SetupFromProjectConfigFile(const std::string& path);

      /**
       * simply reads the project config from a file and returns the object.  This path can
       * be inside an archive that osg supports.
       * @throw XMLLoadParsingException if it can't load and parse the file.
       */
      dtCore::RefPtr<ProjectConfig> LoadProjectConfigFile(const std::string& path);

      /**
       * Saves the project object to a project config file.
       * @throw ProjectConfigSaveException if it is unable to save the file.
       */
      void SaveProjectConfigFile(ProjectConfig& projectConfig, const std::string& path);

      /**
       * Creates a new project context if it doesn't exist.
       * @param path the full path to the new context directory
       * @param createMapsDir create the maps directory.  Otherwise the code just validates that the passed in path
       *                      could be a valid project context.
       * @throw dtCore::ProjectInvalidContextException if the path specified is invalid.
       */
      void CreateContext(const std::string& path, bool createMapsDir = true);

      /**
       * @param slot if this is set, it will return true if the number passed represents
       *             a valid context directory.  if this is not passed, it will return true if
       *             there are any valid context directories.
       * @return true if the context is valid.
       */
      bool IsContextValid(ContextSlot slot = DEFAULT_SLOT_VALUE) const;

      /**
       * Assigns the context for the project.  If it is not opened read only, the code will attempt to make the path
       * into a project.  It no longer creates a context if it does not exist.
       * @see #CreateContext
       * @param path the file path of to the project
       * @param openReadOnly optional parameter that defaults to false.  If true, the class will not attempt to modify
       * the project directory.  This parameter essentially now will override the project global setting.
       * @throw ProjectInvalidContextException if the path specified is invalid.
       */
      void SetContext(const std::string& path, bool openReadOnly = false);

      /**
       * Adds an additional context directory to the project.  If it is not opened read only, the code will att
       * into a project.  It no longer creates a context if it does not exist.
       * @param path the file path of to the project
       * @return which slot the new path gets added to.
       */
      ContextSlot AddContext(const std::string& path);

      /**
       * Removes a context from the list based on the slot number given.
       * @throw ProjectInvalidContextException if the id specified is invalid.
       */
      void RemoveContext(ContextSlot slot);

      void ClearAllContexts();

      /**
       * @return the current context.  This will be empty if no valid context is set.
       */
      const std::string& GetContext(ContextSlot slot = DEFAULT_SLOT_VALUE) const;

      /**
       * @return the number of context slots currently in the project.
       */
      size_t GetContextSlotCount() const;

      /**
       * @return The context slot that is or contains this path.  This will be DEFAULT_SLOT_VALUE if none match.
       * @throw dtUtil::FileNotFoundException
       */
      ContextSlot GetContextSlotForPath(const std::string& path) const;

      /**
       * In the process of opening and using a project, lists of resources, maps, etc, are created
       * This will clear or otherwise reload these cached lists so that any changes made to the project
       * by hand will be picked up.
       */
      void Refresh();

      /**
       * Attempts to parse a file as a map.
       * @param mapFileName the name of the map file to test
       * @return true if the map parser was able to parse the file as a map.
       */
      bool IsValidMapFile(const std::string& mapFileName);

      /**
       * @return a vector with the names of the maps currently in the project.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      const std::set<std::string>& GetMapNames();

      const MapTreeData& GetMapTree();

      /**
       * Parses and returns a map pointer.  This map will only have the header data populated.
       * This will actually read the header from disk, don't let the "Get" prefix on the name confuse you, it will, however,
       * not read the entire file.
       * @return A map with the header data filled out.
       * @throws ProjectInvalidContextException if there is no context.
       * @throws MapParsingException if the map fails to load.
       * @throws FileNotFoundException if the map does not exist.
       */
      MapPtr GetMapHeader(const std::string& mapName);

      /**
       * Parses and returns a map pointer with prefab data.  This map will only have the header data populated.
       * This will actually read the header from disk, don't let the "Get" prefix on the name confuse you, it will, however,
       * not read the entire file.
       * @return A map with the header data filled out.
       * @param prefabResource a resource descriptor that should point to a prefab.
       * @throws ProjectInvalidContextException if there is no context.
       * @throws MapParsingException if the map fails to load.
       * @throws FileNotFoundException if the map does not exist.
       */
      MapPtr GetPrefabHeader(const dtCore::ResourceDescriptor& prefabResource);

      /**
       * returns the map with the given name.
       * @param name the name of the map as specified by the getMapNames() vector.
       * @return the opened map
       * @throws MapParsingException if an error occurs reading the map file.
       * @throws FileNotFoundException if the map does not exist.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      Map& GetMap(const std::string& name);

      /**
       * Checks to see if the named map is loaded in memory.
       * @param name the name of the map to check.
       */
      bool IsMapOpen(const std::string& name);

      std::vector<Map*> GetOpenMaps();

      /**
       * Loads a prefab
       * @param rd Resource pointing to the prefab
       * @return a pointer to the map that loaded with the prefab information.
       * @throws MapParsingException if an error occurs reading the prefab
       * @throws FileNotFoundException if the prefab doesn't exist.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      MapPtr LoadPrefab(const dtCore::ResourceDescriptor& rd, dtCore::ActorRefPtrVector& actorsOut);

      /// This does not yes work.
      dtCore::RefPtr<BaseActorObject> LoadPrefab(const dtCore::PrefabActorType& actorType);

      ResourceDescriptor SavePrefab(const std::string& name, const std::string& category, const ActorRefPtrVector& actorList,
            const std::string& description,
            const std::string& iconFile = std::string(), ContextSlot slot = DEFAULT_SLOT_VALUE);

      /**
       * returns the last backup save of the map with the given name.
       * @note if no backup is found, this call will NOT open the saved map, it will throw a file not
       *       found exception.
       * @param name the name of the map as specified by the getMapNames() vector.
       * @return the opened map
       * @throws MapParsingException if an error occurs reading the map file.
       * @throws FileNotFoundException if a backup does not exist.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      Map& OpenMapBackup(const std::string& name);


      /**
       * Loads a map by name into an application.  If the map is already opened, the currently
       * loaded map will be reused.
       * @param name The name of the map to load.
       * @param scene The application to add the proxy objects to.
       * @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
       * @return the map that was loaded into the scene.
       * @throws MapParsingException if an error occurs reading the map file.
       * @throws FileNotFoundException if the map does not exist.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      Map& LoadMapIntoScene(const std::string& name, dtCore::Scene& scene, bool addBillBoards = false);


      /**
       * Loads a map into a scene.
       * @param map The map to load into the scene
       * @param scene the scene to load the map into
       * @param addBillBoards pass true to add the billboards for any proxies that have the drawmode set to add the billboards.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      void LoadMapIntoScene(Map& map, dtCore::Scene& scene, bool addBillBoards = false);


      /**
       * Creates a new map and saves it.
       * @param name The name of the map.
       * @param fileName the name of the map file.
       * @param slot the id of the context slot to store the map in.  The default is the first slot.
       * @return The new map.
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
       * @throws ExceptionEnum::ProjectException if a map by that name or fileName already exists.
       * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
       */
      Map& CreateMap(const std::string& name, const std::string& fileName, ContextSlot slot = DEFAULT_SLOT_VALUE);


      /**
       * Closes the map passed in.  The map will likely be freed when this method exits.
       * @note if you tell this call to unloadLibraries, be certain you have cleared all of the proxies in this map
       *       from your scene and deleted any memory that may have been allocated by the map or you can
       *       end up in a bad state.
       * @param map the map the close.
       * @param unloadLibraries unload all libraries not used by other open maps. This is dangerous.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      void CloseMap(Map& map, bool unloadLibraries = false);

      /**
       * Closes all open maps
       * @note if you tell this call to unloadLibraries, be certain you have cleared all of the proxies in these maps
       *       from your scene and deleted any memory that may have been allocated by the maps or you can
       *       end up in a bad state.
       * @param unloadLibraries unload all libraries not used by other open maps. This is dangerous.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      void CloseAllMaps(bool unloadLibraries = false);

      /**
       * Deletes the given map.
       * @note if you tell this call to unloadLibraries, be certain you have cleared all of the proxies in this map
       *       from your scene and deleted any memory that may may have been allocated by the map or you can
       *       end up in a bad state.
       * @param map the map the delete.
       * @param unloadLibraries unload all libraries not used by other open maps. This is dangerous.
       * @throws ProjectInvalidContextException if the context is not set.
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
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
       */
      void DeleteMap(const std::string& mapName, bool unloadLibraries = false);

      /**
       * Saves the given map whether it has changed or not.
       * @param map the map to save.
       * @throws ExceptionEnum::ProjectException if you change the name to match another map.
       * @throws ProjectInvalidContextException if the context is not set or the Map is not part of the project.
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
       * @param slot the id of the context slot to store the map in.  The default is the same as the map being re-saved
       * @throws ExceptionEnum::ProjectException if a map by that name or fileName already exists.
       * @throws ProjectInvalidContextException if the context is not set or the Map is not part of the project.
       * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
       * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
       */
      void SaveMapAs(Map& map, const std::string& newName, const std::string& newFileName, ContextSlot slot = DEFAULT_SLOT_VALUE);

      /**
       * Saves the given map to a new file name and alters the open map object to point to the new file.
       * There will then be no open copy of the old map file name.
       * Keep in mind that if the map name given is not loaded, this call will actually open the map before saving it.
       * @param map the current map name.
       * @param newName the new name of the map to save.
       * @param newFileName the new file name of the map to save.
       * @param slot the id of the context slot to store the map in.  The default is the same as the map being re-saved
       * @throws ExceptionEnum::ProjectException if a map by that name or fileName already exists.
       * @throws ProjectInvalidContextException if the context is not set or the Map is not part of the project.
       * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
       * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
       */
      void SaveMapAs(const std::string& mapName, const std::string& newName, const std::string& newFileName, ContextSlot slot = DEFAULT_SLOT_VALUE);

      /**
       * Saves the given map whether it has changed or not.  If the map has not been loaded, but exists, this
       * call is a NOOP.
       * @param map the map to save.
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
       * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
       */
      void SaveMap(const std::string& mapName);

      /**
       * Saves a new backup of the map.
       * @param map the map to save a backup of.
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectReadOnly if the context is read only.
       * @throws ExceptionEnum::MapSaveError if the new map could not be saved.
       */
      void SaveMapBackup(Map& map);

      /**
       * @param map the map to get the backups count for.
       * @return true if the Map has a backup file, false otherwise
       */
      bool HasBackup(Map& map) const;

      /**
       * @param mapName the name map to get the backups count for.
       * @return true if the Map has a backup file, false otherwise
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
       * This will search all open maps until is finds the one that contains the given actor obj id.
       * @return the map that contains a given actor proxy or NULL if it's not found.
       * @param id the id of the actor to search for.
       */
      Map* GetMapForActor(const dtCore::UniqueId& id);

      /**
       * This will search all open maps until is finds the one that contains the given actor obj id.
       * @return the map that contains a given actor proxy or NULL if it's not found.
       * @param id the id of the actor to search for.
       */
      const Map* GetMapForActor(const dtCore::UniqueId& id) const;

      /**
       * This will search all open maps until is finds the one that contains the given actor obj.
       * @return the map that contains a given actor proxy or NULL if it's not found.
       * @param actor the actor to search for.
       */
      Map* GetMapForActor(const BaseActorObject& actor);

      /**
       * This will search all open maps until is finds the one that contains the given actor obj.
       * @return the map that contains a given actor proxy or NULL if it's not found.
       * @param actor the actor to search for.
       */
      const Map* GetMapForActor(const BaseActorObject& actor) const;

      /**
       * This will search for a map event in all currently loaded maps.
       * @param the unique id of the.
       */
      GameEvent* GetGameEvent(const dtCore::UniqueId& id);

      /**
       * This will search for a map event in all currently loaded maps.
       * @param the name of the event.
       */
      GameEvent* GetGameEvent(const std::string& eventName);

      /**
       * @param resourceType the type of resource.  The datatype passed in MUST be a resource type.
       * @param toFill the vector to fill with the handlers.
       * @return The resource types that are available for the given handler.
       * @throws ExceptionEnum::ProjectResourceError if the datatype is a primitive type, not a resource type.
       */
      void GetHandlersForDataType(const DataType& resourceType, std::vector<const ResourceTypeHandler* >& toFill) const;

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
       * @param isCategory The optional parameter, if set to true, will make this call assume the descriptor points
       *                   to a category, which means it will return a path to a directory/folder.  This distinction needs
       *                   to be made because with multiple resource paths, a category could exist that conflicts with a file name
       *                   and the code will skip those categories until it finds the file, and also most code when using this
       *                   call is looking for a file, and a category would mean that there is an error.
       * @return The path to load a resource.
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectResourceError if the string representation is invalid.
       * @throws FileNotFoundException if the file was not found.
       */
      const std::string GetResourcePath(const ResourceDescriptor& resource, bool isCategory = false) const;

      /**
       * Adds a resource to the project by copying it into the project.
       * @param newName the new name of the resource.
       * @param pathToFile The path to the file to copy into the project.
       * @param category The category of the resource file as a dot-notation string.
       * @param type the resounce datatype of the resource.  This must be one of the enums that define a resource.
       * @param slot the id of the context slot to store the map in.
       * @return the resource's unique identifier string.
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws FileExceptionEnum::IOException if the could not complete because of some sort of IO exception.
       * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
       * @throws FileNotFoundException if the file to import does not exist.
       * @throws ExceptionEnum::ProjectResourceError if the file could not be imported if the Datatype is not a resource type.
       */
      const ResourceDescriptor AddResource(const std::string& newName, const std::string& pathToFile, const std::string& category, const DataType& type, ContextSlot slot = DEFAULT_SLOT_VALUE);

      /**
       * Removes a resource from a project.  This will delete the file if it exists.  If no such resource exists, the
       * call will be a noop.  This call is not guaranteed to check all actors to make sure the resource is not in use.
       * @param resource The resource descriptor object.
       * @param slot the default slot
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
       * @throws FileExceptionEnum::IOException if the could not complete because of some sort of IO exception.
       * @throws ExceptionEnum::ProjectResourceError if the resource could not be removed for reasons other than file io.
       */
      void RemoveResource(const ResourceDescriptor& resource, ContextSlot slot = DEFAULT_SLOT_VALUE);

      /**
       * Creates a new resource category.
       * @note if the category already exists, this will be a no-op.
       * @param category the category to create.
       * @param type the data type to add the category into.
       * @param slot the id of the context to create the resource category in.  The default is the first
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
       * @throws FileExceptionEnum::IOException if the operation could not complete because of some sort of IO exception.
       * @throws ExceptionEnum::ProjectResourceError if type is not a resource type.
       */
      void CreateResourceCategory(const std::string& category, const DataType& type, ContextSlot slot = DEFAULT_SLOT_VALUE);

      /**
       * Removes a category from every context. If you don't set the context slot, it will default to all contexts.
       * @param category The category to remove.
       * @param type The datatype the category is in.
       * @param recursive true to recursively delete the category and all subcategories and resources.
       * @param slot the id of the context to remove the resource category from.  The default is all of them.
       * @return true if the category was removed, or didn't exist.  false if the category could not be removed because
       *         recursive was false and the category was not empty.
       * @throws ProjectInvalidContextException if the context is not set.
       * @throws ExceptionEnum::ProjectReadOnly if the project is read only.
       * @throws FileExceptionEnum::IOException if the operation could not complete because of some sort of IO exception.
       * @throws ExceptionEnum::ProjectResourceError if the resource could not be removed for reasons other than file io.
       * @throws FileNotFoundException on rare occasion, this could possibly be thrown if the file contents
       *              are changed while the recusive delete is occuring.
       */
      bool RemoveResourceCategory(const std::string& category, const DataType& type, bool recursive, ContextSlot slot = DEFAULT_SLOT_VALUE);

      /**
       * @note This tree will likely be cached.  See the Project::refresh() method.
       * @see Project#refresh
       * @return all of the resources known to the project.
       */
      const ResourceTree& GetAllResources() const;

      /**
       * @note The data will likely be cached.  See the Project::refresh() method.
       * @see Project#refresh
       * @param toFill The tree to fill with the results.
       * @return all of the resources known to the project of the given type.
       */
      void GetResourcesOfType(const DataType& type, ResourceTree& toFill) const;

      //Add later
      //     void findResources(dtUtil::tree<ResourceTreeNode> toFill,
      //         const DataType& type,
      //         const std::string& partialName,
      //         const std::string& extension) const;

      //void archiveProject(const std::string& targetPath);
      //void unarchiveProject(const std::string& targetPath);

      /**
       * @return true if the current project context is stored in an archive.
       * @throws ProjectInvalidContextException if the context is not set.
       */
      bool IsArchive() const;

      /**
       * @return true if the project is set to read only.  This is NOW valid at all times.
       */
      bool IsReadOnly() const;

      /**
       * Sets if this project should be used as read only or not.  you should set this before
       * setting context directories.
       */
      void SetReadOnly(bool newReadOnly);

      /**
       * This flag will alert actors whether or not there proxy is being loaded into STAGE
       * as a note this should only be set from dtEditQt namespace
       * potential refactor would be to ensure only dtEditQt could set it
       * another implementation would be to have a project state which could
       * control whether we are editing or not
       * if need be we can add a getter for this
       */
      void SetEditMode(bool pInStage);

      /**
       * Returns if Project is being executed from STAGE
       * @return mIsInSTAGE
       * @note NO USER SHOULD CALL THIS FUNCTION, EVER.
       * If you want to find out if you are running from STAGE,
       * please use dtCore::BaseActorObject::IsInSTAGE();
       * @see class dtCore::BaseActorObject
       */
      bool GetEditMode() const;

      /**
       * @return the parser used to load the maps.
       * @note will return NULL if no map is being parsed.
       */
      MapParser* GetCurrentMapParser();

   private:
      static dtCore::RefPtr<Project> mInstance; //< the instance of the project.

      static const std::string LOG_NAME;
      static const std::string MAP_DIRECTORY;
      static const std::string MAP_BACKUP_SUB_DIRECTORY;

      Project();
      // -----------------------------------------------------------------------
      //  Unimplemented constructors and operators
      // -----------------------------------------------------------------------
      Project(const Project&);
      Project& operator=(const Project&);

      friend class ProjectImpl;
      ProjectImpl* mImpl;

   protected:
      virtual ~Project();
   };
}

#endif
