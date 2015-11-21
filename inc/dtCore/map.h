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

#ifndef DELTA_MAP
#define DELTA_MAP

#include <string>
#include <map>

#include <osg/Referenced>
#include <osg/Vec3>
#include <osg/Quat>

#include <dtCore/actorproxy.h>
#include <dtCore/export.h>
#include <dtCore/gameeventmanager.h>
#include <dtUtil/getsetmacros.h>

namespace dtCore 
{
   class ActorHierarchyNode;
   class Project;
   class GameEvent;

   /**
    * @class Map
    * @brief Holds the map information and all of the actor proxies.
    * @note you may not create a new map.  Call Project::createMap(...)
    * @see Project
    */
   class DT_CORE_EXPORT Map : public osg::Referenced 
   {
      public:
         static const std::string MAP_FILE_EXTENSION;
         static const std::string PREFAB_FILE_EXTENSION;

         enum PlaceableFilter 
         {
            Placeable,
            NotPlaceable,
            Either
         };

         class MapGameEvents: public GameEventManager
         {
            public:
               MapGameEvents(Map& parent);
               /**
                *
                */
               virtual void AddEvent(GameEvent& event);

               /**
                * Removes an existing event from the game event manager.
                * @param event The event to remove.  If it is not currently in the
                *  manager, this method is a no-op.
                */
               virtual void RemoveEvent(GameEvent& event);

               /**
                * Removes the game event with the specified unique id from the manager.
                * @param id The unique id of the game event to remove.  If the event is not
                *   currently in the manager, this method is a no-op.
                */
               virtual void RemoveEvent(const dtCore::UniqueId& id);

               /**
                * Clears all the currently registered events from the manager.
                */
               virtual void ClearAllEvents();
            private:
               Map& mParent;
         };

         // Group data.
         struct MapGroupData
         {
            dtCore::ActorPtrVector actorList;
         };
   
         // Preset camera data.
         struct PresetCameraData
         {
         public:
            bool      isValid;

            // Perspective view.
            osg::Vec3 persPosition;
            osg::Quat persRotation;

            // Top view.
            osg::Vec3 topPosition;
            double    topZoom;

            // Side view.
            osg::Vec3 sidePosition;
            double    sideZoom;

            // Front view.
            osg::Vec3 frontPosition;
            double    frontZoom;

            PresetCameraData()
            {
               isValid     = false;

               persPosition.set(0.0f, 0.0f, 0.0f);
               persRotation.set(0.0f, 0.0f, 0.0f, 0.0f);

               topPosition.set(0.0f, 0.0f, 0.0f);
               topZoom     = 1.0f;

               sidePosition.set(0.0f, 0.0f, 0.0f);
               sideZoom    = 1.0f;

               frontPosition.set(0.0f, 0.0f, 0.0f);
               frontZoom   = 1.0f;
            }
         };

         /**
          * Creates a new map
          * @param filemName the filemName to use to save the map.
          *                  If the extension is left off, it will be added.
          * @param mName the mName to give the map.
          * @todo make constructor protected
          */
         Map(const std::string& filemName, const std::string& mName = std::string(""));

         /**
          * @return the mName of the map.
          */
         const std::string& GetName() const;

         /**
          * @return the mName of the map when it was last saved/originally created.
          */
         const std::string& GetSavedName() const;

         /**
          * Sets the maps mName
          * @param newmName the mName to assign
          */
         void SetName(const std::string& newName);

         /**
          * @return the file mName the map was saved to.
          */
         const std::string& GetFileName() const;

         /**
          * @return The map description.  This is intended to be a short description.
          */
         const std::string& GetDescription() const;

         /**
          * Sets the description text.
          * @param newDescription the new text.
          */
         void SetDescription(const std::string& newDescription);

         /**
          * @return The map's author.
          */
         const std::string& GetAuthor() const;

         /**
          * Sets the map's author.
          * @param newAuthor the author's mName.
          */
         void SetAuthor(const std::string& newAuthor);

         /**
          * @return The map comments.  This is intended to be the long comments.
          */
         const std::string& GetComment() const;

         /**
          * Sets the comments.
          * @param newComment the new comment text.
          */
         void SetComment(const std::string& newComment);

         /**
          * @return the copyright information for the map.
          */
         const std::string& GetCopyright() const;

         /**
          * Sets the copyright information.
          * @param newCopyright the new copyright information.
          */
         void SetCopyright(const std::string& newCopyright);

         /**
          * @return the date and time the map was created as a UTC formatted string.
          */
         const std::string& GetCreateDateTime() const;

