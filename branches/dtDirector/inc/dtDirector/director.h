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

#include <dtDAL/map.h>

#include <dtUtil/log.h>


namespace dtDirector
{
   /**
    * Director Graph structure that contains all nodes
    * within the director script.
    */
   class DT_DIRECTOR_EXPORT DirectorGraphData: public dtDAL::PropertyContainer
   {
   public:

      /**
       * Constructor.
       */
      DirectorGraphData(Director* director);

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
      virtual void BuildPropertyMap(bool isParent = false);

      /**
       * Updates all nodes in the graph.
       *
       * @param[in]  simDelta  Elapsed sim time.
       * @param[in]  delta     Elapsed real time.
       */
      void Update(float simDelta, float delta);

      /**
       * Retrieves a node of the given ID.
       *
       * @param[in]  id  The ID of the node.
       *
       * @return     A pointer to the node found, or NULL
       *             if not found.
       */
      Node* GetNode(const dtCore::UniqueId& id);

      /**
       * Adds a node to the graph.
       *
       * @param[in]  node  The node.
       */
      bool AddNode(Node* node);
      
      /**
       * Retrieves the director.
       *
       * @return  The director.
       */
      Director* GetDirector() {return mDirector;}

      /**
       * Accessors for the graph name.
       */
      void SetName(const std::string& name) {mName = name;}
      const std::string& GetName() {return mName;}

      /**
       * Accessors for the graph position.
       */
      void SetPosition(const osg::Vec2& pos) {mPosition = pos;}
      const osg::Vec2& GetPosition() {return mPosition;}

      /**
       * Accessors for the node lists.
       */
      std::vector<dtCore::RefPtr<EventNode> >& GetEventNodes() {return mEventNodes;}
      std::vector<dtCore::RefPtr<ActionNode> >& GetActionNodes() {return mActionNodes;}
      std::vector<dtCore::RefPtr<ValueNode> >& GetValueNodes() {return mValueNodes;}

      /**
       * Retrieves a list of all external input, output, or value
       * nodes that should be visible from the parent.
       */
      std::vector<dtCore::RefPtr<EventNode> >  GetInputNodes();
      std::vector<dtCore::RefPtr<ActionNode> > GetOutputNodes();
      std::vector<dtCore::RefPtr<ValueNode> >  GetExternalValueNodes();

      /**
       * Accessor for sub graphs.
       */
      std::vector<dtCore::RefPtr<DirectorGraphData> >& GetSubGraphs() {return mSubGraphs;}

      Director*          mDirector;
      DirectorGraphData* mParent;

      std::string mName;
      osg::Vec2   mPosition;

      std::vector<dtCore::RefPtr<DirectorGraphData> > mSubGraphs;

      std::vector<dtCore::RefPtr<EventNode> >  mEventNodes;
      std::vector<dtCore::RefPtr<ActionNode> > mActionNodes;
      std::vector<dtCore::RefPtr<ValueNode> >  mValueNodes;
   };


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
       *
       * @param[in]  map  The current map.
       */
      virtual void Init(dtDAL::Map* map = NULL);

      /**
       * Clears all data in this Director script.
       */
      virtual void Clear();

      /**
       * Retrieves the map.
       *
       * @return the map.
       */
      dtDAL::Map* GetMap() {return mMap;}

      /**
       * Sets the map.
       *
       * @param[in]  map  The current map.
       */
      void SetMap(dtDAL::Map* map) {mMap = map;}

      void CreateDebugScript();

      /**
       * Loads a Director script.
       *
       * @param[in]  scriptFile  The name of the script file to load.
       * @param[in]  map         The current map (this may be optional based
       *                          the type of script being loaded).
       *
       * @return     True if the script was found and loaded successfuly.
       */
      bool LoadScript(const std::string& scriptFile, dtDAL::Map* map);

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
      void SetName(const std::string& name) {mGraph->SetName(name);}
      const std::string& GetName() {return mGraph->GetName();}

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
       * Retrieves the graph data.
       */
      DirectorGraphData* GetGraphData() {return mGraph.get();}

      /**
       * Retrieves a node of the given the id.
       *
       * @param[in]  id  The id of the node.
       *
       * @return     A pointer to the node or NULL if not found.
       */
      Node* GetNode(const dtCore::UniqueId& id);

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~Director();

   private:

      // Core Info.
      std::string mDescription;
      std::string mAuthor;
      std::string mComment;
      std::string mCopyright;
      std::string mCreationTime;

      dtCore::RefPtr<dtDAL::Map> mMap;
      bool        mModified;

      std::vector<std::string> mLibraries;
      std::map<std::string, std::string> mLibraryVersionMap;

      dtCore::RefPtr<DirectorGraphData> mGraph;

      dtUtil::Log*   mLogger;
   };
}

#endif // DIRECTOR_DIRECTOR