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

#ifndef DIRECTORQT_CLIPBOARD
#define DIRECTORQT_CLIPBOARD


#include <dtDirector/node.h>
#include <dtCore/uniqueid.h>
#include <dtDAL/propertycontainer.h>

#include <osg/Vec2>

#include <vector>


namespace dtDirector
{
   class DirectorGraph;
   class Node;
   class UndoManager;

   /**
    * Clipboard manager for copy/paste management of Director Nodes.
    */
   class Clipboard
   {
   public:

      /**
       * Retrieves a static instance of the clipboard.
       */
      static Clipboard& GetInstance();

      /**
       * Clears all objects saved to the clipboard.
       */
      void Clear();

      /**
       * Adds an object to the clipboard.
       *
       * @param[in]  object  The object to add.
       */
      void AddObject(dtDAL::PropertyContainer* object);

      /**
       * Removes an object from the clipboard.
       *
       * @param[in]  object  The object to remove.
       */
      void RemoveObject(dtDAL::PropertyContainer* object);

      /**
       * Pastes the contents of the clipboard to a Director graph
       * at a given position.
       *
       * @param[in]  parent       The parent Director Graph to copy to.
       * @param[in]  undoManager  The undo manager.
       * @param[in]  position     The top/left most position to paste nodes.
       *
       * @return     The list of nodes that were newly created.
       */
      std::vector<dtDAL::PropertyContainer*> PasteObjects(DirectorGraph* graph, UndoManager* undoManager, const osg::Vec2& position = osg::Vec2());

      /**
       * Retrieves whether there are items copied to the clipboard.
       */
      bool CanPaste() {return !mCopied.empty();}

   private:

      /**
       * Copies an object.
       *
       * @param[in]  object    The object to copy.
       * @param[in]  parent    The parent Director Graph to copy to.
       * @param[in]  position  The top/left most position to paste nodes.
       *
       * @return     A pointer to the newly created object.
       */
      dtDAL::PropertyContainer* CopyObject(dtDAL::PropertyContainer* object, DirectorGraph* graph, const osg::Vec2& position = osg::Vec2());

      /**
       * Connects all the links on a pasted node.
       *
       * @param[in]  node    The node to link.
       * @param[in]  parent  The parent Director Graph to copy to.
       */
      void LinkNode(Node* node, DirectorGraph* parent);

      /**
       * Links all inputs to outputs for a node.
       *
       * @param[in]  link      The new link that needs connecting.
       * @param[in]  fromLink  The original link with connections to copy from.
       * @param[in]  parent    The parent Director Graph to copy to.
       */
      void LinkInputs(InputLink* link, InputLink* fromLink, DirectorGraph* parent);

      /**
       * Links all outputs to inputs for a node.
       *
       * @param[in]  link      The new link that needs connecting.
       * @param[in]  fromLink  The original link with connections to copy from.
       * @param[in]  parent    The parent Director Graph to copy to.
       */
      void LinkOutputs(OutputLink* link, OutputLink* fromLink, DirectorGraph* parent);

      /**
       * Links all value links with value nodes.
       *
       * @param[in]  link      The new link that needs connecting.
       * @param[in]  fromLink  The original link with connections to copy from.
       * @param[in]  parent    The parent Director Graph to copy to.
       */
      void LinkValues(ValueLink* link, ValueLink* fromLink, DirectorGraph* parent);

      std::vector<dtCore::RefPtr<dtDAL::PropertyContainer> > mCopied;
      osg::Vec2  mOffset;
      bool       mFirstObject;

      std::vector<Node*> mPasted;
      std::vector<DirectorGraph*> mPastedGraphs;
      std::map<dtCore::UniqueId, Node*> mIDNewToOld;
      std::map<dtCore::UniqueId, Node*> mIDOldToNew;

      static Clipboard* mInstance;
   };
}

#endif
