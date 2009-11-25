/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTOR_DIRECTOR
#define DIRECTOR_DIRECTOR

#include <dtDirector/export.h>

#include <dtDirector/eventnode.h>
#include <dtDirector/actionnode.h>
#include <dtDirector/valuenode.h>

#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>
#include <dtDirector/valuelink.h>

#include <dtUtil/log.h>


namespace dtDirector
{
   /**
    * This is the base class for all action nodes.
    *
    * @note
    *      Node objects must be created through the NodePluginRegistry or
    *      the NodeManager. If they are not created in this fashion,
    *      the node types will not be set correctly.
    */
   class DT_DIRECTOR_EXPORT Director: public dtDAL::PropertyContainer
   {
   public:

      /**
       * Constructs the Director.
       */
      Director();

      /**
       * Initializes the Director.
       */
      virtual void Init();

      /**
       * Loads a Director script.
       *
       * @param[in]  scriptFile  The name of the script file to load.
       *
       * @return     True if the script was found and loaded successfuly.
       */
      bool LoadScript(const std::string& scriptFile);

      /**
       * Saves a Director script.
       *
       * @param[in]  scriptFile  The name of the script file to save.
       *
       * @return     True if the script was saved successfuly.
       */
      bool SaveScript(const std::string& scriptFile);

      /**
       * This method is called in init, which instructs the director
       * to create its properties.  Methods implementing this should
       * be sure to call their parent class's buildPropertyMap method to
       * ensure all properties in the proxy inheritance hierarchy are
       * correctly added to the property map.
       *
       * @see GetDeprecatedProperty to handle old properties that need
       *       to be removed.
       */
      virtual void BuildPropertyMap();

      /**
       * Updates the Director.
       *
       * @param[in]  simDelta  The simulation time step.
       * @param[in]  delta     The real time step.
       */
      virtual void Update(float simDelta, float delta);

      /**
       * Accessors for the name of the script.
       */
      void SetName(const std::string& name) {mName = name;}
      std::string& GetName() {return mName;}

      /**
       * Accessors for the description of the script.
       */
      void SetDescription(const std::string& description) {mDescription = description;}
      std::string& GetDescription() {return mDescription;}

      /**
       * Accessors for the Author of the script.
       */
      void SetAuthor(const std::string& author) {mAuthor = author;}
      std::string& GetAuthor() {return mAuthor;}

      /**
       * Accessors for the comment of the script.
       */
      void SetComment(const std::string& comment) {mComment = comment;}
      std::string& GetComment() {return mComment;}

      /**
       * Accessors for the copyright information of the script.
       */
      void SetCopyright(const std::string& copyright) {mCopyright = copyright;}
      std::string& GetCopyright() {return mCopyright;}

      /**
       * Accessors for the scripts creation time.
       */
      void SetCreateDateTime(const std::string& time) {mCreationTime = time;}
      std::string& GetCreateDateTime() {return mCreationTime;}

      /**
       * Inserts a node library with the given name at the given position.
       * If a library of the given name is already listed, the version
       * will be updated and the order adjusted to match the iterator.
       *
       * @param[in]  pos      The position to place the library.
       * @param[in]  name     The name of the library
       * @param[in]  version  The version string of the library.
       */
      void InsertLibrary(unsigned pos, const std::string& name, const std::string& version);

      /**
       * This will add the given library to list.  If a library by the
       * given name is already listed, the library will be placed at the
       * end and the version will be update to the one given.
       *
       * @param[in]  name     The name of the library.
       * @param[in]  version  The version string of the library.
       */
      void AddLibrary(const std::string& name, const std::string& version);

      /**
       * Removes a library from the list.
       *
       * @param[in]  name  The name of the library to remove.
       *
       * @return     True if an item was actually removed.
       */
      bool RemoveLibrary(const std::string& name);

      /**
       * Retrieves whether a library is listed.
       *
       * @param[in]  name  The name of the library.
       *
       * @return     True if this map loads the given library.
       */
      bool HasLibrary(const std::string& name) const;

      /**
       * Retrieves all node libraries referenced in this script.
       */
      const std::vector<std::string>& GetAllLibraries() const;
      const std::string GetLibraryVersion(const std::string& mName) const;

      /**
       * Retrieves the list of event nodes.
       */
      const std::vector<dtCore::RefPtr<EventNode> >& GetEventNodes() const {return mEventNodes;}

      /**
       * Retrieves the list of action nodes.
       */
      const std::vector<dtCore::RefPtr<ActionNode> >& GetActionNodes() const {return mActionNodes;}

      /**
       * Retrieves the list of value nodes.
       */
      const std::vector<dtCore::RefPtr<ValueNode> >& GetValueNodes() const {return mValueNodes;}

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~Director();

   private:

      // Core Info.
      std::string mName;
      std::string mDescription;
      std::string mAuthor;
      std::string mComment;
      std::string mCopyright;
      std::string mCreationTime;

      bool        mModified;

      std::vector<std::string> mLibraries;
      std::map<std::string, std::string> mLibraryVersionMap;

      std::vector<dtCore::RefPtr<EventNode> >  mEventNodes;
      std::vector<dtCore::RefPtr<ActionNode> > mActionNodes;
      std::vector<dtCore::RefPtr<ValueNode> >  mValueNodes;

      dtUtil::Log*   mLogger;
   };
}

#endif // DIRECTOR_DIRECTOR