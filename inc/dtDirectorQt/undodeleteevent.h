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

#ifndef DIRECTORQT_UNDO_DELETE_EVENT
#define DIRECTORQT_UNDO_DELETE_EVENT

#include <dtDirectorQt/undoevent.h>

#include <dtDirector/node.h>
#include <dtDirector/inputlink.h>
#include <dtDirector/outputlink.h>
#include <dtDirector/valuelink.h>
#include <dtDirector/valuenode.h>

#include <dtCore/refptr.h>

#include <vector>


namespace dtDirector
{
   class DirectorEditor;
   class DirectorGraph;

   /**
    * Undo event for a property change.
    */
   class UndoDeleteEvent: public UndoEvent
   {
   public:

      /**
       * Constructor.
       *
       * @param[in]  editor    The editor.
       * @param[in]  nodeID    The ID of the node being deleted.
       * @param[in]  parentID  The ID of the parent graph that owns this node.
       */
      UndoDeleteEvent(DirectorEditor* editor,
                      const dtDirector::ID& nodeID,
                      const dtDirector::ID& parentID);

      /**
       * Perform undo.
       */
      virtual void Undo();
      void Undo(bool isParent);

      /**
       * Perform redo.
       */
      virtual void Redo();
      void Redo(bool isParent);

   protected:

      /**
       * Destructor.
       */
      virtual ~UndoDeleteEvent();

      /**
       * Saves the link data.
       *
       * @param[in]  link  The link to save.
       */
      void SaveLink(InputLink& link);
      void SaveLink(OutputLink& link);
      void SaveLink(ValueLink& link);
      void SaveLink(ValueNode* node);

      /**
       * Restores the links.
       */
      void RestoreLinks();

      /**
       * Creates a sub deletion event for a node.
       *
       * @param[in]  node  The node.
       */
      void CreateSubEventForNode(Node* node);

      /**
       * Creates a sub deletion event for a graph.
       *
       * @param[in]  graph  The graph.
       */
      void CreateSubEventForGraph(DirectorGraph* graph);

      struct PropData
      {
         std::string mPropName;
         std::string mValue;
      };

      struct LinkData
      {
         dtDirector::ID mSourceID;
         dtDirector::ID mDestID;

         std::string mSource;
         std::string mDest;

         bool mVisible;
      };

      dtDirector::ID mParentID;
      dtDirector::ID mNodeID;

      std::string mName;
      std::string mCategory;

      std::vector<LinkData> mInputs;
      std::vector<LinkData> mOutputs;
      std::vector<LinkData> mValues;

      std::vector<PropData> mProperties;
      std::vector<dtCore::RefPtr<UndoDeleteEvent> > mSubEvents;
   };
}

#endif // DIRECTORQT_UNDO_DELETE_EVENT
