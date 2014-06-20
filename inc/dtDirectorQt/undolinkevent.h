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

#ifndef DIRECTORQT_UNDO_LINK_EVENT
#define DIRECTORQT_UNDO_LINK_EVENT

#include <dtDirectorQt/undoevent.h>
#include <dtDirector/node.h>

namespace dtDirector
{
   class DirectorEditor;

   /**
    * Undo event for a property change.
    */
   class UndoLinkEvent: public UndoEvent
   {
   public:

      enum LinkType
      {
         INPUT_LINK,
         VALUE_LINK,
      };

      /**
       * Constructor.
       *
       * @param[in]  editor          The editor.
       * @param[in]  type            The link type.
       * @param[in]  sourceID        The source node ID.
       * @param[in]  destID          The destination node ID.
       * @param[in]  sourceLinkName  The source link name.
       * @param[in]  destLinkName    The destination link name.
       * @param[in]  connection      Connecting or disconnecting?
       */
      UndoLinkEvent(DirectorEditor* editor,
                    LinkType type,
                    const dtDirector::ID& sourceID,
                    const dtDirector::ID& destID,
                    const std::string& sourceLinkName,
                    const std::string& destLinkName,
                    bool connecting);

      /**
       * Perform undo.
       */
      virtual void Undo();

      /**
       * Perform redo.
       */
      virtual void Redo();

   protected:

      /**
       * Connects the link.
       */
      void ConnectLink();

      /**
       * Disconnects the link.
       */
      void DisconnectLink();

      /**
       * Destructor.
       */
      virtual ~UndoLinkEvent();

      LinkType mType;

      dtDirector::ID mSourceID;
      dtDirector::ID mDestID;

      std::string mSource;
      std::string mDest;

      bool mConnecting;
   };
}

#endif // DIRECTORQT_UNDO_LINK_EVENT
