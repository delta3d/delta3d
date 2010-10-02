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
#include <dtDirector/directorgraph.h>
#include <dtDirector/messagegmcomponent.h>

#include <dtDAL/map.h>

#include <dtGame/gamemanager.h>

#include <dtCore/observerptr.h>
#include <dtUtil/getsetmacros.h>

#include <cstdio>

namespace dtUtil
{
   class Log;
}


/** 
 * Provides a node-based hierarchy for script processing.
 */
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
   private:
      struct ThreadData;

   public:
      struct RecordThreadData;

      // Recording Data.
      struct RecordNodeData
      {
         float time;
         dtCore::UniqueId nodeID;
         std::string input;
         std::vector<std::string> outputs;

         std::vector<RecordThreadData*> subThreads;
      };

      struct RecordThreadData
      {
         std::vector<RecordNodeData> nodes;
      };

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
      virtual void Init(dtGame::GameManager* gm = NULL, dtDAL::Map* map = NULL);

      /**
       * Clears all data in this Director script.
       */
      virtual void Clear();

      /**
       * Clears all recording data.
       *
       * @param[in]  threads  The recording threadlist to clear.
       */
      void ClearRecordingData(std::vector<RecordThreadData*>& threads);


      /**
       * Clears all running threads to stop all current nodes.
       */
      void ClearThreads();

      /**
       * Retrieves the Director Proxy if it has one.
       *
       * @return  The Director Proxy.
       */
      Director* GetParent() {return mParent.get();}

      /**
       * Sets the Director Proxy.
       *
       * @param[in]  proxy  The Director Proxy.
       */
      void SetParent(Director* parent) {mParent = parent;}

      /**
       * Retrieves the map.
       *
       * @return  The map.
       */
      dtDAL::Map* GetMap();

      /**
       * Sets the map.
       *
       * @param[in]  map  The current map.
       */
      void SetMap(dtDAL::Map* map);

      /**
       * Retrieves the Messaging GM component.
       *
       * @return  The Messaging GM component.
       */
      dtDirector::MessageGMComponent* GetMessageGMComponent() {return mMessageGMComponent;}

      /**
       * Retrieves the Game Manager.
       *
       * @return  The Game Manager.
       */
      dtGame::GameManager* GetGameManager() {return mGameManager;}

      /**
       * Retrieves the logger.
       */
      dtUtil::Log* GetLogger();

      /**
      * Retrieves the currently loaded script.
      */
      const std::string& GetScriptName() {return mScriptName;}

      /**
       * Loads a Director script.
       *
       * @param[in]  scriptFile  The name of the script file to load.
       *
       * @return     True if the script was found and loaded successfully.
       */
      bool LoadScript(const std::string& scriptFile);

      /**
       * Saves a Director script.
       *
       * @param[in]  scriptFile  The name of the script file to save.
       *
       * @return     True if the script was saved successfully.
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
      void Update(float simDelta, float delta);

      /**
       * Starts a new update thread.
       *
       * @param[in]  node       The starting node to update.
       * @param[in]  index      The index of the input being activated.
       * @param[in]  immediate  True to immediately execute the new thread.
       */
      void BeginThread(Node* node, int index, bool immediate = false);

      /**
       * Pushes a new item to the thread stack.
       *
       * @param[in]  node   The new stacks starting node.
       * @param[in]  index  The new stacks started input.
       */
      void PushStack(Node* node, int index);

      /**
       * Retrieves whether there are any active threads running.
       */
      bool IsRunning();

      /**
       * Accessors for the name of the script.
       */
      void SetName(const std::string& name);
      const std::string& GetName();

      /**
       * Accessors for the description of the script.
       */
      void SetDescription(const std::string& description);
      std::string& GetDescription();

      /**
       * Accessors for the Author of the script.
       */
      void SetAuthor(const std::string& author);
      std::string& GetAuthor();

      /**
       * Accessors for the comment of the script.
       */
      void SetComment(const std::string& comment);
      std::string& GetComment();

      /**
       * Accessors for the copyright information of the script.
       */
      void SetCopyright(const std::string& copyright);
      std::string& GetCopyright();

      /**
       * Accessors for the scripts creation time.
       */
      void SetCreateDateTime(const std::string& time);
      std::string& GetCreateDateTime();

      /**
       * Accessors for the node logging flag.
       */
      void SetNodeLogging(bool enabled);
      bool GetNodeLogging();

      /**
       * Sets the player.
       *
       * @param[in]  player  The player.
       */
      void SetPlayer(const dtCore::UniqueId& player);

      /**
       * Retrieves the player.
       */
      dtCore::UniqueId GetPlayer();

      /**
      * Begins recording of the Director graphs.
      */
      void StartRecording();

      /**
      * Pauses the recording of the Director graphs.
      */
      void PauseRecording();

      /**
      * Ends the recording of the Director graphs.
      */
      void StopRecording();

      /**
       * Retrieves whether this Director is recording.
       *
       * @return  True if we are recording.
       */
      bool IsRecording();

      /**
      * Saves any recorded data to a file.
      *
      * @param[in]  filename  The name of the file to save.
      *
      * @return     Returns true if the file was saved.
      */
      bool SaveRecording(const std::string& filename);

      /**
      * Loads any recorded data for this script.
      * @note  All recorded nodes must match the current loaded script
      *        or this will fail.
      *
      * @param[in]  filename  The name of the file to load.
      *
      * @return     Returns true if the file was loaded.
      */
      bool LoadRecording(const std::string& filename);

      /**
      * Retrieves the recording data.
      */
      std::vector<Director::RecordThreadData*> GetRecordingData();

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
      DirectorGraph* GetGraphRoot();

      /**
       * Retrieves a graph of the given id.
       *
       * @param[in]  id  The id of the graph.
       *
       * @return     A pointer to the graph or NULL if not found.
       */
      DirectorGraph* GetGraph(const dtCore::UniqueId& id);

      /**
       * Retrieves a node of the given the id.
       *
       * @param[in]  id  The id of the node.
       *
       * @return     A pointer to the node or NULL if not found.
       */
      Node* GetNode(const dtCore::UniqueId& id);

      /**
       * Retrieves a list of nodes that are of a certain type.
       *
       * @param[in]   name      The type name of the node.
       * @param[in]   category  The type category of the node.
       * @param[out]  outNodes  A list of nodes found.
       */
      void GetNodes(const std::string& name, const std::string& category, std::vector<Node*>& outNodes);

      /**
       * Retrieves a list of nodes that are of a certain type,
       * and contain a property with a given value.
       *
       * @param[in]   name      The type name of the node.
       * @param[in]   category  The type category of the node.
       * @param[in]   property  The name of the property to find.
       * @param[in]   value     The value of the property.
       * @param[out]  outNodes  A list of nodes found.
       */
      void GetNodes(const std::string& name, const std::string& category, const std::string& property, const std::string& value, std::vector<Node*>& outNodes);

      /**
       * Retrieves a list of all nodes in the Director.
       *
       * @param[out]  outNodes  A list of nodes found.
       */
      void GetAllNodes(std::vector<Node*>& outNodes);

      /**
       * Retrieves a value node with the given name.
       * @note  If there are more than one value with the
       *         same name, only the first one will be returned.
       *
       * @param[in]  name  The name of the value to find.
       *
       * @return     A pointer to the value node, NULL if not found.
       */
      ValueNode* GetValueNode(const std::string& name);

      /**
       * Deletes a graph.
       *
       * @param[in]  graph  The graph to remove.
       *
       * @return     True if the graph was found and removed.
       */
      bool DeleteGraph(const dtCore::UniqueId& id);

      /**
       * Deletes a node.
       *
       * @param[in]  node  The node to remove.
       *
       * @return     True if the node was found and removed.
       */
      bool DeleteNode(const dtCore::UniqueId& id);

   protected:

      /**
       *	Protected Destructor.  dtCore::RefPtr will handle its destruction.
       */
      virtual ~Director();

      /**
       * Updates a thread.
       *
       * @param[in]  data       The thread data.
       * @param[in]  simDelta   The simulation time step.
       * @param[in]  delta      The real time step.
       * @param[out] continued  Return parameter that tells whether this update
       *                        should be called again immediately.
       *
       * @return     Returns false if the thread should stop.
       */
      bool UpdateThread(ThreadData& data, float simDelta, float delta, bool& continued);

      /**
       * Processes an updated node.
       *
       * @param[in]  node       The node.
       * @param[in]  first      True if this is the nodes first update.
       * @param[in]  continued  True if this node will continue to update.
       * @param[in]  input      The input index that was fired.
       * @param[in]  outputs    A list of outputs that were activated.
       */
      void ProcessUpdatedNode(Node* node, bool first, bool continued, int input, std::vector<OutputLink*> outputs);

      /**
      * Writes a list of recorded thread data.
      *
      * @param[in]  file     The file to save to.
      * @param[in]  threads  The list of threads.
      *
      * @return     True if the file was saved successfully.
      */
      bool WriteRecordThreads(FILE* file, std::vector<RecordThreadData*>& threads);

      /**
      * Reads a list of recorded thread data.
      *
      * @param[in]  file     The file to load from.
      * @param[in]  threads  The list of threads to output.
      *
      * @return     True if the file was loaded successfully.
      */
      bool ReadRecordThreads(FILE* file, std::vector<RecordThreadData*>& threads);

      /**
      * Writes a list of recorded node data.
      *
      * @param[in]  file   The file to save to.
      * @param[in]  nodes  The list of nodes.
      *
      * @return     True if the file was saved successfully.
      */
      bool WriteRecordNodes(FILE* file, std::vector<RecordNodeData>& nodes);

      /**
      * Reads a list of recorded node data.
      *
      * @param[in]  file   The file to read from.
      * @param[in]  nodes  The list of nodes to output.
      *
      * @return     True if the file was saved successfully.
      */
      bool ReadRecordNodes(FILE* file, std::vector<RecordNodeData>& nodes);

      /**
      * Writes a string to a file.
      *
      * @param[in]  file  The file.
      * @param[in]  str   The string to write.
      */
      bool WriteString(FILE* file, const std::string& str);

      /**
      * Reads a string from a file.
      *
      * @param[in]  file  The file.
      * @param[in]  str   The string to read.
      */
      bool ReadString(FILE* file, std::string& str);

   private:

      // Thread Queue.
      struct ThreadQueue
      {
         Node* node;
         int   input;
         bool  isStack;
      };

      // Thread stacks.
      struct StackData
      {
         Node* node;
         int   index;
         bool  first;

         std::vector<ThreadData> subThreads;
         int currentThread;
      };

      // Execution threads.
      struct ThreadData
      {
         std::vector<StackData> stack;

         RecordThreadData* recordThread;
      };

      // Thread Data.
      std::vector<ThreadData> mThreads;
      int mCurrentThread;

      std::vector<ThreadQueue> mThreadQueue;
      bool mQueueingThreads;

      // Recording Data.
      bool  mRecording;
      float mRecordTime;
      std::vector<RecordThreadData*> mRecordThreads;

      // Other Data.
      dtCore::UniqueId mPlayer;

      // Core Info.
      std::string mScriptName;
      std::string mDescription;
      std::string mAuthor;
      std::string mCopyright;
      std::string mCreationTime;

      dtCore::RefPtr<dtDAL::Map> mMap;
      bool        mModified;
      bool        mStarted;

      std::vector<std::string> mLibraries;
      std::map<std::string, std::string> mLibraryVersionMap;

      dtCore::RefPtr<DirectorGraph> mGraph;

      bool           mLogNodes;
      dtUtil::Log*   mLogger;

      dtGame::GameManager* mGameManager;
      dtCore::RefPtr<dtDirector::MessageGMComponent> mMessageGMComponent;

      dtCore::ObserverPtr<Director> mParent;

      // Switch to enable/disable this director
      DT_DECLARE_ACCESSOR_INLINE(bool, Active)
   };
}

#endif // DIRECTOR_DIRECTOR
