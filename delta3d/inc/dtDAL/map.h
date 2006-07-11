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
 * @author David Guthrie
 */

#ifndef DELTA_MAP
#define DELTA_MAP

#include <string>
#include <map>

#include <osg/Referenced>
#include <osg/ref_ptr>

#include "dtDAL/actorproxy.h"
#include "dtDAL/export.h"
#include "dtDAL/environmentactor.h"

namespace dtDAL 
{
   class Project;

   /**
    * @class Map
    * @brief Holds the map information and all of the actor proxies.
    * @note you may not create a new map.  Call Project::createMap(...)
    * @see Project
    */
   class DT_DAL_EXPORT Map : public osg::Referenced 
   {
      public:
         static const std::string MAP_FILE_EXTENSION;

         enum PlaceableFilter 
         {
            Placeable,
            NotPlaceable,
            Either
         };

         /**
          * @return the mName of the map.
          */
         const std::string& GetName() const { return mName; }

         /**
          * @return the mName of the map when it was last saved/originally created.
          */
         const std::string& GetSavedName() const { return mSavedName; }

         /**
          * Sets the maps mName
          * @param newmName the mName to assign
          */
         void SetName(const std::string& newName) { mModified = true; mName = newName; }

         /**
          * @return the file mName the map was saved to.
          */
         const std::string& GetFileName() const { return mFileName; }

         /**
          * @return The map description.  This is intended to be a short description.
          */
         const std::string& GetDescription() const { return mDescription; }

         /**
          * Sets the description text.
          * @param newDescription the new text.
          */
         void SetDescription(const std::string& newDescription) { mModified = true;  mDescription = newDescription; }

         /**
          * @return The map's author.
          */
         const std::string& GetAuthor() const { return mAuthor; }

         /**
          * Sets the map's author.
          * @param newAuthor the author's mName.
          */
         void SetAuthor(const std::string& newAuthor) { mModified = true; mAuthor = newAuthor; }


         /**
         * @return The name of the file containing waypoints or pathnodes
         */
         const std::string& GetPathNodeFileName() const { return mPathNodeFile; }

         /**
         * Sets the name of the file used for loading and saving pathnodes.
         * @param newFilename the new filename.
         */
         void SetPathNodeFileName(const std::string& newFilename) { mModified = true;  mPathNodeFile = newFilename; }

         /**
          * @return The map comments.  This is intended to be the long comments.
          */
         const std::string& GetComment() const { return mComment; }

         /**
          * Sets the comments.
          * @param newComment the new comment text.
          */
         void SetComment(const std::string& newComment) { mModified = true; mComment = newComment; }

         /**
          * @return the copyright information for the map.
          */
         const std::string& GetCopyright() const { return mCopyright; }

         /**
          * Sets the copyright information.
          * @param newCopyright the new copyright information.
          */
         void SetCopyright(const std::string& newCopyright) { mModified = true; mCopyright = newCopyright; }

         /**
          * @return the date and time the map was created as a UTC formatted string.
          */
         const std::string& GetCreateDateTime() const { return mCreateDateTime; }

         /**
          * Sets the create date time for map based on the first time it was saved.
          * The value is not validated, but it is expected to be a UTC formatted string.
          * @param newCreateDateTime the new create date time value.
          */
         void SetCreateDateTime(const std::string& newCreateDateTime)
         {
            mModified = true;
            mCreateDateTime = newCreateDateTime;
         }

         /**
          * Searches all open maps to find the proxy with the given unique id.
          * @param id the id of the proxy to return.
          * @return the proxy found or NULL if it was not found.  This should be stored in a ref_ptr once received.
          */
         ActorProxy* GetProxyById(const dtCore::UniqueId& id);

         /**
          * Searches all open maps to find the proxy with the given unique id.
          * @param id the id of the proxy to return.
          * @return the proxy found or NULL if it was not found.  This should be stored in a ref_ptr once received.
          */
         const ActorProxy* GetProxyById(const dtCore::UniqueId& id) const;

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
         void FindProxies(std::vector<osg::ref_ptr<ActorProxy> >& container,
                          const std::string& name,
                          const std::string& category = std::string(""),
                          const std::string& typeName = std::string(""),
                          const std::string& className = std::string(""),
                          PlaceableFilter placeable = Either);

         /**
          * Const version of find for methods that may need to find a proxy only to read values.
          * @param container The container to fill, note that it holds const ActorProxy.
          * @param name The name of the proxy to find or empty string to not use the name.
          * @param category The category of the proxy to find, this will also find proxies in subcategories.
          *                  Use empty string to ignore.
          * @param typeName The name of the proxy actor type. Use empty string or omit the parameter to ignore the type.
          * @param className The name of the actor class to match. It will match this as a superclass.
          * @param placeable set to Placeable to search for only proxys that return true for isPlaceable,
          *                  NonPlaceable for false, or Either for both.
          */
         void FindProxies(std::vector<osg::ref_ptr<const ActorProxy> >& container,
                          const std::string& name, const std::string& category = std::string(""),
                          const std::string& typeName = std::string(""),
                          const std::string& className = std::string(""),
                          PlaceableFilter placeable = Either) const;

         /**
          * @return a vector of ref pointers to the all the proxies in the map.
          */
         const std::map<dtCore::UniqueId, osg::ref_ptr<ActorProxy> >& GetAllProxies() const { return mProxyMap; }

         /**
          * @return fills a vector with ref pointers to all the proxies in the map.
          */
         void GetAllProxies(std::vector<osg::ref_ptr<ActorProxy> >& container) { FindProxies(container, ""); }