         /**
          * Sets the create date time for map based on the first time it was saved.
          * The value is not validated, but it is expected to be a UTC formatted string.
          * @param newCreateDateTime the new create date time value.
          */
         void SetCreateDateTime(const std::string& newCreateDateTime);

         /**
          * This icon is used for prefabs.
          */
         DT_DECLARE_ACCESSOR(std::string, IconFile)

         /**
          * Searches all open maps to find the proxy with the given unique id.
          * @param id the id of the proxy to return.
          * @return the proxy found or NULL if it was not found.  This should be stored in a ref_ptr once received.
          */
         BaseActorObject* GetProxyById(const dtCore::UniqueId& id);
         
         /**
          * Templated version of GetProxyById that takes a pointer to the desired type and assigns it by dynamic casting
          * to the that type.
          * @param id the id of the actor to find.
          * @param proxy Output Parametr. A pointer to a proxy by reference.
          */
         template <typename ProxyType>
         void GetProxyById(const dtCore::UniqueId& id, ProxyType*& proxy)
         {
            proxy = dynamic_cast<ProxyType*>(GetProxyById(id));
         }

         /**
          * Searches all open maps to find the proxy with the given unique id.
          * @param id the id of the proxy to return.
          * @return the proxy found or NULL if it was not found.  This should be stored in a ref_ptr once received.
          */
         const BaseActorObject* GetProxyById(const dtCore::UniqueId& id) const;

         /**
          * Templated version of GetProxyById that takes a pointer to the desired type and assigns it by dynamic casting
          * to the that type.
          * @param id the id of the actor to find.
          * @param proxy Output Parametr. A pointer to a proxy by reference.
          */
         template <typename ProxyType>
         void GetProxyById(const dtCore::UniqueId& id, const ProxyType*& proxy) const
         {
            proxy = dynamic_cast<ProxyType*>(GetProxyById(id));
         }

         /**
          * non-const search for proxies.
          * @param container The container to fill.
          * @param name The name of the proxy to find or empty string to not use the name.
          * @param category The category of the proxy to find, this will also find proxies in subcategories. Use empty string to ignore.
          * @param typeName The name of the proxy actor type. Use empty string or omit the parameter to ignore the type.
          * @param className The name of the actor class to match. It will match this as a superclass.
          * @param placeable set to Placeable to search for only proxys that return true for isPlaceable,
          *                  NonPlaceable for false, or Either for both.
          */
         void FindProxies(std::vector<dtCore::RefPtr<BaseActorObject> >& container,
                          const std::string& name,
                          const std::string& category = std::string(""),
                          const std::string& typeName = std::string(""),
                          const std::string& className = std::string(""),
                          PlaceableFilter placeable = Either);

         /**
          * Const version of find for methods that may need to find a proxy only to read values.
          * @param container The container to fill, note that it holds const BaseActorObject.
          * @param name The name of the proxy to find or empty string to not use the name.
          * @param category The category of the proxy to find, this will also find proxies in subcategories.
          *                  Use empty string to ignore.
          * @param typeName The name of the proxy actor type. Use empty string or omit the parameter to ignore the type.
          * @param className The name of the actor class to match. It will match this as a superclass.
          * @param placeable set to Placeable to search for only proxys that return true for isPlaceable,
          *                  NonPlaceable for false, or Either for both.
          */
         void FindProxies(std::vector<dtCore::RefPtr<const BaseActorObject> >& container,
                          const std::string& name, const std::string& category = std::string(""),
                          const std::string& typeName = std::string(""),
                          const std::string& className = std::string(""),
                          PlaceableFilter placeable = Either) const;

         /**
          * @return a vector of ref pointers to the all the proxies in the map.
          */
         const std::map<dtCore::UniqueId, dtCore::RefPtr<BaseActorObject> >& GetAllProxies() const;

         /**
          * @return fills a vector with ref pointers to all the proxies in the map.
          */
         void GetAllProxies(std::vector<dtCore::RefPtr<BaseActorObject> >& container);

         /**
          * Adds a new proxy to the map.
          * @param proxy the proxy to add.
          * @param reNumber true if we want to re-number the proxy with a unique value
          */
         void AddProxy(BaseActorObject& proxy, bool reNumber = false);

         /**
          * Removes a proxy.
          * @param proxy the proxy to remove.
          * @return true if the proxy passed in was actually removed.
          */
         bool RemoveProxy(BaseActorObject& proxy);

         /**
         * Should be called when a proxy has been renamed.
         * This will keep track of our highest number values.
         * @param proxy the proxy that is being renamed.
         */
         void OnProxyRenamed(BaseActorObject& proxy);

