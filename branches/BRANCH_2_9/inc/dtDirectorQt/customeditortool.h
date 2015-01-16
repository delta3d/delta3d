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

#ifndef DIRECTORQT_CUSTOM_EDITOR_TOOL
#define DIRECTORQT_CUSTOM_EDITOR_TOOL

#include <dtDirectorQt/export.h>

#include <dtDirectorQt/directoreditor.h>

namespace dtDirector
{
   /**
    * @class CustomEditorTool
    *
    * @brief Base class for custom editing tools for special Director sub-Graphs.
    */
   class DT_DIRECTOR_QT_EXPORT CustomEditorTool
   {
   public:
      /**
       * Constructs the custom editor.
       *
       * @param[in]  name  The name of the tool.
       */
      CustomEditorTool(const std::string& name);

      /**
       * Destructs the custom editor.
       */
      virtual ~CustomEditorTool();

      /**
       * Retrieves the name of the tool.
       */
      const std::string& GetName() const {return mToolName;}

      /**
       * Retrieves whether this tool can be used with the given
       * director script.
       *
       * @param[in]  director  The director script.
       */
      virtual bool IsDirectorSupported(Director* director) const;

      /**
       * Retrieves the main Director Editor.
       *
       * @return  The editor.
       */
      DirectorEditor* GetEditor() const {return mEditor;}

      /**
       * Retrieves the currently openned Director Graph.
       *
       * @return  The Director Graph.
       */
      DirectorGraph* GetGraph() const {return mGraph;}

      /**
       * Event handler when this tool should be opened for the given graph.
       * @note:  This method should be overloaded to perform any initial
       * operations when this tool is activated.
       *
       * @param[in]  editor  The editor that is using this tool.
       * @param[in]  graph   The graph to open the tool for.
       */
      virtual void Open(DirectorEditor* editor, DirectorGraph* graph);

      /**
       * Event handler to close the tool.
       * @note:  This method should be overloaded to perform any shut down
       * operations when this tool is deactivated.
       */
      virtual void Close();

      /**
       * Retrieves whether the editor is currently open.
       *
       * @return  True if the editor is currently open.
       */
      bool IsOpen() const {return mIsOpen;}

      /**
       * Handles all common actions at the beginning of a save:
       * - Saves all current external link node connections.
       * - Clears all current nodes in the graph.
       * - Initializes the undo manager for all changes to be made during
       *    the save operation.
       *
       * @return  True if initialization was successful.
       */
      virtual bool BeginSave();

      /**
       * Handles all common actions to finalize a save operation:
       * - Re-connects all external link node connections.
       * - Organizes the positions of all nodes based on their connections.
       * - Finalizes all node changes to the undo manager.
       *
       * @return  True if the finalization was successful.
       */
      virtual bool EndSave();

      /**
       * Creates a new node.  Any non-value node created will automatically
       * be positioned to the right of the given Chained Node.  If no
       * chained node is specified, the new node will be placed in its
       * own empty row.  For value nodes, the positioning is done during
       * the Connect method.
       *
       * The node height determines how far below any new nodes are
       * positioned relative to this newly created node.
       *
       * @param[in]  name         Name of the node to create.
       * @param[in]  category     Category name of the node to create.
       * @param[in]  chainedNode  The node to chain with.
       * @param[in]  nodeHeight   The estimated height of the node being created.
       * @param[in]  nodeWidth    The estimated width of the node being created.
       *
       * @return     A pointer to the newly created node.
       */
      dtDirector::Node* CreateNode(const std::string& name, const std::string& category, dtDirector::Node* chainedNode = NULL, int nodeHeight = 400, int nodeWidth = 400);

      /**
       * Connects an output on a node to the input of another node.
       *
       * @param[in]  node1       The left node to connect.
       * @param[in]  node2       The right node to connect.
       * @param[in]  outputName  The name of the output link on Node2.
       * @param[in]  inputName   The name of the input link on Node1.
       *
       * @return     True if the connection was made.
       */
      bool Connect(dtDirector::Node* node1, dtDirector::Node* node2, const std::string& outputName, const std::string& inputName);

      /**
       * Connects a value link on a node to a value node.  This will also
       * re-position the value node so it is beneath the link it is
       * connected to.
       *
       * @param[in]  node         The node to connect.
       * @param[in]  valueNode    The value node to connect to.
       * @param[in]  linkName     The name of the value link on node.
       */
      bool Connect(dtDirector::Node* node, dtDirector::Node* valueNode, const std::string& linkName);

      /**
       * Auto positions a node to the right of a given chained now.
       * If no chained node is provided, the node will be auto positioned
       * in its own empty row.
       * Note: The CreateNode and Connect to value node methods are
       * already designed to automatically position the node.
       *
       * @param[in]  node         The node to position.
       * @param[in]  chainedNode  The node to chain with.
       * @param[in]  nodeHeight   The estimated height of the node being positioned.
       * @param[in]  nodeWidth    The estimated width of the node being positioned.
       */
      void AutoPositionNode(dtDirector::Node* node, dtDirector::Node* chainedNode, int nodeHeight, int nodeWidth = 400);

      /**
       * Retrieves a list of all input links connected to a given nodes
       * output link.
       * the outNodes value is not cleared before this operation.
       *
       * @param[in]   node      The current node.
       * @param[in]   linkName  The output link to search.
       * @param[out]  outLinks  The list of input links connected.
       *
       * @return      False if data could not be found.
       */
      bool GetNext(dtDirector::Node* node, const std::string& linkName, std::vector<dtDirector::InputLink*>& outLinks);

      /**
       * Retrieves a list of all output links connected to a given nodes
       * input link.
       * the outNodes value is not cleared before this operation.
       *
       * @param[in]   node      The current node.
       * @param[in]   linkName  The input link to search.
       * @param[out]  outLinks  The list of output links connected.
       *
       * @return      False if data could not be found.
       */
      bool GetPrev(dtDirector::Node* node, const std::string& linkName, std::vector<dtDirector::OutputLink*>& outLinks);

   protected:

   private:

      struct ChainData
      {
         dtDirector::Node* node;
         dtDirector::Node* parent;
         osg::Vec2         offset;
      };

      struct ConnectionData
      {
         bool visible;
         std::string sourceNode;

         dtDirector::Node* destNode;
         std::string destNodeLink;
      };

      bool              mIsOpen;

      std::string       mToolName;
      DirectorEditor*   mEditor;

      DirectorGraph*    mGraph;

      std::vector<ConnectionData> mInputConnections;
      std::vector<ConnectionData> mOutputConnections;
      std::vector<ConnectionData> mValueConnections;

      std::map<dtDirector::Node*, ChainData> mChainedNodeMap;
      int mRowHeight;
   };

} // namespace dtDirector

#endif // DIRECTORQT_CUSTOM_EDITOR_TOOL