         /**
          * Adds a new proxy to the map.
          * @param proxy the proxy to add.
          */
         void AddProxy(ActorProxy& proxy);

         /**
          * Removes a proxy.
          * @param proxy the proxy to remove.
          * @return true if the proxy passed in was actually removed.
          */
         bool RemoveProxy(const ActorProxy& proxy);

         /**
          * Clear all the proxies from the map.
          */
         void ClearProxies();

         /**
          * @return a set of all the names of the classes the proxies in this map wrap or inherit from.
          */
         const std::set<std::string>& GetProxyActorClasses() const { return mProxyActorClasses; }

         /**
          * rebuilds the list of classes of the actor proxies since items are not removed when
          * proxies are not removed.  Call this after a large number of proxies has been removed.
          */
         void RebuildProxyActorClassSet() const;

         /**
          * @return true if this map has been modified since it was loaded.
          */
         bool IsModified() const { return mModified; }

         /**
          * Sets the map modified status.
          * @param val True modified, false otherwise.
          */
         void SetModified(bool val) { mModified = val; }

         bool HasLoadingErrors() 
         {
            return mMissingLibraries.size() > 0 || mMissingActorTypes.size() > 0;
         }

         const std::vector<std::string>& GetMissingLibraries() { return mMissingLibraries; }

         const std::set<std::string>& GetMissingActorTypes() { return mMissingActorTypes; }

         /**
          * @return a map of library mNames mapped to their version numbers.  These are the libraries the map depends on.
          */
         const std::map<std::string, std::string>& GetLibraryVersionMap() const { return mLibraryVersionMap; }

         /**
          * @return A list of library mNames in the order they should be loaded.
          */
         const std::vector<std::string>& GetAllLibraries() const { return mLibraryOrder; }

         /**
          * @param mName the mName of the library.
          * @return true if this map loads the given library.
          */
         bool HasLibrary(const std::string& mName) const 
         {
            return mLibraryVersionMap.find(mName) != mLibraryVersionMap.end();
         }

         /**
          * @param mName the mName of the library to query.
          * @return the version of the library passed in or "" if that library is not referenced by the map.
          */
         const std::string GetLibraryVersion(const std::string& mName) const 
         {
            std::map<std::string, std::string>::const_iterator i = mLibraryVersionMap.find(mName);
            if (i == mLibraryVersionMap.end())
               return std::string("");

            return i->second;
         }

         /**
          * Inserts a library with the given mName at the given position.  If a library of the given mName is already in the map,
          * the version will be updated and the order adjusted to match the iterator.
          * @param pos the position to place the library.
          * @param mName the mName of the library
          * @param version the version string of the library.
          */
         void InsertLibrary(unsigned pos, const std::string& mName, const std::string& version);

         /**
          * This will add the given library to the map.  If a library by the given mName is already in the map, the library
          * will be placed at the end and the version will be update to the one given.
          * @param mName the mName of the library.
          * @param version the version string of the library.
          */
         void AddLibrary(const std::string& mName, const std::string& version);

         /**
          * Removes a library by mName.
          * @param mName the mName of the library to remove.
          * @return true or false if an item was actually removed.
          */
         bool RemoveLibrary(const std::string& mName);

         /**
          * Creates a new map
          * @param filemName the filemName to use to save the map.
          *                  If the extension is left off, it will be added.
          * @param mName the mName to give the map.
          * @todo make constructor protected
          */
         Map(const std::string& filemName, const std::string& mName = std::string(""));

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
         ActorProxy* GetEnvironmentActor() { return mEnvActor.get(); }

         /**
          * const version of the above function
          * Returns the environment actor of this map or NULL if no environment is set
          * @return A pointer to the environment actor or NULL
          */
         const ActorProxy* GetEnvironmentActor() const { return mEnvActor.get(); }

         /**
          * Sets the environment actor on this map
          * @param envActor The new environment actor to set
          */
         void SetEnvironmentActor(ActorProxy *envActor);
         

      protected:
         friend class Project;


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
         void SetSavedName(const std::string& newSavedName) { mSavedName = newSavedName; }

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

         virtual ~Map() {}
      
       private:
         bool mModified;
         //typedef std::multimap<std::string, osg::ref_ptr<ActorProxy> > ProxiesByClassMap;
         std::string mName;
         //The name set when it was created/last saved.
         std::string mSavedName;
         std::string mFileName;
         std::string mPathNodeFile;
         std::string mDescription;
         std::string mAuthor;
         std::string mComment;

         std::string mCopyright;
         std::string mCreateDateTime;

         osg::ref_ptr<dtDAL::ActorProxy> mEnvActor;

         //ProxiesByClassMap proxiesByClass;
         std::map<dtCore::UniqueId, osg::ref_ptr<ActorProxy> > mProxyMap;

         std::map<std::string, std::string> mLibraryVersionMap;
         std::vector<std::string> mLibraryOrder;

         std::vector<std::string> mMissingLibraries;
         std::set<std::string> mMissingActorTypes;

         //all of the classes used by the proxies in the map.
         mutable std::set<std::string> mProxyActorClasses;

         bool MatchesSearch(const ActorProxy& actorProxy, const std::string& category, const std::string& typemName,
                           const std::string& classmName, PlaceableFilter placeable) const;

         static bool Match(char* WildCards, char* str);
         static bool Scan(char*& Wildcards, char*& str);

         // -----------------------------------------------------------------------
         //  Unimplemented constructors and operators
         // -----------------------------------------------------------------------
         Map(const Map&);
         Map& operator=(const Map&);

   };

}

#endif