         /**
          * Clear all the proxies from the map.
          */
         void ClearProxies();

         /**
          * @return a set of all the names of the classes the proxies in this map wrap or inherit from.
          */
         const std::set<dtUtil::RefString>& GetProxyActorClasses() const;

         /**
          * rebuilds the list of classes of the actor proxies since items are not removed when
          * proxies are not removed.  Call this after a large number of proxies has been removed.
          */
         void RebuildProxyActorClassSet() const;

         /**
          * @return true if this map has been modified since it was loaded.
          */
         bool IsModified() const;

         /**
          * Sets the map modified status.
          * @param val True modified, false otherwise.
          */
         void SetModified(bool val);

         bool HasLoadingErrors() const;

         const std::vector<std::string>& GetMissingLibraries() const;

         const std::set<std::string>& GetMissingActorTypes() const;

         /**
          * @return a map of library mNames mapped to their version numbers.  These are the libraries the map depends on.
          */
         const std::map<std::string, std::string>& GetLibraryVersionMap() const;

         /**
          * @return A list of library mNames in the order they should be loaded.
          */
         const std::vector<std::string>& GetAllLibraries() const;

         /**
          * @param mName the mName of the library.
          * @return true if this map loads the given library.
          */
         bool HasLibrary(const std::string& name) const;

         /**
          * @param mName the mName of the library to query.
          * @return the version of the library passed in or "" if that library is not referenced by the map.
          */
         const std::string GetLibraryVersion(const std::string& name) const;

         /**
          * Inserts a library with the given mName at the given position.  If a library of the given mName is already in the map,
          * the version will be updated and the order adjusted to match the iterator.
          * @param pos the position to place the library.
          * @param mName the mName of the library
          * @param version the version string of the library.
          */
         void InsertLibrary(unsigned pos, const std::string& name, const std::string& version);

         /**
          * This will add the given library to the map.  If a library by the given name is already in the map, the library
          * will be placed at the end and the version will be updated to the one given.
          * @param mName the mName of the library.
          * @param version the version string of the library.
          */
         void AddLibrary(const std::string& name, const std::string& version);

         /**
          * Removes a library by mName.
          * @param mName the mName of the library to remove.
          * @return true or false if an item was actually removed.
          */
         bool RemoveLibrary(const std::string& name);

         /**
          * Iterates over all the actors in the map and adds any missing
          */
         void CorrectLibraryList(bool removeUnusedLibraries = false);

         /**
          * Matches a string against wildcard string.  "*" matches anything.  "?" matches any single character.
          * @param sWild the string with wildcards to match with.
          * @param sString the string to check.
          * @return true or false if sString matches or doesn't match the wildcard.
          */
         static bool WildMatch(const std::string& sWild, const std::string& sString);

         /**
          * Returns the environment actor of this map or NULL if no environment is set
          * @return A pointer to the environment actor or NULL
          */
         BaseActorObject* GetEnvironmentActor();

         /**
          * const version of the above function
          * Returns the environment actor of this map or NULL if no environment is set
          * @return A pointer to the environment actor or NULL
          */
         const BaseActorObject* GetEnvironmentActor() const;

         ///@return the GameEventManager that holds the game events for this map.
         GameEventManager& GetEventManager();
         ///@return as const the GameEventManager that holds the game events for this map.
         const GameEventManager& GetEventManager() const;

         /**
          * Sets the environment actor on this map
          * @param envActor The new environment actor to set
          */
         void SetEnvironmentActor(BaseActorObject *envActor);

         /**
          * Retrieves the total number of groups.
          */
         int GetGroupCount();

         /**
         * Retrieves the number of actors in a group.
         *
         * @param[in]  groupIndex  The index of the group.
         *
         * @return     The total actors within the group, -1 if the group does not exist.
         */
         int GetGroupActorCount(int groupIndex);

         /**
         * Adds an actor into a given group.
         *
         * @param[in]  groupIndex  The index of the group.
         *                         If the index is invalid, a new group will be created.
         *
         * @param[in]  actor       The actor to add to the group.
         */
         void AddActorToGroup(int groupIndex, dtCore::BaseActorObject& actor);

         /**
         * Removes an actor from any groups they are currently in.
         *
         * @param[in]  actor  The actor to remove.
         *
         * @return     True if the actor belonged to any groups.
         */
         bool RemoveActorFromGroups(dtCore::BaseActorObject& actor);

         /**
         * Retrieves a group that belongs to the given actor.
         *
         * @param[in]  actor  The actor to search for.
         *
         * @return     The group in which this actor belongs to.
         */
         int FindGroupForActor(dtCore::BaseActorObject& actor);

         /**
         * Retrieves an actor from a group.
         *
         * @param[in]  groupIndex  The index of the group to get.
         * @param[in]  actorIndex  The index of the actor to get.
         *
         * @return     The actor, or NULL if the group or actor index are out of scope.
         */
         dtCore::BaseActorObject* GetActorFromGroup(int groupIndex, int actorIndex);

         /**
         * Gets the matrix of a preset camera position.
         *
         * @param[in]  index  The index of the preset camera to retrieve.
         *
         * @return     The matrix to the preset camera.
         */
         PresetCameraData GetPresetCameraData(int index);

         /**
         * Sets a preset camera index.
         *
         * @param[in]  index       The preset camera index.
         * @param[in]  presetData  The preset camera data.
         */
         void SetPresetCameraData(int index, PresetCameraData presetData);

         /**
          * Resets the UUID on every actor in the map, including subcomponents
          */
         void ResetUUIDs();

         dtCore::ActorHierarchyNode* GetDrawableActorHierarchy() { return mDrawableActorHierarchy; }

      protected:
         friend class Project;
         friend class ProjectImpl;

         /**
          * Assigns the file name this map should be saved to. It should not have
          * an extension.
          * @param newFileName the new file name.
          */
         void SetFileName(const std::string& newFileName);

         /**
          * Assigns the saved map name.  This is used as an override when loading backup copies
          * of maps since they may have had the name changed and need this set.
          * @param newSavedname the new file name.
          */
         void SetSavedName(const std::string& newSavedName);

         /**
          * Clears the modified flag on this map and clears the lists of missing libraries
          * and actor types.
          */
         void ClearModified();

         /**
          * Adds a vector of library mNames to the list of libraries that could not
          * be loaded when this map was read from a save file.
          * @param libs the libraries to add.
          */
         void AddMissingLibraries(const std::vector<std::string>& libs);

         /**
          * Adds a set of actor types to the set of types that could not be created
          * when this map was loaded from a save file.
          * @param types the types to add.
          */
         void AddMissingActorTypes(const std::set<std::string>& types);

         virtual ~Map();
         
       private:
         bool mModified;
         //typedef std::multimap<std::string, dtCore::RefPtr<BaseActorObject> > ProxiesByClassMap;
         std::string mName;
         //The name set when it was created/last saved.
         std::string mSavedName;
         std::string mFileName;
         bool mCreateNavMesh;
         std::string mDescription;
         std::string mAuthor;
         std::string mComment;

         std::string mCopyright;
         std::string mCreateDateTime;

         dtCore::RefPtr<dtCore::BaseActorObject> mEnvActor;

         dtCore::RefPtr<ActorHierarchyNode>     mDrawableActorHierarchy;

         dtCore::RefPtr<GameEventManager> mEventManager;

         //ProxiesByClassMap proxiesByClass;
         typedef std::map<dtCore::UniqueId, dtCore::RefPtr<BaseActorObject> > ActorMap;
         ActorMap mActorMap;

         std::map<std::string, std::string> mLibraryVersionMap;
         std::vector<std::string> mLibraryOrder;

         std::map<std::string, int> mProxyNumberMap;

         std::vector<std::string> mMissingLibraries;
         std::set<std::string> mMissingActorTypes;

         std::vector<MapGroupData> mActorGroups;

         //all of the classes used by the proxies in the map.
         mutable std::set<dtUtil::RefString> mProxyActorClasses;

         std::vector<PresetCameraData> mPresetCameras;

         bool MatchesSearch(const BaseActorObject& actorProxy, const std::string& category, const std::string& typemName,
                           const std::string& classmName, PlaceableFilter placeable) const;

         static bool Match(char* WildCards, char* str);
         static bool Scan(char*& Wildcards, char*& str);

         /**
         * This function will separate a full proxy name into its
         * name and number components.
         *
         * @param[in]   fullName  The original full name of the proxy.
         * @param[out]  name      The returned name component.
         * @param[out]  number    The returned number component.
         */
         void SplitProxyName(const std::string& fullName, std::string& name, std::string& number);

         /**
         * Converts a given number to a string value.
         *
         * @param[in]  The number to convert.
         *
         * @return     The returned string value of the number.
         */
         std::string NumberToString(int number);

         bool RemoveProxy_Internal(const BaseActorObject& proxy);

         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         Map(const Map&);
         Map& operator=(const Map&);
   };

   typedef RefPtr<Map> MapPtr;
}

#endif
